<?php
//phpinfo();die();
//print_r($_POST);die();
session_start();

require 'db.php';

/* response in json format */
//header('Content-type: application/json');
//header('Content-type: text/html');
?>

<?php
/* Get current server time */
$timestamp = time();
$datum = date("Y:m:d H:i:s",$timestamp);
/* GET => Read DB action */
if ( $_SERVER['REQUEST_METHOD'] == 'GET' ) {

    $mysqli = db_open();
    $result = $mysqli->query('SELECT * FROM beer');
    echo json_encode(($result->fetch_all(MYSQLI_ASSOC)));
   
    /* alternatief: per regel fetch associative array */
    // while ($row = $result->fetch_assoc()) {
    //     echo json_encode($row);
    // }

    /* send response in json format */
    header('Content-type: application/json');
    http_response_code(200); // status ok
}

/* POST => Create DB action */
if ($_SERVER['REQUEST_METHOD'] == 'POST' ) {

    try{
        /* required parameter checking */
        $temperature      = $_POST['temperature']     ;
        $humidity    = $_POST['humidity']   ;
        $pressure = $_POST['pressure'];
     	$time = $_POST['time'];
   		$errors = $_POST['errors'];

        if (!$temperature || !$humidity || !$pressure || !$time || !$errors){ 
            throw new Exception('parameters temperature, humidity and/or pressure and/or dateTime not provided'); 
        }

        /* database query */
        $mysqli = db_open();
        $query = sprintf(
            'INSERT INTO tblsensordata (intTemp, intHum, intPress, tTime, intStationErrors, dtDateTime) VALUES ("%s", "%s", "%s", "%s", "%s", "%s")',
            $mysqli->real_escape_string($temperature)     ,
            $mysqli->real_escape_string($humidity)   , 
            $mysqli->real_escape_string($pressure)   , 
        	$mysqli->real_escape_string($time)   ,
        	$mysqli->real_escape_string($errors)   ,
        	$mysqli->real_escape_string($datum)
        );
        $result = $mysqli->query($query);

        /* processing result(s) */
        if ($result){
            http_response_code(200); // status ok
            echo "ok";
        }
        else{
            http_response_code(400); // bad request
            echo "nok";
        }        
    }
    catch (Exception $e){
        http_response_code(400); /* bad request */
        echo 'nok... ';
        echo $e->getMessage();
    }
}



?>