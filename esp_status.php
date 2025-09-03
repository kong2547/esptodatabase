<?php
header('Content-Type: application/json; charset=utf-8');
$mysqli = new mysqli("localhost", "root", "hobby2547", "esp_control");

$data = json_decode(file_get_contents('php://input'), true);
if (!$data) { echo json_encode(['ok'=>false]); exit; }

$device = $mysqli->real_escape_string($data['device_id']);
foreach ($data['states'] as $s) {
    $pin = (int)$s['pin']; $val = (int)$s['value'];
    $mysqli->query("UPDATE switches SET state=$val WHERE device_id='$device' AND gpio_pin=$pin LIMIT 1");
}
echo json_encode(['ok'=>true]);
