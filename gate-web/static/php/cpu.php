<?php
$q = intval($_GET['q']);

$con = mysqli_connect('39.100.88.210','siov','siov','siov');
if (!$con) {
    die('Could not connect: ' . mysqli_error($con));
}

$sql="SELECT * FROM cpu";
$result = mysqli_query($con,$sql);

$cpu="";

while($row = mysqli_fetch_array($result)) {
    $cpu .= $row['rate'];
    $cpu .= ',';
}
echo $cpu;
mysqli_close($con);
?>
