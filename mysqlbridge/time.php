<?php
$tz = 'Europe/Amsterdam';
$timestamp = time();
$dt = new DateTime("now", new DateTimeZone($tz)); //first argument "must" be a string
$dt->setTimestamp($timestamp); //adjust the object to correct timestamp
$datum = $dt->format('d-m-Y H:i:s');

if ( $_SERVER['REQUEST_METHOD'] == 'GET' ) {
    //echo json_encode(($datum));
    echo $datum;
    /* alternatief: per regel fetch associative array */
    // while ($row = $result->fetch_assoc()) {
    //     echo json_encode($row);
    // }

    /* send response in json format */
    //header('Content-type: application/json');
	header('Content-type: text/plain');
    http_response_code(200); // status ok
}
?>
