<?php
$temp = 0;
$humi = 0;
$metan = 0;
$date = date("d-m-Y");
$time = date("H:i:s");

    if (isset($_POST["temp"])){
        $temp = $_POST['temp'];
    }
    if (isset($_POST["humi"])){
        $humi = $_POST['humi'];
    }
    if (isset($_POST["metan"])){
        $metan = $_POST['metan'];
    }

$hostname = 'localhost';
$username = 'root';
$password = '';
$database = 'compost';

$conn = mysqli_connect($hostname, $username, $password, $database);

$sql = "INSERT INTO data (temp, humi, metan, date, time) 
        VALUES ('$temp','$humi','$metan','$date', '$time')";
$result = mysqli_query($conn, $sql);
if ($result) {
    echo "Gui du lieu thanh cong";
}
mysqli_close($conn);
?>