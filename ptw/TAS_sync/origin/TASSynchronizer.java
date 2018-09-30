import java.util.*;
import java.text.*;
import java.sql.*;
import java.io.*;
import oracle.jdbc.pool.*;
import oracle.jdbc.driver.*;
import oracle.sql.DATE; 
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.cookie.Cookie;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;

//import org.apache.http.client.HttpClient;
 

public class TASSynchronizer {

  static String username = "";
  static String password = "";	

  static String db_driver = "";
  static String db_server = "";
  static String db_port = "";
  static String db_sid = "";
  
  static String db_connection_string = "";  
  
  static String rbsDriver = "";
  static String rbsConnectionString = "";
  static String rbsURL = "";
  static String rbsLoginURL = "";
  
  static Hashtable roomToID = new Hashtable();
  static Hashtable idToRoom = new Hashtable();
  static Hashtable fields = new Hashtable();
  static String[] fieldList = {"name","description","start_day","start_month","start_year","start_seconds","end_day","end_month","end_year","end_seconds","area","rooms[]","type","confirmed","private","f_tas_import","f_tas_period","f_tas_sem","f_tas_user_comp_acc","f_tas_subject_code","rep_type","rep_end_day","rep_end_month","rep_end_year","rep_day[]","rep_num_weeks","returl","create_by","rep_id","edit_type","f_tas_syndate"}; 
  static Hashtable roomToAreaID = new Hashtable();

  static Hashtable subjectHT = new Hashtable();
  static Hashtable staffHT = new Hashtable();
  static Hashtable teachingRequirementHT = new Hashtable();
  
	static String period = "";
	static String sem = "";
	static String start_day= "";
	static String start_month= "";
	static String start_year= "";
	static String end_day="";
	static String end_month="";
	static String end_year="";


  public static boolean replicateTimeTable(String period, String sem, String room, String subject) throws Exception {
  	init();
  	String rst = "";
		String condition=""; 
		String delCondition="";
		
		System.out.print("TASSynchronizer.replicateTimeTable(): Collecting Room Information, sucessful = ");
		boolean r = getRemoteRoomList();
		System.out.println(r);
		System.out.println();
		
		System.out.print("TASSynchronizer.replicateTimeTable(): Collecting TAS Basic Information, sucessful = ");
		r = getTASInfo(period);
		System.out.println(r);
		System.out.println();
		
			if (condition==null) condition="";
			//if (condition.equals("")) condition="a.Period='"+period+"'"; 
		
		condition += "a.Period='"+period+"'  and a.STerm<="+sem+" and " + sem+"<=a.ETerm"; 
		delCondition += "tas_import=1 and tas_period='"+period+"' and tas_sem='"+sem+"'"; 
		
		if (room!=null) {
			condition += " and venue='"+room+"'"; 
			delCondition += " and room_id="+roomToID.get("room");
		}
		if (subject!=null) {
			condition += " and a.subject_code='"+subject+"'"; 
			delCondition += " and tas_subject_code='"+subject+"'";
		}

			try {	

			//String sql = "select " + fieldListAssignmentTimeTable + " from "+assigmentTimeTableTAS+" where " + condition;
			
			System.out.println("Replicating Assignment TimeTable having condition "+condition);
			String sql="";

			sql = "select JobNo,subject_code,shour,ehour,wday,venue from assignment_timetable a where " + condition ;
			sql += " group by JobNo,subject_code,shour,ehour,wday,venue"; 
			sql +=" order by a.subject_code "; 
			//System.out.println("sql=\n"+sql);
			System.out.println();
			
			System.out.println("TASSynchronizer.replicateTimeTable() : Connecting to DB "+db_server+" by " + username);
			System.out.println();
					
			Class.forName(db_driver);	
			//System.out.println("TASSynchronizer.replicateTimeTable(String,String,String,String) : Connecting to "+db_connection_string+" by "+username+" with password length "+password.length());

					Connection conn = DriverManager.getConnection(db_connection_string,username,password);
					PreparedStatement pstmtLocal = conn.prepareStatement(sql);
					ResultSet rs = pstmtLocal.executeQuery();
				
					Class.forName(rbsDriver).newInstance();
					Connection rbsconn = DriverManager.getConnection(rbsConnectionString); //,rbsUsername,rbsPassword);        
					rbsconn.setAutoCommit(false);

			System.out.print("Removing record from RBS before replicatiion for condition " + delCondition + " .... ");
			sql = "delete from mrbs_entry where " + delCondition;
			PreparedStatement ps = rbsconn.prepareStatement(sql);
			ps.execute();
			//sql = "delete from mrbs_repeat where tas_period='"+period+"' and tas_sem='"+sem+"'";
			sql = "delete from mrbs_repeat where " + delCondition;
			ps = rbsconn.prepareStatement(sql);
			ps.execute();
			ps.close();
			System.out.println("done");
			System.out.println();
			
					//PreparedStatement pstmt = rbsconn.prepareStatement(sql);
			//ResultSet rs = pstmt.executeQuery();
			Vector v = new Vector();
				
			int count=0;
			int done=0;
					int SID; 
				if (rs!=null) {    	                
				while (rs.next()) {
					try {
						Hashtable ht = new Hashtable();
						count++;
						//SID	= rsLocal.getInt("SID");
						//String tassid = rs.getString("SID");
						String jobno = rs.getString("jobno");
						String subjectCode = rs.getString("subject_code");
						String shour = rs.getString("shour");
						String start_seconds = new String(convertToSeconds(shour)+"");
						String ehour = rs.getString("ehour");
						String end_seconds = new String(convertToSeconds(ehour)+""); 
						String wday=rs.getString("wday");
						String rep_day=convertToDayOfWeek(wday)+"";
						String venue = rs.getString("venue");
						//String subjectTitle=rs.getString("subject_title");
						//String sname=rs.getString("sname");
						
						System.out.println("TASSynchronizer.replicateTimeTable(): Processing "+subjectCode+ " on " +wday + " " + shour + "-" + ehour + " at " + venue);
						
						String subjectTitle="";
						try {
							subjectTitle = ((Subject)subjectHT.get(subjectCode)).subject_title;
						} catch (Exception e) {
							System.out.println("*** ERROR: TASSynchronizer.replicateTimeTable(): subject title of " + subjectCode + " not available");
						}
						
						String sname = "";
						String description = "";
						try {
							sname = ((TeachingRequirement)teachingRequirementHT.get(jobno)).getStaffNameList();
							description = subjectTitle+" (" + sname + ")";
							System.out.println("TASSynchronizer.replicateTimeTable():  by " + sname );
						} catch (Exception e) {
							System.out.println("*** ERROR: TASSynchronizer.replicateTimeTable(): Teaching Requirement of " + jobno + " subject code " + subjectCode + "  not available");
						}

						
						if (!rep_day.equals("-1") && roomToAreaID.get(venue)!=null && roomToID.get(venue)!=null) {
							done++;
							ht.put("name",subjectCode);
							ht.put("description",description);
							ht.put("start_day",start_day);
							ht.put("start_month",start_month);
							ht.put("start_year",start_year);
							ht.put("start_seconds",start_seconds);
							ht.put("end_day",start_day);
							ht.put("end_month",start_month);
							ht.put("end_year",start_year);
							ht.put("end_seconds",end_seconds); 
							ht.put("area",(String)roomToAreaID.get(venue));
							ht.put("rooms[]",(String)roomToID.get(venue));
							ht.put("type","I");
							ht.put("confirmed","1");
							ht.put("private","0");
							ht.put("f_tas_import","1");					
							ht.put("f_tas_period",period);
							ht.put("f_tas_sem",sem);
							ht.put("f_tas_user_comp_acc","");
							ht.put("rep_type","2");
							ht.put("rep_end_day",end_day);
							ht.put("rep_end_month",end_month);
							ht.put("rep_end_year",end_year);
							ht.put("rep_day[]",rep_day);
							ht.put("rep_num_weeks","");
							ht.put("returl","");
							ht.put("create_by","cspaulin");
							ht.put("rep_id","0");
							ht.put("edit_type","series");
							ht.put("f_tas_subject_code",subjectCode);					
							
							/*
							ht.put("tas_import","1");
							ht.put("tas_period",period);
							ht.put("tas_sem",sem);
							*/
							ht.put("f_tas_syndate",getCurrentDateFormatted());
							//ht.put("tas_sid",tassid);
							
							v.add(ht);
							//insertToRemoteDB(rbsconn,ht);
							callInsertBookingURL(ht);
						} else {
							System.out.println("Not replicating " + subjectCode+" by " + sname +" " + wday + " " + shour +"-" + ehour + " at " + venue);
						}
						System.out.println();
					} catch (Exception e1) {
						System.out.println("*** ERROR: TASSynchronizer.replicateTimeTable(): " + e1.getMessage());
						e1.printStackTrace();
					}
				} 
					}
			System.out.println("Count Matching condition = "+count+", done = " + done + "\n");
					
					rbsconn.commit();  
					rbsconn.close();   
															
					rs.close();
			conn.close();  

			rst="TASSynchronizer.replicateTimeTable() : Finished\n";   
			
			System.out.println(rst); 
			
			} catch (Exception e) {
			e.printStackTrace();
				rst=e.getMessage();
			} 	
			return r;	
  }
  
  public static String getCurrentDateFormatted() {
		String r="";
		java.util.Date d = new java.util.Date();
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		r = sdf.format(d);
		return r;
  }

  public static boolean getRemoteRoomList() {
		init();
  	//String r="";
  	boolean r=false;
  	try {
        Class.forName(rbsDriver).newInstance();
        Connection rbsconn = DriverManager.getConnection(rbsConnectionString); //,rbsUsername,rbsPassword);
        String sql = "select id,room_name ,area_id from mrbs_room order by id";
		PreparedStatement ps = rbsconn.prepareStatement(sql);
		ResultSet rs = ps.executeQuery();
		if (rs!=null) {
			while (rs.next()) {
				String id = rs.getString(1);
				String room = rs.getString(2);
				String area_id = rs.getString(3);
				roomToID.put(room,id);
				idToRoom.put(id,room);
				roomToAreaID.put(room,area_id);
			}
		}
		rbsconn.close();            
		r=true;
		//r="TASSynchronizer.testRemoteConn : Finished";             		
  	} catch (Exception e) {
  		e.printStackTrace();
  		//r=e.getMessage();	
  	}	
  	return r;	
  }

  public static boolean getTASInfo(String period) {
		init();
  	boolean r=false;
  	try {
		Class.forName(db_driver);	
        Connection conn = DriverManager.getConnection(db_connection_string,username,password);
		String sql = "select sid,sname from staff where period='"+period+"'";
        PreparedStatement pstmtLocal = conn.prepareStatement(sql);
        ResultSet rs = pstmtLocal.executeQuery();
		if (rs!=null) {
			while (rs.next()) {
				String sid = rs.getString(1);
				String sname = rs.getString(2);
				Staff staff = new Staff (sid,sname);
				staffHT.put(sid,staff);
			}
		}
		rs.close();
		
		sql = "select subject_code,subject_title from subject where period='"+period+"'";
		pstmtLocal = conn.prepareStatement(sql);
        rs = pstmtLocal.executeQuery();
		if (rs!=null) {
			while (rs.next()) {
				String code = rs.getString(1);
				String title = rs.getString(2);
				Subject subject = new Subject(code,title);
				subjectHT.put(code,subject);
			}
		}
		rs.close();		
		
		sql = "select  jobno, subject_code, c_code, a_code from Teaching_Requirement where period='"+period+"'";
		pstmtLocal = conn.prepareStatement(sql);
        rs = pstmtLocal.executeQuery();
		if (rs!=null) {
			while (rs.next()) {
				String jobno = rs.getString(1);
				String subject_code = rs.getString(2);
				String c_code = rs.getString(3);
				String a_code = rs.getString(4);
				TeachingRequirement teachingRequirement = new TeachingRequirement(jobno,subject_code,c_code,a_code);
				teachingRequirement.staffHT = getTeachingRequirementStaff(jobno,period, conn);
				teachingRequirementHT.put(jobno,teachingRequirement);
			}
		}
		rs.close();	
		
		conn.close();            
		r=true;
		//r="TASSynchronizer.testRemoteConn : Finished";             		
  	} catch (Exception e) {
  		e.printStackTrace();
  		//r=e.getMessage();	
  	}	
  	return r;	
  }
  
  public static Hashtable getTeachingRequirementStaff(String jobno,String period, Connection conn) {
		Hashtable sHT = new Hashtable();
		init();
		try {
		//Class.forName(db_driver);	
        //Connection conn = DriverManager.getConnection(db_connection_string,username,password);
		String sql = "select sid from assignment_timetable where jobno='"+jobno+"'" + " and period='"+period+"'";
        PreparedStatement pstmtLocal = conn.prepareStatement(sql);
        ResultSet rs = pstmtLocal.executeQuery();
		if (rs!=null) {
			while (rs.next()) {
				String sid = rs.getString(1);
				sHT.put(sid,(Staff)staffHT.get(sid));
			}
		}
		rs.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return sHT; 
  }
  
  public static int convertToSeconds(String t) {
		int r=0;
		int i = t.indexOf(":");
		int h=Integer.parseInt(t.substring(0,i));
		int m=Integer.parseInt(t.substring(i+1,t.length()));
		r= (h*60+m)*60;
		return r;
  }

  public static int convertToDayOfWeek(String t) {
		int r=-1;
		if (t==null) t="";
		t=t.toUpperCase();
		if (t.equals("SUN")) r=0;
		if (t.equals("MON")) r=1;
		if (t.equals("TUE")) r=2;
		if (t.equals("WED")) r=3;
		if (t.equals("THU")) r=4;
		if (t.equals("FRI")) r=5;
		if (t.equals("SAT")) r=6;	
		return r;
  }

  public static void callInsertBookingURL(Hashtable ht) throws Exception {

		init();

		//Terence 20150108 Ignore SSL Cert Checking
        //DefaultHttpClient httpclient = new DefaultHttpClient();
        DefaultHttpClient httpclient = new DefaultHttpClient();
		httpclient = WebClientDevWrapper.wrapClient(httpclient); 


        try {

			String queryString = getQueryString(ht);
			
            HttpPost httpost = new HttpPost(rbsLoginURL);

            List <NameValuePair> nvps = new ArrayList <NameValuePair>();
            //nvps.add(new BasicNameValuePair("IDToken1", "username"));
            //nvps.add(new BasicNameValuePair("IDToken2", "password"));
            nvps.add(new BasicNameValuePair("NewUserName", "<username>"));
            nvps.add(new BasicNameValuePair("NewUserPassword", "<password>"));
            nvps.add(new BasicNameValuePair("returl", ""));
            nvps.add(new BasicNameValuePair("TargetURL", "admin.php?"));
            nvps.add(new BasicNameValuePair("Action", "SetName"));
            nvps.add(new BasicNameValuePair("submit", " Log in "));

            httpost.setEntity(new UrlEncodedFormEntity(nvps, HTTP.UTF_8));

            HttpResponse response = httpclient.execute(httpost);
            HttpEntity entity = response.getEntity();

            System.out.println("Login form get: " + response.getStatusLine());
			//displayResponseContent(entity);
            EntityUtils.consume(entity);
		
			//System.out.println("Query String = \n" + queryString);
			//HttpPost httpost2 = new HttpPost(rbsURL+"?" + queryString);
			HttpPost httpost2 = new HttpPost(rbsURL);
			
			List <NameValuePair> nvps2 = getNameValuePair(ht);
            httpost2.setEntity(new UrlEncodedFormEntity(nvps2, HTTP.UTF_8));
			
            HttpResponse response2 = httpclient.execute(httpost2);
            HttpEntity entity2 = response2.getEntity();
			//displayResponseContent(entity2);
			displayErrorMessage(entity2);
			EntityUtils.consume(entity2);

            System.out.println("Send Data form get: " + response2.getStatusLine());
			//System.out.println();
			
            //EntityUtils.consume(entity);			

        } finally {
            // When HttpClient instance is no longer needed,
            // shut down the connection manager to ensure
            // immediate deallocation of all system resources
            httpclient.getConnectionManager().shutdown();
        }
  }  
	
	public static List <NameValuePair> getNameValuePair(Hashtable ht) {
		List <NameValuePair> nvps = new ArrayList <NameValuePair>();
		for (int i=0;i<fieldList.length;i++) {
			nvps.add(new BasicNameValuePair(fieldList[i], (String)ht.get(fieldList[i])));
		}
		return nvps;
	}
	
	public static void displayResponseContent(HttpEntity entity) {
		try {
			System.out.println("Writing Content of Response");
			InputStreamReader isr = new InputStreamReader(entity.getContent());
			BufferedReader br = new BufferedReader(isr);
			String line="";
			while ((line=br.readLine())!=null) {
				System.out.println(line);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void displayErrorMessage(HttpEntity entity) {
		try {
			//System.out.println("Writing Error Message");
			InputStreamReader isr = new InputStreamReader(entity.getContent());
			BufferedReader br = new BufferedReader(isr);
			String line="";
			boolean begin=false;
			while ((line=br.readLine())!=null) {
				if (line.equals("###End Error Message###-->")) return; //begin=false;
				if (begin) System.out.println(line);
				if (line.equals("<!--###Begin Error Message###")) begin=true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static String getQueryString(Hashtable ht) {
		String r="";
		for (int i=0;i<fieldList.length;i++) {
			r+=fieldList[i]+"="+(String)ht.get(fieldList[i])+"&";
		}
		return r;
	}
	
	public static Hashtable getTestHT() {
		Hashtable ht = new Hashtable();
					ht.put("name","COMP001");
					ht.put("description","description");
					ht.put("start_day",start_day);
					ht.put("start_month",start_month);
					ht.put("start_year",start_year);
					ht.put("start_seconds","28800");
					ht.put("end_day",start_day);
					ht.put("end_month",start_month);
					ht.put("end_year",start_year);
					ht.put("end_seconds","32400"); 
					ht.put("area","1");
					ht.put("rooms[]","1");
					ht.put("type","I");
					ht.put("confirmed","1");
					ht.put("private","0");
					ht.put("f_tas_import","1");
					ht.put("f_tas_period","2011-2012");
					ht.put("f_tas_sem","2");
					ht.put("f_tas_user_comp_acc","cstester");
					ht.put("rep_type","2");
					ht.put("rep_end_day",end_day);
					ht.put("rep_end_month",end_month);
					ht.put("rep_end_year",end_year);
					ht.put("rep_day[]","3");
					ht.put("rep_num_weeks","");
					ht.put("returl","");
					ht.put("create_by","tester");
					ht.put("rep_id","0");
					ht.put("edit_type","series");

					ht.put("tas_syndate","");
					ht.put("tas_sid","tassid");		
					ht.put("f_tas_subject_code","COMP001");					
		
		return ht;
	}
	
  public static String testRemoteConn() {
  	init();
  	String r="";
  	try {
        Class.forName(rbsDriver).newInstance();
        Connection rbsconn = DriverManager.getConnection(rbsConnectionString);
        rbsconn.close();            
		r="TASSynchronizer.testRemoteConn : Finished";             		
  	} catch (Exception e) {
  		e.printStackTrace();
  		r=e.getMessage();	
  	}	
  	return r;
  }  
  
  public static String testLocalConn() {
  	init();
  	String r="";
  	try {
		System.out.println("TASSynchronizer.testLocalConn : Using DB Driver "+db_driver);
		Class.forName ("oracle.jdbc.OracleDriver");
		System.out.println("TASSynchronizer.testLocalConn : Connecting to "+db_connection_string+" by "+username+" with password length "+password.length());
        Connection conn = DriverManager.getConnection(db_connection_string,username,password);
        conn.close();            
		r="TASSynchronizer.testLocalConn : Finished";             		
  	} catch (Exception e) {
		System.out.println("TASSynchronizer.testLocalConn : Error while creating ORA-Conn Object");
  		e.printStackTrace();
  		r=e.getMessage();	
  	}	
  	return r;
  }  


   
//////////////////////////////////////////////////////////////////////////////  

  public static String init() {
	  
		String iniFileName = "config.properties";                     
		Properties INI = new Properties();
		try { 	     
			INI.load(ClassLoader.getSystemResourceAsStream(iniFileName) );
	  	
	  	  	username = INI.getProperty("TASusername");
		  	password = INI.getProperty("TASpassword");	
		
		  	db_driver = INI.getProperty("TASdb_driver");
		  	db_server = INI.getProperty("TASdb_server");
		  	db_port = INI.getProperty("TASdb_port");
		  	db_sid = INI.getProperty("TASdb_sid");

		  	rbsDriver = INI.getProperty("rbsDriver");
			rbsConnectionString = INI.getProperty("rbsConnectionString");
		  	rbsURL = INI.getProperty("rbsURL");
		  	rbsLoginURL = INI.getProperty("rbsLoginURL");
			
			period = INI.getProperty("period");
			sem = INI.getProperty("sem");
			start_day= INI.getProperty("start_day");
			start_month =INI.getProperty("start_month");
			start_year=INI.getProperty("start_year");
			end_day=INI.getProperty("end_day");
			end_month=INI.getProperty("end_month");
			end_year=INI.getProperty("end_year");

			

		} catch (Exception e) {
			e.printStackTrace();	
		}
	  db_connection_string = "jdbc:oracle:thin:@" + db_server + ":" + db_port + ":" + db_sid;  

	  String r="TASSynchronizer initialized";
	  return r; 
  	
  }
   
  
   public static void main (String args[])  throws Exception {
		init();

		String r;
		r="";


		System.out.println("Replicating TAS Timetable");	
		System.out.println();

		//replicateTimeTable(period,sem,null,"COMP200");
		//replicateTimeTable(period,sem,null,"COMP212A");
		//replicateTimeTable(period,sem,null,"COMP309");

		replicateTimeTable(period,sem,null,null);

		//r = getCurrentDateFormatted();
		//System.out.println("Current Date Formated = "+r);		

		
	}  
  
   
}


