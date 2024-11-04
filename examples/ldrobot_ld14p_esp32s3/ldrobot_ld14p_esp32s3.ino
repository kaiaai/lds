// Copyright 2023-2024 REMAKE.AI, KAIA.AI, MAKERSPET.COM
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP32
  #error This example runs on ESP32S3
#endif

#include <LDS_LDROBOT_LD14P.h>

// ESP32-S3
HardwareSerial LidarSerial(1);
LDS_LDROBOT_LD14P lidar;

void setup() {
  Serial.begin(115200);

  Serial.print("LiDAR model ");
  Serial.println(lidar.getModelName());

  Serial.print("LiDAR RX buffer size "); // default 128 hw + 256 sw
  Serial.print(LidarSerial.setRxBufferSize(1024)); // must be before .begin()

  uint32_t baud_rate = lidar.getSerialBaudRate();
  Serial.print(", baud rate ");
  Serial.println(baud_rate);

  LidarSerial.begin(baud_rate, SERIAL_8N1, 16, 15); // GPIO16 as RX1, GPIO15 as TX1  

  //while (LidarSerial.read() >= 0);

  lidar.setScanPointCallback(lidar_scan_point_callback);
  lidar.setPacketCallback(lidar_packet_callback);
  lidar.setSerialWriteCallback(lidar_serial_write_callback);
  lidar.setSerialReadCallback(lidar_serial_read_callback);
  //lidar.setMotorPinCallback(lidar_motor_pin_callback);
  lidar.init();

  LDS::result_t result = lidar.start();
  Serial.print("LiDAR start() result: ");
  Serial.println(lidar.resultCodeToString(result));

  if (result < 0)
    Serial.println("Is the LiDAR connected to ESP32?");
}

int lidar_serial_read_callback() {
  return LidarSerial.read();
/*
  int ch = LidarSerial.read();
  if (ch != -1)
    Serial.println(ch);
  return ch;
*/
}

size_t lidar_serial_write_callback(const uint8_t * buffer, size_t length) {
  return LidarSerial.write(buffer, length);
}

void lidar_scan_point_callback(float angle_deg, float distance_mm, float quality,
  bool scan_completed) {
  static int i=0;

  if ((i++ % 20 == 0) || scan_completed) {
    Serial.print(i);
    Serial.print(' ');
    Serial.print(distance_mm);
    Serial.print(' ');
    Serial.print(angle_deg);
    if (scan_completed)
      Serial.println('*');
    else
      Serial.println();
  }
}

void lidar_info_callback(LDS::info_t code, String info) {
  Serial.print("LiDAR info ");
  Serial.print(lidar.infoCodeToString(code));
  Serial.print(": ");
  Serial.println(info);
}

void lidar_error_callback(LDS::result_t code, String aux_info) {
  Serial.print("LiDAR error ");
  Serial.print(lidar.resultCodeToString(code));
  Serial.print(": ");
  Serial.println(aux_info);
}

//void lidar_motor_pin_callback(float value, LDS::lds_pin_t lidar_pin) {
  // This LiDAR has only TX, RX pins
//}

void lidar_packet_callback(uint8_t * packet, uint16_t length, bool scan_completed) {
  return;
}

void loop() {
  lidar.loop();
}
