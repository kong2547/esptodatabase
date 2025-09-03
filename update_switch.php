<?php
header('Content-Type: application/json; charset=utf-8');

$mysqli = new mysqli("localhost", "root", "hobby2547", "esp_control");
if ($mysqli->connect_errno) {
    http_response_code(500);
    echo json_encode(['ok'=>false,'error'=>'DB connect failed']);
    exit;
}

$data = json_decode(file_get_contents('php://input'), true);

if (!$data || !isset($data['device_id']) || !isset($data['pin']) || !isset($data['state'])) {
    http_response_code(400);
    echo json_encode(['ok'=>false,'error'=>'Bad JSON']);
    exit;
}

$device_id = substr($data['device_id'], 0, 64);
$pin       = intval($data['pin']);
$state     = intval($data['state']) ? 1 : 0;

$stmt = $mysqli->prepare("
    INSERT INTO esp_last_state (device_id, pin, value, updated_at)
    VALUES (?, ?, ?, NOW())
    ON DUPLICATE KEY UPDATE value=VALUES(value), updated_at=NOW()
");
$stmt->bind_param("sii", $device_id, $pin, $state);

if ($stmt->execute()) {
    echo json_encode(['ok'=>true]);
} else {
    http_response_code(500);
    echo json_encode(['ok'=>false,'error'=>'DB error']);
}

$stmt->close();
$mysqli->close();
