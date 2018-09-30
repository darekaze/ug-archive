<?php
// To include, use: $configs = include('config.php');
return array(
    'TASusername' => '',
    'TASpassword' => '',

    'TASdb_driver' => 'oracle.jdbc.driver.OracleDriver',
    'TASdb_server' => '',
    'TASdb_port' => '1521',
    'TASdb_sid' => 'xxx',
    
    // RBS Stuff
    'rbsDriver' => 'com.mysql.jdbc.Driver',
    'rbsConnectionString' => 'jdbc:mysql://myxxx.comp.polyu.edu.hk/rbs?user=><username>&password=><password>',
    'rbsURL' => 'https://xxx.comp.polyu.edu.hk/edit_entry_handler.php',
    'rbsLoginURL' => 'https://xxx.comp.polyu.edu.hk/admin.php',

    // TAS Syn Configuration
    'period' => '2018-2019',
    'sem' => '1',
    'start_day'=>'11',
    'start_month'=>'1',
    'start_year'=>'2016',
    'end_day'=>'16',
    'end_month'=>'4',
    'end_year'=>'2016'
);
?>