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
$sql = "SELECT * FROM deviant_process";
$result = $conn->query($sql);
 
    // 输出数据
    while($row = $result->fetch_assoc()) {
    //echo $row["name"]. "\t" . $row["pid"]. "\t" . $row["uid"]. "\t" . $row["path"]. "\n";
    echo "<tr style='background-color: #f2dede; border:1px solid #708090;'>
    <td style = 'text-align:center'>" . $row['name'] . 
    "</td><td style = 'text-align:center'>" . $row['pid'] . 
    "</td><td style = 'text-align:center'>" . $row['uid'] . 
    "</td><td style = 'text-align:center'>" . $row['path'] . "</td></tr>";
    }

$sql = "SELECT * FROM process";
$result = $conn->query($sql);
 
    // 输出数据
    while($row = $result->fetch_assoc()) {
    //echo $row["name"]. "\t" . $row["pid"]. "\t" . $row["uid"]. "\t" . $row["path"]. "\n";
    echo "<tr style='background-color: #d9edf7; border:1px solid #708090;'>
    <td style = 'text-align:center'>" . $row['name'] . 
    "</td><td style = 'text-align:center'>" . $row['pid'] . 
    "</td><td style = 'text-align:center'>" . $row['uid'] . 
    "</td><td style = 'text-align:center'>" . $row['path'] . "</td></tr>";
    }
$conn->close();
?>