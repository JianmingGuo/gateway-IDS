<?php
$q = intval($_GET['q']);

$con = mysqli_connect('39.100.88.210','siov','siov','siov');
if (!$con) {
    die('Could not connect: ' . mysqli_error($con));
}

$sql="SELECT * FROM mem";
$result = mysqli_query($con,$sql);

$mem="";

while($row = mysqli_fetch_array($result)) {
    $mem .= $row['rate'];
    $mem .= ',';
}
echo $mem;
mysqli_close($con);
?>
