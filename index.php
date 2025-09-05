<?php
$mysqli = new mysqli("localhost", "root", "", "esptodb");
if ($mysqli->connect_errno) die("DB Error");

// toggle จากปุ่มเว็บ
if (isset($_GET['toggle'])) {
    $id = intval($_GET['toggle']);
    $mysqli->query("UPDATE switches SET state = 1 - state WHERE id=$id");
    header("Location: index.php"); exit;
}

$result = $mysqli->query("SELECT * FROM switches ORDER BY id");
?>
<!DOCTYPE html><html lang="th"><head>
<meta charset="utf-8"><title>ESP Control</title></head><body>
<h2>ESP32 Control Panel</h2>
<a href="new_switch.php">➕ เพิ่มสวิตช์</a><hr>
<?php while($row=$result->fetch_assoc()): ?>
  <div style="border:1px solid #aaa; padding:10px; margin:5px; display:inline-block;">
    <b><?=htmlspecialchars($row['name'])?></b><br>
    GPIO: <?=$row['gpio_pin']?> | 
    สถานะ: <?=$row['state']?'ON':'OFF'?><br>
    <a href="?toggle=<?=$row['id']?>"><button><?=$row['state']?'ปิด':'เปิด'?></button></a>
  </div>
<?php endwhile; ?>
</body></html>
