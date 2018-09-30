<?php
$roomToID = array();
$idToRoom = array();
$roomToAreaID = array();

$subjectHT = array();
$staffHT = array();
$teachingRequirementHT = array();

$fieldList = array(
    "name","description",
    "start_day","start_month","start_year","start_seconds",
    "end_day","end_month","end_year","end_seconds",
    "area","rooms[]","type","confirmed","private",
    "f_tas_import","f_tas_period","f_tas_sem","f_tas_user_comp_acc","f_tas_subject_code",
    "rep_type","rep_end_day","rep_end_month","rep_end_year","rep_day[]","rep_num_weeks",
    "returl","create_by","rep_id","edit_type","f_tas_syndate"
);

/**
* @throws Exception if operation fail
*/
function replicateTimeTable($period, $sem, $room, $subject) {
    $configs = init();
    $rst = "";
    $condition=""; 
    $delCondition="";

    echo "TASSynchronizer.replicateTimeTable(): Collecting Room Information, sucessful = ";
    $r = getRemoteRoomList();
    echo $r . "\n";

    echo "TASSynchronizer.replicateTimeTable(): Collecting TAS Basic Information, sucessful = ";
    $r = getTASInfo($configs->period);
    echo $r . "\n\n";

    if ($condition === null) $condition = "";
    $condition = "{$condition}a.Period='{$period}' and a.STerm<={$sem} and {$sem}<=a.ETerm"; 
    $delCondition = "{$delCondition}tas_import=1 and tas_period='{$period}' and tas_sem='{$sem}'";
    
    if ($room !== null) {
        $condition = "{$condition} and venue='{$room}'"; 
        $delCondition += "{$delCondition} and room_id={$this->$roomToID['room']}";
    }
    if ($subject !== null) {
        $condition = "{$condition} and a.subject_code='{$subject}'"; 
        $delCondition += "{$delCondition} and tas_subject_code='{$subject}'";
    }

    try {	
        echo "Replicating Assignment TimeTable having condition {$condition}";
        echo "\nTASSynchronizer.replicateTimeTable() : Connecting to DB {$configs['db_server']} by {$configs['username']}\n";

         // TODO: TAS Oracle connection
        
    } catch (Exception $e) {
        echo $e->getMessage();
    } 	
    return $r;	
}

function getRemoteRoomList() {
    GLOBAL $roomToID, $idToRoom, $roomToAreaID;
    $configs = init();
    $r = false;
    try {
        $rbsconn = new mysqli($configs->rbs_db, $configs->rbs_username, $configs->rbs_password);
        if ($rbsconn->connect_error) {
            throw new Exception("Connection failed: " . $rbsconn->connect_error);
        }
        $sql = "select id,room_name,area_id from mrbs_room order by id";
        $rs = $rbsconn->query($sql);
        if ($rs->num_rows > 0) {
            while ($row = $rs->fetch_assoc()) {
                $id = $row["id"];
                $room = $row["room_name"];
                $area_id = $row["area_id"];

                $roomToID[$room] = $id;
                $idToRoom[$id] = $room;
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

function getTASInfo($period) {
    GLOBAL $staffHT, $subjectHT, $teachingRequirementHT;
    $configs = init();
    $r = false;

    try {
        $conn = oci_connect($configs->TAS_username, $configs->TAS_password, $configs->TAS_db);
        if (!$conn) die("Connection failed: " . oci_error());

        // Get Staff hashtable
        $query = "select sid,sname from staff where period='{$period}'";
        $stid = oci_parse($conn, $query);
        oci_execute($stid);
        while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
            $sid = $row["sid"];
            $sname = $row["sname"];
            $staff = array($sid,$sname); // Staff

            $staffHT[$sid] = $staff;
        }
        oci_free_statement($stid);
        
        // Get Subject hashtable
        $query = "select subject_code,subject_title from subject where period='{$period}'";
        $stid = oci_parse($conn, $query);
        oci_execute($stid);
        while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
            $code = $row["subject_code"];
            $title = $row["subject_title"];
            $subject = array($code,$title);
            
            $subjectHT[$code] = $subject;
        }
        oci_free_statement($stid);

        // Get Teaching requirement hashtable
        $query = "select jobno,subject_code,c_code,a_code from Teaching_Requirement where period='{$period}'";
        $stid = oci_parse($conn, $query);
        oci_execute($stid);
        while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
            $jobno = $row["jobno"];
            $s_code = $row["subject_code"];
            $c_code = $row["c_code"];
            $a_code = $row["a_code"];
            $teachingRequirement = array($jobno,$s_code,$c_code,$a_code);
            $teachingRequirement["staffHT"] = getTeachingRequirementStaff($jobno,$period,$conn);

            $teachingRequirementHT[$jobno] = $teachingRequirement;
        }
        oci_free_statement($stid);
        oci_close($conn);
        $r = true; 

    } catch (Exception $e) {
        echo $e->getMessage();
    }	
    return $r;	
}

function getTeachingRequirementStaff($jobno, $period, $conn) {
    GLOBAL $staffHT;
    $configs = init();
    $sHT = array();
    try {
        $query = "select sid from assignment_timetable where jobno='{$jobno}' and period='{$period}'";
        $stid = oci_parse($conn, $query);
        oci_execute($stid);
        while ($row = oci_fetch_array($stid, OCI_RETURN_NULLS+OCI_ASSOC)) {
            $sid = $row["sid"];
            $sHT[$sid] = $staffHT[$sid];
        }
        oci_free_statement($stid);

    } catch (Exception $e) {
        echo $e->getMessage();
    }
    return $sHT;
}

function callInsertBookingURL($ht) {}


// ---------------Short function-----------//

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

function getNameValuePair($ht) {
    GLOBAL $fieldList;
    $nvps = array();
    foreach($fieldList as $field)
        $nvps[$field] = $ht[$field];
    return $nvps;
}

function getQueryString($ht) {
    GLOBAL $fieldList;
    $r = "";
    foreach($fieldList as $field)
        $r = "{$field}={$ht[$field]}&";
    return $r;
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

//-----------Test functions------------//
// TODO: Finish test functions
function getTestHT() {

}

function testRemoteConn() {

}

function testLocalConn() {

}

//////////////////////////////
/**
* @throws Exception if operation fail
*/
function init() {
    return include('config.php');
}

/**
* @throws Exception if operation fail
*/
function start() {
    $configs = init();
    echo "Replicating TAS Timetable\n";	
    replicateTimeTable($configs->period, $configs->sem, null, null);
}

start();

?>