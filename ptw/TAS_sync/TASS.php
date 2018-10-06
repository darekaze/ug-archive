<?php
// TAS Sync program
/**
* @throws Exception if operation fail
*/
function replicateTimeTable($configs, $room, $subject) {
    $roomToID = array();
    $roomToAreaID = array();

    /* // This part Need rbs data
    echo "TASSynchronizer.replicateTimeTable(): Collecting Room Information, sucessful = ";
    $r = getRemoteRoomList($configs->RBS, $roomToID, $roomToAreaID); 
    echo $r . "\n";
    */

    // Get TAS Info
    $staffHT = array();
    $subjectHT = array();
    $teachingRequirementHT = array();

    echo "TASSynchronizer.replicateTimeTable(): Collecting TAS Basic Information, sucessful = ";
    $r = false;
    try {
        $conn = oci_connect($configs->TAS->username, $configs->TAS->password, $configs->TAS->db);
        if (!$conn) die("Connection failed: " . oci_error());

        getStaffHT($conn, $configs->period, $staffHT);
        getSubjectHT($conn, $configs->period, $subjectHT);
        getTeachingRequirementHT($conn, $configs->period, $staffHT, $teachingRequirementHT);
        oci_close($conn);
        $r = true;
    } catch (Exception $e) {
        echo $e->getMessage();
    }		
    echo $r . "\n\n";

    // Make conditions
    $condition="";
    $delCondition="";
    
    $condition = "{$condition}a.Period='{$configs->period}' and a.STerm<={$configs->sem} and {$configs->sem}<=a.ETerm"; 
    $delCondition = "{$delCondition}tas_import=1 and tas_period='{$configs->period}' and tas_sem='{$configs->sem}'";
    if ($room !== null) {
        $condition = "{$condition} and venue='{$room}'"; 
        // $delCondition = "{$delCondition} and room_id={$roomToID['room']}"; // Need data from rbs
    }
    if ($subject !== null) {
        $condition = "{$condition} and a.subject_code='{$subject}'"; 
        $delCondition = "{$delCondition} and tas_subject_code='{$subject}'";
    }

    try {
        echo "Replicating Assignment TimeTable having condition {$condition}";
        echo "\nTASSynchronizer.replicateTimeTable() : Connecting to DB {$configs->TAS->db} by {$configs->TAS->username}\n";

        $conn = oci_connect($configs->TAS->username, $configs->TAS->password, $configs->TAS->db);
        if (!$conn) die("Connection failed: " . oci_error());

        $query = "select JobNo,subject_code,shour,ehour,wday,venue from assignment_timetable a where {$condition}" . 
            " group by JobNo,subject_code,shour,ehour,wday,venue" . 
            " order by a.subject_code";
        $stid = oci_parse($conn, $query);
        // oci_execute($stid) ? delRepetition($configs->RBS, $delCondition) : null; // Need rbs data
        
        // TAS Synchronizer start replicate time table
        $count = 0;
        $done = 0;

        // TODO: 10/6 continue from here
        while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
            $ht = array();
            $count++;

            $jobno = $row["JOBNO"];
            $subjectCode = $row["SUBJECT_CODE"];
            $start_seconds = convertToSeconds($row["SHOUR"]);
            $end_seconds = convertToSeconds($row["EHOUR"]);
            $rep_day = convertToDayOfWeek($row["WDAY"]);
            $venue = $row["VENUE"];

            // If error, try changing back to use exception
            echo "TASSynchronizer.replicateTimeTable(): Processing {$subjectCode} on {$rep_day} {$start_seconds}-{$end_seconds} at {$venue}";
            $subjectTitle = $subjectHT[$subjectCode]["SUBJECT_TITLE"];
            if($subjectTitle == null || $subjectTitle == "") {
                throw new Exception("*** ERROR: TASSynchronizer.replicateTimeTable(): subject title of {$subjectCode} not available");
            }

            $sNameList = getStaffNameList($teachingRequirementHT[$jobno]["staffHT"]); // return StaffNameList in string
            $description = "{$subjectTitle} ({$sNameList})";
            echo "TASSynchronizer.replicateTimeTable(): by {$sNameList}";
            if($sNameList == null || $sNameList == "") {
                throw new Exception("*** ERROR: TASSynchronizer.replicateTimeTable(): Teaching Requirement of {$jobno} subject code {$subjectCode} not available");
            }

            // TODO: Need test
            if ($rep_day != "-1" && $roomToAreaID[$venue] !== null && $roomToID[$venue] !== null) {
                $synDate = getCurrentDateFormatted();
                $done++;
                $ht = array(
                    "name" => $subjectCode,
                    "description" => $description,
                    "start_day" => $configs->start_day,
                    "start_month" => $configs->start_month,
                    "start_year" => $configs->start_year,
                    "start_seconds" => $start_seconds,
                    "end_day" => $configs->start_day,
                    "end_month" => $configs->start_month,
                    "end_year" => $configs->start_year,
                    "end_seconds" => $end_seconds,
                    "area" => $roomToAreaID[$venue],
                    "rooms[]" => $roomToID[$venue],
                    "type" => "I",
                    "confirmed" => "1",
                    "private" => "0",
                    "f_tas_import" => "1",					
                    "f_tas_period" => $configs->period,
                    "f_tas_sem" => $configs->sem,
                    "f_tas_user_comp_acc" => "",
                    "rep_type" => "2",
                    "rep_end_day" => $configs->end_day,
                    "rep_end_month" => $configs->end_month,
                    "rep_end_year" => $configs->end_year,
                    "rep_day[]" => $rep_day,
                    "rep_num_weeks" => "",
                    "returl" => "",
                    "create_by" => "cspaulin",
                    "rep_id" => "0",
                    "edit_type" => "series",
                    "f_tas_subject_code" => $subjectCode,
                    "f_tas_syndate" => $synDate
                );

                callInsertBookingURL($ht);
            } else {
                echo "Not replicating {$subjectCode} by {$sname} {$wday} {$shour}-{$ehour} at {$venue}\n";
            }
        }

        echo "Count Matching condition = {$count}, done = {$done}\n";
        oci_close($conn);

        echo "TASSynchronizer.replicateTimeTable() : Finished\n\n";

    } catch (Exception $e) {
        echo $e->getMessage();
    } 	
}

// Test: Done
function getStaffHT($conn, $period, &$staffHT) {
    $query = "select sid,sname from staff where period='{$period}'";
    $stid = oci_parse($conn, $query);
    oci_execute($stid);
    while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
        $sid = $row["SID"];
        $sname = $row["SNAME"];
        $staff = array($sid,$sname); // Staff

        $staffHT[$sid] = $staff;
    }
    oci_free_statement($stid);
}

// Test: Done
function getSubjectHT($conn, $period, &$subjectHT) {
    $query = "select subject_code,subject_title from subject where period='{$period}'";
    $stid = oci_parse($conn, $query);
    oci_execute($stid);
    while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
        $code = $row["SUBJECT_CODE"];
        $title = $row["SUBJECT_TITLE"];
        $subject = array($code,$title);
        
        $subjectHT[$code] = $subject;
    }
    oci_free_statement($stid);
}

// Test: Done
function getTeachingRequirementHT($conn, $period, $staffHT, &$teachingRequirementHT) {
    $query = "select jobno,subject_code,c_code,a_code from Teaching_Requirement where period='{$period}'";
    $stid = oci_parse($conn, $query);
    oci_execute($stid);
    while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
        $jobno = $row["JOBNO"];
        $s_code = $row["SUBJECT_CODE"];
        $c_code = $row["C_CODE"];
        $a_code = $row["A_CODE"];

        $teachingRequirement = array($jobno,$s_code,$c_code,$a_code);
        $teachingRequirement["staffHT"] = getTeachingRequirementStaff($conn,$staffHT,$period,$jobno);

        $teachingRequirementHT[$jobno] = $teachingRequirement;
    }
    oci_free_statement($stid);
}

// Test: Done
function getTeachingRequirementStaff($conn, &$staffHT, $period, $jobno) {
    $sHT = array();
    $query = "select sid from assignment_timetable where jobno='{$jobno}' and period='{$period}'";
    $stid = oci_parse($conn, $query);
    oci_execute($stid);
    while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
        $sid = $row["SID"];
        $sHT[$sid] = $staffHT[$sid];
    }
    oci_free_statement($stid);
    return $sHT;
}

function getRemoteRoomList($rbs, &$roomToID, &$roomToAreaID) {
    $r = false;
    try {
        $rbsconn = new mysqli($rbs->db, $rbs->username, $rbs->password);
        if ($rbsconn->connect_error) {
            throw new Exception("Connection failed: " . $rbsconn->connect_error);
        }
        $sql = "select id,room_name,area_id from mrbs_room order by id";
        $rs = $rbsconn->query($sql);
        if ($rs->num_rows > 0) {
            while ($row = $rs->fetch_assoc()) {
                $id = $row["ID"];
                $room = $row["ROOM_NAME"];
                $area_id = $row["AREA_ID"];

                $roomToID[$room] = $id;
                $roomToAreaID[$room] = $area_id;
            }
        }
        $rbsconn->close();            
        $r = true;           		
    } catch (Exception $e) {
        echo $e->getMessage();
    }
    return $r;
}

function delRepetition($rbs, $delCondition) {
    $rbsconn = new mysqli($rbs->db, $rbs->username, $rbs->password);
    if ($rbsconn->connect_error) {
        die("Connection failed: " . $rbsconn->connect_error);
    }
    echo "Removing record from RBS before replicatiion for condition {$delCondition} .... ";
    
    $sql = "delete from mrbs_entry where {$delCondition}";
    if ($rbsconn->query($sql) === true) {
        echo "Record deleted successfully";
    } else {
        echo "Error deleting record: " . $rbsconn->error;
    }

    $sql = "delete from mrbs_repeat where {$delCondition}";
    if ($rbsconn->query($sql) === true) {
        echo "Record deleted successfully";
    } else {
        echo "Error deleting record: " . $rbsconn->error;
    }
    $rbsconn->commit();  
    $rbsconn->close(); 
    echo "Done!\n";
}

// Do mock test
/**
 * @throws Exception if operation fail
 */
function callInsertBookingURL($ht, $loginURL, $rbsURL) {
    // Get login form
    $nvps = array(
        'NewUserName' => '<username>',
        'NewUserPassword' => '<password>',
        'returl' => '',
        'TargetURL' => 'admin.php?',
        'Action' => 'SetName',
        'submit' => ' Log in '
    );
    $options = array(
        'http' => array(
            'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
            'method'  => 'POST',
            'content' => http_build_query($nvps)
        )
    );
    $context = stream_context_create($options);
    $result = file_get_contents($loginURL, false, $context);
    if ($result === false) { 
        throw new Exception("Error occur");
    }
    echo "Login form get: \n";
    var_dump($result);

    // Post to rbs
    $nvps = getNameValuePair($ht);
    $options = array(
        'http' => array(
            'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
            'method'  => 'POST',
            'content' => http_build_query($nvps)
        )
    );
    $context  = stream_context_create($options);
    $result = file_get_contents($rbsURL, false, $context);
    if ($result === false) { 
        throw new Exception("Error occur");
    }
    
    echo "Send Data form get: \n";
    var_dump($result);
}


// ---------------util function--------------//

function convertToSeconds($t) {
    return (strtotime($t) - strtotime('TODAY'));
}

function convertToDayOfWeek($t) {
    return date('N', strtotime($t));
}

function getCurrentDateFormatted() {
    date_default_timezone_set('Asia/Hong_Kong');
    $date = new DateTime();
    return "{$date->format('Y-m-d h:i:s')}";
}

function getNameValuePair($fieldList ,$ht) {
    $nvps = array();
    foreach($fieldList as $field)
        $nvps[$field] = $ht[$field];
    return $nvps;
}

function getQueryString($fieldList, $ht) {
    $r = "";
    foreach($fieldList as $field)
        $r = "{$field}={$ht[$field]}&";
    return $r;
}

function getStaffNameList($staffHT) {
    $str = "";
    foreach($staffHT as $sid => $staff) {
        $str = $str . " " . $staff[1];
    }
    return $str;
}

function displayResponseContent($entity) {
    echo "Writing Content of Response" . html_entity_decode($entity);
}

function displayErrorMessage($entity) { // [HttpEntity entity]
    $content = html_entity_decode($entity);
    $display = explode('<!--###Begin Error Message###', $content);
    $display = explode('###End Error Message###-->', $display[1]);
    echo $display[0];
}

//-----------Test connection-----------//

function testRemoteConn($rbs) {
    try {
        $rbsconn = new mysqli($rbs->db, $rbs->username, $rbs->password);
        if ($rbsconn->connect_error) {
            throw new Exception("Connection failed: " . $rbsconn->connect_error . "\n");
        }
        $rbsconn->close();
        echo "TASSynchronizer.testRemoteConn : Finished\n";

    } catch (Exception $e) {
        echo "TASSynchronizer.testRemoteConn : Error while creating ORA-Conn Object\n";
        echo $e->getMessage();	
    }
}

function testLocalConn($tas) {
    try {
        echo "TASSynchronizer.testLocalConn : Using Oracle\n";
        echo "TASSynchronizer.testLocalConn : Connecting to {$tas->db} by {$tas->username} with password length " . strlen($tas->password) . "\n";
        
        $conn = oci_connect($tas->username, $tas->password, $tas->db);
        if (!$conn) {
            throw new Exception("Connection failed: " . oci_error() . "\n");
        }
        
        $period = '2018-2019';
        // can add test case here

        oci_close($conn);
        echo "TASSynchronizer.testLocalConn : Finished\n";

    } catch (Exception $e) {
        echo "TASSynchronizer.testLocalConn : Error while creating ORA-Conn Object\n";
        echo $e->getMessage();
    }
}

//---------------Main------------------//
/**
* @throws Exception if operation fail
*/
function start() {
    $configs = include('config.php');
    echo "Replicating TAS Timetable\n";	
    // testLocalConn($configs->TAS);
    replicateTimeTable($configs, null, null);
}

start();

?>