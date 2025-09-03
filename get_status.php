<?php
header('Content-Type: application/json; charset=utf-8');
$mysqli = new mysqli("localhost", "root", "hobby2547", "esp_control");

$device_id = $_GET['device_id'] ?? '';
if ($device_id==='') { echo json_encode(['ok'=>false]); exit; }

$res = $mysqli->query("SELECT gpio_pin,state FROM switches WHERE device_id='".$mysqli->real_escape_string($device_id)."'");
$states=[]; while($r=$res->fetch_assoc()) $states[]=['pin'=>(int)$r['gpio_pin'],'state'=>(int)$r['state']];
echo json_encode(['ok'=>true,'device_id'=>$device_id,'states'=>$states]);
