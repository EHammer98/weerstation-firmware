<?php

require "settings.txt";


// Create connection
function db_open(){
	
	// external defined global variables
	global $servername;
	global $username;
	global $password;
	global $dbname;
	global $dbport;
	
	$conn = new MySQLi($servername, $username, $password, $dbname, $dbport);
	
	// Check connection
	if ($conn->connect_error) 
	{
		printf("Connection failed: %s\n", $conn->connect_error);
		die();
	} 
	
	if (!$conn->set_charset("utf8")) {
		printf("Error loading character set utf8: %s\n", $conn->error);
		exit();
	}
	

	return $conn;
}




function db_close($conn){
	if ($conn)
	{
		$conn->close();
		$conn = null;
	}
}

?>