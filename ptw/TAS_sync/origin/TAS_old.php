<?php

$configs = include('config.php');

class TASSynchronizer {

    protected static $roomToID = array();
    protected static $idToRoom = array();
    protected static $roomToAreaID = array();
    protected static $fieldList = array("name","description","start_day","start_month","start_year","start_seconds","end_day","end_month","end_year","end_seconds","area","rooms[]","type","confirmed","private","f_tas_import","f_tas_period","f_tas_sem","f_tas_user_comp_acc","f_tas_subject_code","rep_type","rep_end_day","rep_end_month","rep_end_year","rep_day[]","rep_num_weeks","returl","create_by","rep_id","edit_type","f_tas_syndate"); 
    
    protected static $subjectHT = array();
    protected static $staffHT = array();
    protected static $teachingRequirementHT = array();
    
    /**
     * @throws Exception if operation fail
     */
    public static function replicateTimeTable($period, $sem, $room, $subject) {
        $rst = "";
        $condition=""; 
        $delCondition="";
        
        echo "TASSynchronizer.replicateTimeTable(): Collecting Room Information, sucessful = ";
        $r = getRemoteRoomList();
        echo $r . "\n";
        
        echo "TASSynchronizer.replicateTimeTable(): Collecting TAS Basic Information, sucessful = ";
        $r = getTASInfo($configs['period']);
        echo $r . "\n";
        
        if ($condition === NULL) $condition = "";
        $condition = "{$condition}a.Period='{$period}' and a.STerm<={$sem} and {$sem}<=a.ETerm"; 
        $delCondition = "{$delCondition}tas_import=1 and tas_period='{$period}' and tas_sem='{$sem}'"; 
        
        if ($room !== NULL) {
            $condition = "{$condition} and venue='{$room}'"; 
            $delCondition += "{$delCondition} and room_id={$this->$roomToID['room']}";
        }
        if ($subject !== NULL) {
            $condition = "{$condition} and a.subject_code='{$subject}'"; 
            $delCondition += "{$delCondition} and tas_subject_code='{$subject}'";
        }

        try {	
            echo "Replicating Assignment TimeTable having condition {$condition}";
            echo "\nTASSynchronizer.replicateTimeTable() : Connecting to DB {$configs['db_server']} by {$configs['username']}\n";

            $conn = new mysqli($server, $username, $password);
            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
            } 
            $sql = "select JobNo,subject_code,shour,ehour,wday,venue from assignment_timetable a where {$condition}" . 
                    " group by JobNo,subject_code,shour,ehour,wday,venue" . 
                    " order by a.subject_code ";
            $rs = $conn->query($sql);
            
            $rbsconn = new mysqli($server, $username, $password);
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
            echo "Done!\n";
        
            $count = 0;
            $done = 0;
            if ($rs->num_rows > 0) {    	                
                while ($row = $rs->fetch_assoc()) {
                    try {
                        $ht = array();
                        $count++;

                        $jobno = $row["jobno"];
                        $subjectCode = $row["subject_code"];
                        $shour = $row["shour"];
                        $start_seconds = "{$this->convertToSeconds($shour)}";
                        $ehour = $row["ehour"];
                        $end_seconds = "{$this->convertToSeconds($ehour)}";
                        $wday = $row["wday"];
                        $rep_day= "{$this->convertToDayOfWeek($wday)}";
                        $venue = $row["venue"];
                        
                        echo "TASSynchronizer.replicateTimeTable(): Processing {$subjectCode} on {$wday} {$shour}-{$ehour} at {$venue}";
                        
                        $subjectTitle = "";
                        try {
                            $subjectTitle = $this->$subjectHT[$subjectCode]["subject_title"];
                        } catch (Exception $e) {
                            echo "*** ERROR: TASSynchronizer.replicateTimeTable(): subject title of {$subjectCode} not available";
                        }
                        
                        $sname = "";
                        $description = "";
                        try {
                            $sname = $this->$teachingRequirementHT[$jobno]->getStaffNameList();
                            $description = "{$subjectTitle} ({$sname})";
                            echo "TASSynchronizer.replicateTimeTable():  by {$sname}";
                        } catch (Exception $e) {
                            echo "*** ERROR: TASSynchronizer.replicateTimeTable(): Teaching Requirement of {$jobno} subject code {$subjectCode} not available";
                        }

                        if ($rep_day != "-1" && $this->$roomToAreaID[$venue] !== NULL && $this->$roomToID[$venue] !== NULL) {
                            $done++;
                            $ht = array(
                                "name" => $subjectCode,
                                "description" => $description,
                                "start_day" => $configs['start_day'],
                                "start_month" => $configs['start_month'],
                                "start_year" => $configs['start_year'],
                                "start_seconds" => "0",
                                "end_day" => $configs['start_day'],
                                "end_month" => $configs['start_month'],
                                "end_year" => $configs['start_year'],
                                "end_seconds" => "0", 
                                "area" => $roomToAreaID[$venue],
                                "rooms[]" => $roomToID[$venue],
                                "type" => "I",
                                "confirmed" => "1",
                                "private" => "0",
                                "f_tas_import" => "1",					
                                "f_tas_period" => $configs['period'],
                                "f_tas_sem" => $configs['sem'],
                                "f_tas_user_comp_acc" => "",
                                "rep_type" => "2",
                                "rep_end_day" => $configs['end_day'],
                                "rep_end_month" => $configs['end_month'],
                                "rep_end_year" => $configs['end_year'],
                                "rep_day[]" => $rep_day,
                                "rep_num_weeks" => "",
                                "returl" => "",
                                "create_by" => "cspaulin",
                                "rep_id" => "0",
                                "edit_type" => "series",
                                "f_tas_subject_code" => subjectCode
                            );
                            ht.put("f_tas_syndate",getCurrentDateFormatted());
                        //----------------
                            $this->callInsertBookingURL($ht);
                        } else {
                            echo "Not replicating {$subjectCode} by {$sname} {$wday} {$shour}-{$ehour} at {$venue}";
                        }
                        echo "\n";
                    } catch (Exception $e1) {
                        echo "*** ERROR: TASSynchronizer.replicateTimeTable(): {$e->getMessage()}";
                    }
                } 
            }
            echo "Count Matching condition = {$count}, done = {$done}\n";
            
            $rbsconn->commit();  
            $rbsconn->close();   
            $conn->close();  

            echo "TASSynchronizer.replicateTimeTable() : Finished\n\n";   
            
        } catch (Exception $e) {
            echo $e->getMessage();
        } 	
        return $r;	
    }

    public static function convertToSeconds($t) {
        return (strtotime($t) - strtotime('TODAY'));
    }

    public static function convertToDayOfWeek($t) {
        return date('N', strtotime($t));
    }
  
    public static function getCurrentDateFormatted() {
        date_default_timezone_set('Asia/Hong_Kong');
        $date = new DateTime();
        return "{$date->format('Y-m-d h:i:s')}";
    }

    public static function getRemoteRoomList() {
        $r = false;
        try {
            $rbsconn = new mysqli($server, $username, $password); // rbsUsername,rbsPassword
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
                    $this->$roomToID[$room] = $id;
                    $this->$idToRoom[$id] = $room;
                    $this->$roomToAreaID[$room] = $area_id;
                }
            }
            $rbsconn->close();            
            $r = true;           		
        } catch (Exception $e) {
            echo $e->getMessage();
        }	
        return $r;	
    }
  
    public static function getTASInfo($period) {
        $r = false;
        try {
            $conn = new mysqli($server, $username, $password);
            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
            }
            $sql = "select sid,sname from staff where period='{$period}'";
            $rs = $conn->query($sql);
            if ($rs->num_rows > 0) {
                while ($row = $rs->fetch_assoc()) {
                    $sid = $row["sid"];
                    $sname = $row["sname"];
                    $staff = array($sid,$sname); // Staff
                    $this->$staffHT[$sid] = $staff;
                }
            }
            
            $sql = "select subject_code,subject_title from subject where period='{$period}'";
            $rs = $conn->query($sql);
            if ($rs->num_rows > 0) {
                while ($row = $rs->fetch_assoc()) {
                    $code = $row["subject_code"];
                    $title = $row["subject_title"];
                    $subject = array($code,$title);
                    $this->$subjectHT[$code] = $subject;
                }
            }	
            
            $sql = "select jobno,subject_code,c_code,a_code from Teaching_Requirement where period='{$period}'";
            $rs = $conn->query($sql);
            if ($rs->num_rows > 0) {
                while ($row = $rs->fetch_assoc()) {
                    $jobno = $row["jobno"];
                    $s_code = $row["subject_code"];
                    $c_code = $row["c_code"];
                    $a_code = $row["a_code"];
                    $teachingRequirement = array($jobno,$s_code,$c_code,$a_code);
                    $teachingRequirement["staffHT"] = $this->getTeachingRequirementStaff($jobno,$period,$conn);
                    $this->$teachingRequirementHT[$jobno] = $teachingRequirement;
                }
            }
            $conn->close();            
            $r = true;          		
        } catch (Exception $e) {
            echo $e->getMessage();
        }	
        return $r;	
    }
  
    public static function getTeachingRequirementStaff($jobno, $period, $conn) {
        $sHT = array();
        try {
            $sql = "select sid from assignment_timetable where jobno='{$jobno}' and period='{$period}'";
            $rs = $conn->query($sql);
            if ($rs->num_rows > 0) {
                while ($row = $rs->fetch_assoc()) {
                    $sid = $row["sid"];
                    $sHT[$sid] = $this->$staffHT[$sid];
                }
            }
        } catch (Exception $e) {
            echo $e->getMessage();
        }
        return $sHT;
    }
  
    /**
     * @throws Exception if operation fail
     */
    public static function callInsertBookingURL($ht) {
        // $queryString = getQueryString($ht);
        $httpost = $configs['rbsLoginURL'];
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
        $context  = stream_context_create($options);
        $result = file_get_contents($httpost, false, $context);
        if ($result === false) { 
            throw new Exception("Error occur");
        }
        
        echo "Login form get: \n";
        var_dump($result);

        $httpost2 = $configs['rbsURL'];
        $nvps2 = getNameValuePair($ht);

        $options2 = array(
            'http' => array(
                'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
                'method'  => 'POST',
                'content' => http_build_query($nvps2)
            )
        );
        $context2  = stream_context_create($options2);
        $result2 = file_get_contents($httpost2, false, $context);
        if ($result2 === false) { 
            throw new Exception("Error occur");
        }
        
        echo "Send Data form get: \n";
        var_dump($result);
    }  
	
	public static function getNameValuePair($ht) {
        $nvps = array();
        foreach($this->$fieldList as $field)
            $nvps[$field] = $ht[$field];
		return $nvps;
	}
	
	public static function displayResponseContent($entity) { // [HttpEntity entity]
		echo "Writing Content of Response" . html_entity_decode($entity);
	}
	
	public static function displayErrorMessage($entity) { // [HttpEntity entity]
        $content = html_entity_decode($entity);
        $display = explode('<!--###Begin Error Message###', $content);
        $display = explode('###End Error Message###-->', $display[1]);
        echo $display[0];
	}
	
	public static function getQueryString($ht) {
        $r = "";
        foreach($this->$fieldList as $field)
            $r = "{$field}={$ht[$field]}&";
		return $r;
	}
	
	public static function getTestHT() {
		return array(
            "name" => "COMP001", 
            "description" => "description", 
            "start_day" => $configs['start_day'], 
            "start_month" => $configs['start_month'], 
            "start_year" => $configs['start_year'], 
            "start_seconds" => "28800", 
            "end_day" => $configs['start_day'], 
            "end_month" => $configs['start_month'], 
            "end_year" => $configs['start_year'],
            "end_seconds" => "32400",  
            "area" => "1", 
            "rooms[]" => "1", 
            "type" => "I", 
            "confirmed" => "1", 
            "private" => "0", 
            "f_tas_import" => "1", 
            "f_tas_period" => "2011-2012", 
            "f_tas_sem" => "2", 
            "f_tas_user_comp_acc" => "cstester", 
            "rep_type" => "2", 
            "rep_end_day" => $configs['end_day'], 
            "rep_end_month" => $configs['end_month'], 
            "rep_end_year" => $configs['end_year'], 
            "rep_day[]" => "3", 
            "rep_num_weeks" => "", 
            "returl" => "", 
            "create_by" => "tester", 
            "rep_id" => "0", 
            "edit_type" => "series", 

            "tas_syndate" => "", 
            "tas_sid" => "tassid", 		
            "f_tas_subject_code" => "COMP001", 				
        );
	}
	
    public static function testRemoteConn() {
        $r = "";
        try {
            $rbsconn = new mysqli($server, $username, $password); // rbsUsername,rbsPassword
            if ($rbsconn->connect_error) {
                throw new Exception("Connection failed: " . $rbsconn->connect_error);
            }
            $rbsconn->close();
            $r="TASSynchronizer.testRemoteConn : Finished";             		
        } catch (Exception $e) {
            echo "TASSynchronizer.testRemoteConn : Error while creating ORA-Conn Object";
            $r = $e->getMessage();
            echo $r;	
        }	
        return $r;
    }  
  
    public static function testLocalConn() {
        $r = "";
        try {
            echo "TASSynchronizer.testLocalConn : Using DB Driver {$configs['db_driver']}";
            echo "TASSynchronizer.testLocalConn : Connecting to {$configs['db_connection_string']} by {$configs['username']} with password length " . strlen($configs['password']);
            $conn = new mysqli($server, $username, $password); // rbsUsername,rbsPassword
            if ($conn->connect_error) {
                throw new Exception("Connection failed: " . $conn->connect_error);
            } 
            $conn->close();         
            $r="TASSynchronizer.testLocalConn : Finished";             		
        } catch (Exception $e) {
            echo "TASSynchronizer.testLocalConn : Error while creating ORA-Conn Object";
            $r = $e->getMessage();
            echo $r;
        }	
        return $r;
    }
}

/**
* @throws Exception if operation fail
*/
function start() {
    $r="";
    $syncObject = new TASSynchronizer();
    echo "Replicating TAS Timetable\n";	
    $syncObject->replicateTimeTable($configs['period'], $configs['sem'], NULL, NULL);
}

start();

?>