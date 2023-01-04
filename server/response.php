<?php
date_default_timezone_set('Asia/Ho_Chi_Minh');
// connect database
$hostname = 'localhost';
$username = 'root';
$password = '';
$database = 'compost';

$conn = mysqli_connect($hostname, $username, $password, $database);
if (!$conn){
    echo mysqli_error();
}
// get temp, humidity, methan from database
$sql = "SELECT *
        FROM data
        WHERE id = (
            SELECT MAX(id)
            FROM data
        )";

$result = mysqli_query($conn, $sql);
if (!$result){
    echo mysqli_error();
}

$row = mysqli_fetch_assoc($result);

$temp = $row['temp'];
$humi = $row['humi'];
$methane = $row['metan'];
// compute progess 
$sql = "SELECT * FROM data 
        WHERE id = (
            SELECT MIN(id)
            FROM data
        )";

$result = mysqli_query($conn, $sql);
if (!$result){
    echo mysqli_error();
}
$row = mysqli_fetch_assoc($result);

$started_date = "15-12-2022";//$row['date'];
$current_date = date("d-m-Y");

$now = strtotime($current_date);
$past = strtotime($started_date);
$progress = ($now - $past)/86400 + 1 ;

// close database connection
mysqli_close($conn);

// Handle temperature and moisture
$tempDanger = 80; //độ C
$tempHigh = 70 ;
$tempOK = 60; 
$tempLow = 32;

$moistHigh = 60;
$moistLow = 40;

$tempAlert = 0;
$moistAlert = 0;
$methaneAlert = 0;
$vent = 0; // 0: close, 1: open
$water_pump = 2; // 2:close, 3:open

if ($methane > 50000){
    $methaneAlert = 2;
}   
elseif (50000 >= $methane && $methane > 10000){
    $methaneAlert = 1;
}   


if ($temp > $tempDanger){
    $tempAlert = 3;
    $vent = 1;
    $water_pump = 3;
}
else {
    if ($progress <= 35 && $progress >=26){
        $tempAlert = 2;
    }
    elseif ($progress > 35){
        $tempAlert = 1;
    }
    else {
        if ($tempDanger >= $temp && $temp > $tempHigh) {
            $tempAlert = 4;
            $vent = 1;
            $water_pump = 3;
        }   
        elseif ($tempHigh >= $temp && $temp > $tempOK) {
                $tempAlert = 6;
        }
        elseif ($tempOK >= $temp && $temp > $tempLow){
            $tempAlert = 7;
        } 
        else {
            $tempAlert = 8;
            $vent = 0;
        }
    }
}

if ($humi > 60){
    $moistAlert = 0;
    $vent = 1;
}
elseif ($humi <= 60 && $humi >40){
    $moistAlert = 2;
}
else {
    $moistAlert = 4;
    $water_pump = 3;
}

$response = array("temp" => "$temp", "humi" => "$humi", 
            "metan" => "$methane", "progress" => "$progress", 
            "tempAlert" => "$tempAlert", "moistAlert" => "$moistAlert",
            "action" => "$vent$water_pump", "methaneAlert" => "$methaneAlert");
echo json_encode($response);
?>