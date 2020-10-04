<?php
$servername = "39.100.88.210";
$username = "siov";
$password = "siov";
$dbname="siov";

// 创建连接
$conn = new mysqli($servername, $username, $password,$dbname);

// 检测连接
if ($conn->connect_error) {
    die("连接失败: " . $conn->connect_error);
}
$sql = "SELECT * FROM deviant_socket";
$result = $conn->query($sql);
 
    // 输出数据
    while($row = $result->fetch_assoc()) {
    //echo $row["protocol"]. "\t" . $row["socket"]. "\t" . $row["process_id"]. "\t" . $row["process_name"]. "\n";
    echo "<tr style='background-color: #f2dede; border:1px solid #708090;'>
    <td style = 'text-align:center'>" . $row['protocol'] . 
    "</td><td style = 'text-align:center'>" . $row['socket'] . 
    "</td><td style = 'text-align:center'>" . $row['process_id'] . 
    "</td><td style = 'text-align:center'>" . $row['process_name'] . "</td></tr>";
    }

$sql = "SELECT * FROM socket";
$result = $conn->query($sql);
 
    // 输出数据
    while($row = $result->fetch_assoc()) {
    //echo $row["protocol"]. "\t" . $row["socket"]. "\t" . $row["process_id"]. "\t" . $row["process_name"]. "\n";
    echo "<tr style='background-color: #d9edf7; border:1px solid #708090;'>
    <td style = 'text-align:center'>" . $row['protocol'] . 
    "</td><td style = 'text-align:center'>" . $row['socket'] . 
    "</td><td style = 'text-align:center'>" . $row['process_id'] . 
    "</td><td style = 'text-align:center'>" . $row['process_name'] . "</td></tr>";
    }
$conn->close();
?>