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

#pragma once
#include <Arduino.h>

// Interface to be implemented
class LDS {
  public:
    enum lds_pin_t {
      LDS_MOTOR_EN_PIN,
      LDS_MOTOR_PWM_PIN,
    };

    enum result_t {
      RESULT_OK = 0,
      ERROR_TIMEOUT,
      ERROR_INVALID_PACKET,
      ERROR_CRC,
      ERROR_NOT_READY,
      ERROR_NOT_IMPLEMENTED,
      ERROR_NOT_CONFIGURED,
      ERROR_MOTOR_DISABLED,
      ERROR_INVALID_MODEL,
      ERROR_DEVICE_INFO,
      ERROR_DEVICE_HEALTH,
      ERROR_START_SCAN,
      ERROR_INVALID_VALUE,
      ERROR_UNKNOWN,
    };

    enum info_t {
      INFO_MODEL,
      INFO_FIRMWARE_VERSION,
      INFO_HARDWARE_VERSION,
      INFO_SERIAL_NUMBER,
      INFO_DEVICE_HEALTH,
      INFO_SAMPLING_RATE,
      INFO_DEFAULT_TARGET_SCAN_FREQ_HZ,
      INFO_OTHER,
    };

    enum packet_pos_t {
      UNKNOWN_POS = 0,
      START_POS = 1,
      END_POS = 2,
      MID_POS = 3,
    };

    typedef void (*ScanPointCallback)(float, float);
    typedef void (*PacketCallback)(uint8_t*, uint16_t, packet_pos_t);
    typedef void (*MotorPinCallback)(float, lds_pin_t);
    typedef size_t (*SerialWriteCallback)(const uint8_t *, size_t);
    typedef int (*SerialReadCallback)(void);
    typedef void (*InfoCallback)(info_t, String);
    typedef void (*ErrorCallback)(result_t, String);

    static const int8_t DEFAULT_VALUE = -1;
    enum lds_pin_state_t {
      VALUE_PWM = 0,
      VALUE_LOW = -1,
      VALUE_HIGH = -2,
      DIR_INPUT = -3,
      DIR_OUTPUT_CONST = -4,
      DIR_OUTPUT_PWM = -5,
    };
  public:
    LDS();
    result_t start(); // Initialize and start; call from Arduino setup()
    void stop(); // Stop the motor and scanning
    void loop(); // Call frequently from Arduino loop()

    uint32_t getSerialBaudRate();
    float getCurrentScanFreq();
    float getTargetScanFreqHz();
    int getSamplingRateHz();
    bool isActive();

    result_t setScanTargetFreqHz(float freq);
    result_t setScanPIDCoeffs(float Kp, float Ki, float Kd);
    result_t setScanPIDSamplePeriodMs(uint32_t sample_period_ms);

    void setScanPointCallback(ScanPointCallback scan_callback);
    void setMotorPinCallback(MotorPinCallback motor_pin_callback);
    void setPacketCallback(PacketCallback packet_callback);
    void setSerialReadCallback(SerialReadCallback serial_read_callback);
    void setSerialWriteCallback(SerialWriteCallback serial_write_callback);
    void setInfoCallback(InfoCallback info_callback);
    void setErrorCallback(ErrorCallback error_callback);

    String resultCodeToString(result_t code);
    String infoCodeToString(info_t code);
    
  protected:
    void postScanPoint(float angle_deg, float dist_mm);
    void setMotorPin(float value, lds_pin_t pin);
    void postPacket(uint8_t* data, uint16_t length, packet_pos_t packet_position);
    int readSerial();
    size_t writeSerial(const uint8_t * buffer, size_t length);
    void postInfo(info_t code, String info);
    void postError(result_t code, String aux_info);

    void enableMotor(bool enable);

  protected:
    ScanPointCallback scan_point_callback;
    PacketCallback packet_callback;
    SerialWriteCallback serial_write_callback;
    SerialReadCallback serial_read_callback;
    MotorPinCallback motor_pin_callback;
    InfoCallback info_callback;
    ErrorCallback error_callback;
};
