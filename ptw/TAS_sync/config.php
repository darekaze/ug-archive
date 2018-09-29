<?php
// To include, use: $configs = include('config.php');

return array(
    'TASusername' => xxx,
    'TASpassword' => xxx,

    'TASdb_driver' => 'oracle.jdbc.driver.OracleDriver',
    'TASdb_server' => 'xxx.comp.polyu.edu.hk',
    'TASdb_port' => '1521',
    'TASdb_sid' => 'xxx',
    
    // RBS Synchronizer
    'rbsDriver' => 'com.mysql.jdbc.Driver',

    // Production
    'rbsConnectionString' => 'jdbc:mysql://myxxx.comp.polyu.edu.hk/rbs?user=><username>&password=><password>',
    'rbsURL' => 'https://xxx.comp.polyu.edu.hk/edit_entry_handler.php',
    'rbsLoginURL' => 'https://xxx.comp.polyu.edu.hk/admin.php',
    'GIBURL' => 'https://xxx.comp.polyu.edu.hk/getindbooking.php',

    // TAS Syn Configuration
    'period' => '2018-2019',
    'sem' => '1',
    'start_day'=>'11',
    'start_month'=>'1',
    'start_year'=>'2016',
    'end_day'=>'16',
    'end_month'=>'4',
    'end_year'=>'2016',

    'emailFrom' => 'labbook@comp.polyu.edu.hk',
    'emailTo' => 'csterence@comp.polyu.edu.hk,csevan@comp.polyu.edu.hk,helpdesk@comp.polyu.edu.hk',
    'smtphost' => 'mailhost.comp.polyu.edu.hk',

    // CLS DB Parameter
    'cls.driver'   => 'oracle.jdbc.driver.OracleDriver',
    'cls.url'      => 'jdbc:oracle:thin:@xxx.comp.polyu.edu.hk:1521:xxx',
    'cls.account'  => 'compxxxx',
    'cls.password' => 'xxxxxxxx'
)

?>