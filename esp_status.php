<?php
// esp_status.php
// Accept JSON POST from ESP32 and write to MySQL

header('Content-Type: application/json; charset=utf-8');

$DB_HOST = 'localhost';
$DB_USER = 'root';
$DB_PASS = 'hobby2547';        // ใส่รหัสจริงของคุณ
$DB_NAME = 'esp_control';

$mysqli = new mysqli($DB_HOST, $DB_USER, $DB_PASS, $DB_NAME);
if ($mysqli->connect_errno) {
    http_response_code(500);
    echo json_encode(['ok'=>false, 'error'=>'DB connect failed']);
    exit;
}

$raw = file_get_contents('php://input');
$data = json_decode($raw, true);
if (!$data || !isset($data['device_id']) || !isset($data['states'])) {
    http_response_code(400);
    echo json_encode(['ok'=>false, 'error'=>'Bad JSON']);
    exit;
}

$device_id = substr($data['device_id'], 0, 64);
$states = $data['states'];

$mysqli->begin_transaction();
try {
    // Insert events
    $stmt = $mysqli->prepare("INSERT INTO esp_events (device_id, pin, value) VALUES (?, ?, ?)");
    foreach ($states as $s) {
        $pin = intval($s['pin']);
        $val = intval($s['value']) ? 1 : 0;
        $stmt->bind_param('sii', $device_id, $pin, $val);
        $stmt->execute();

        // Upsert last state
        $stmt2 = $mysqli->prepare("
            INSERT INTO esp_last_state (device_id, pin, value, updated_at)
            VALUES (?, ?, ?, NOW())
            ON DUPLICATE KEY UPDATE value=VALUES(value), updated_at=NOW()
        ");
        $stmt2->bind_param('sii', $device_id, $pin, $val);
        $stmt2->execute();
        $stmt2->close();
    }
    $stmt->close();

    $mysqli->commit();
    echo json_encode(['ok'=>true]);
} catch (Exception $e) {
    $mysqli->rollback();
    http_response_code(500);
    echo json_encode(['ok'=>false, 'error'=>'DB error']);
}

