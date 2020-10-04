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
$sql = "SELECT * FROM rates";
$result = $conn->query($sql);
 
if ($result->num_rows > 0) {
	$row = $result->fetch_assoc();
	echo $row["DL_rates"]. "\t" . $row["UL_rates"];
    
} else {
    echo "0 结果";
}
$conn->close();
?>
