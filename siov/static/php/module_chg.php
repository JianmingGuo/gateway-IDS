<?php
$q = intval($_GET['q']);

$con = mysqli_connect('39.100.88.210','siov','siov','siov');
if (!$con) {
    die('Could not connect: ' . mysqli_error($con));
}

$sql="SELECT * FROM changed_module";
$result = mysqli_query($con,$sql);


while($row = mysqli_fetch_array($result)) {
    if ($row['identity'] == "abnormal")
        echo "<tr style='background-color: #f2dede; border:1px solid #708090;'><td style = 'text-align:center'>" . $row['status'] . "</td><td style = 'text-align:center'>" . $row['module'] . "</td><td style = 'text-align:center'>" . $row['time'] . "</td></tr>";
    if ($row['identity'] == "normal")
        echo "<tr style='background-color: #d9edf7; border:1px solid #708090;'><td style = 'text-align:center'>" . $row['status'] . "</td><td style = 'text-align:center'>" . $row['module'] . "</td><td style = 'text-align:center'>" . $row['time'] . "</td></tr>";
}
mysqli_close($con);
?>
