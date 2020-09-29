<?php
$q = intval($_GET['q']);

$con = mysqli_connect('39.100.88.210','siov','siov','siov');
if (!$con) {
    die('Could not connect: ' . mysqli_error($con));
}

$sql="SELECT * FROM net";
$result = mysqli_query($con,$sql);

// echo "<tr><th>proto</th><th>local</th><th>remote</th><th>time</th><th>identity</th><tr>";
while($row = mysqli_fetch_array($result)) {
    if ($row['proto'] == 'tcp6')
    {
        $local = substr($row['local'],0,22) . '<br>' . substr($row['local'],22);
        $remote = substr($row['remote'],0,22) . '<br>' . substr($row['remote'],22);
    }
    else
    {
        $local = $row['local'];
        $remote = $row['remote'];
    }
    
    if ($row['identity'] == "abnormal")
        echo "<tr style='background-color: #f2dede; border:1px solid #708090;'><td style = 'text-align:center'>" . $row['proto'] . "</td><td style = 'text-align:center'>" . $local . "</td><td style = 'text-align:center'>" . $remote . "</td><td style = 'text-align:center'>" . $row['time'] . "</td></tr>";
    if ($row['identity'] == "normal")
        echo "<tr style='background-color: #d9edf7; border:1px solid #708090;'><td style = 'text-align:center'>" . $row['proto'] . "</td><td style = 'text-align:center'>" . $local . "</td><td style = 'text-align:center'>" . $remote . "</td><td style = 'text-align:center'>" . $row['time'] . "</td></tr>";
}

mysqli_close($con);
?>
