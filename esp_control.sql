-- สร้างฐานข้อมูล
CREATE DATABASE IF NOT EXISTS esp_control
  CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE esp_control;

-- เก็บ log ทุกครั้งที่มีการส่งสถานะ
CREATE TABLE IF NOT EXISTS esp_events (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  device_id  VARCHAR(64) NOT NULL,
  pin        INT NOT NULL,
  value      TINYINT(1) NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- เก็บสถานะล่าสุดของแต่ละ pin
CREATE TABLE IF NOT EXISTS esp_last_state (
  device_id  VARCHAR(64) NOT NULL,
  pin        INT NOT NULL,
  value      TINYINT(1) NOT NULL,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (device_id, pin)
) ENGINE=InnoDB;

-- เก็บ log แบบง่าย ๆ (ตามโครงสร้าง gpio_log เดิม)
CREATE TABLE IF NOT EXISTS gpio_log (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  pin        INT NOT NULL,
  status     TINYINT(1) NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;
