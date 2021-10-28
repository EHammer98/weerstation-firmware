<?php
$timestamp = time();
$datum = date("Y:m:d H:i:s",$timestamp);
//echo "$datum\n";

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