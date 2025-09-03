<?php
$mysqli = new mysqli("localhost", "root", "hobby2547", "esp_control");
if ($_SERVER['REQUEST_METHOD']==='POST') {
    $name = $mysqli->real_escape_string($_POST['name']);
    $gpio = intval($_POST['gpio_pin']);
    $device = $mysqli->real_escape_string($_POST['device_id']);
    $mysqli->query("INSERT INTO switches (device_id,name,gpio_pin,state) VALUES('$device','$name',$gpio,0)");
    header("Location: index.php"); exit;
}
?>
<!DOCTYPE html><html><head><meta charset="utf-8"><title>New Switch</title></head><body>
<h3>เพิ่มสวิตช์</h3>
<form method="post">
  ชื่อ: <input name="name"><br>
  GPIO: <input name="gpio_pin"><br>
  Device ID: <input name="device_id" value="ESP32_001"><br>
  <button type="submit">บันทึก</button>
</form>
</body></html>
