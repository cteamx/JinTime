#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>   
#include <time.h>      
#include <sys/time.h>  
#include <Arduino.h>
#include <SPIFFS.h>
#include <Audio.h>

// 系统标题
const char* SYSTEM_TITLE = "三斤智能钟";
// AP WIFI 密码
const char* AP_PASSWD = "88888888";
// mDNS 地址，默认是 https://time.local
const char* MDNS_NAME = "time";

// 舵机引脚定义
#define DUOJI_PWM1 20
#define DUOJI_PWM2 21

// 语音模块引脚定义
#define VOICE_SERIAL_RX 9
#define VOICE_SERIAL_TX 10

// 播放器引脚定义
#define MP3_I2S_LRC 15
#define MP3_I2S_BCLK 16
#define MP3_I2S_DOUT 17

// NTP 时间服务器配置
const char* NTP_SERVER = "pool.ntp.org";
const long CHINA_OFFSET_SEC = 8 * 3600;  // 中国时区 GMT+8
const long USA_OFFSET_SEC = -5 * 3600;   // 美国东部时区 GMT-5 (EST)
const long UK_OFFSET_SEC = 0 * 3600;     // 英国时区 GMT+0

#define SERVOMIN 125
#define SERVOMAX 575

class ServoController {
private:
  Adafruit_PWMServoDriver pwm1;
  Adafruit_PWMServoDriver pwm2;

  // 定义每个舵机的开启角度
  int pwm1_open_angles[16] = { 0, 0, 95, 110, 0, 95, 0, 0, 95, 0, 95, 0, 0, 105, 95, 0 };
  int pwm2_open_angles[16] = { 4, 7, 99, 117, 0, 105, 0, 0, 117, 0, 110, 0, 0, 115, 100, 0 };

  // 定义每个舵机的关闭角度
  int pwm1_close_angles[16] = { 110, 110, 0, 0, 110, 0, 110, 0, 0, 110, 0, 110, 110, 0, 0, 0 };
  int pwm2_close_angles[16] = { 110, 120, 0, 0, 110, 0, 110, 0, 0, 110, 0, 110, 110, 0, 0, 0 };

  // 记录每个位置上一次显示的数字（用于优化数字0的显示逻辑）
  int lastDisplayedDigits[4] = { -1, -1, -1, -1 }; // 4个位置，初始值-1表示未设置

  // 角度转换为脉冲宽度
  int angleToPulse(int ang) {
    int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX);
    return pulse;
  }

  // 控制单个舵机的内部函数
  void controlServo(Adafruit_PWMServoDriver& pwm_driver, int channel, int state, int pwm_id) {
    int angle;

    if (pwm_id == 1) {
      angle = (state == 1) ? pwm1_open_angles[channel] : pwm1_close_angles[channel];
    } else {
      angle = (state == 1) ? pwm2_open_angles[channel] : pwm2_close_angles[channel];
    }

    pwm_driver.setPWM(channel, 0, angleToPulse(angle));
  }

public:
  // 构造函数
  ServoController()
    : pwm1(0x40), pwm2(0x41) {}

  // 初始化函数
  void begin() {
    Serial.begin(115200);
    Wire.begin(DUOJI_PWM1, DUOJI_PWM2);

    pwm1.begin();
    pwm2.begin();

    pwm1.setPWMFreq(60);
    pwm2.setPWMFreq(60);
  }

  // 简化的控制函数 - PWM1 (1-14)
  void setControl1(int state) {
    controlServo(pwm1, 0, state, 1);
  }
  void setControl2(int state) {
    controlServo(pwm1, 1, state, 1);
  }
  void setControl3(int state) {
    controlServo(pwm1, 2, state, 1);
  }
  void setControl4(int state) {
    controlServo(pwm1, 3, state, 1);
  }
  void setControl5(int state) {
    controlServo(pwm1, 4, state, 1);
  }
  void setControl6(int state) {
    controlServo(pwm1, 5, state, 1);
  }
  void setControl7(int state) {
    controlServo(pwm1, 6, state, 1);
  }
  void setControl8(int state) {
    controlServo(pwm1, 8, state, 1);
  }
  void setControl9(int state) {
    controlServo(pwm1, 9, state, 1);
  }
  void setControl10(int state) {
    controlServo(pwm1, 10, state, 1);
  }
  void setControl11(int state) {
    controlServo(pwm1, 11, state, 1);
  }
  void setControl12(int state) {
    controlServo(pwm1, 12, state, 1);
  }
  void setControl13(int state) {
    controlServo(pwm1, 13, state, 1);
  }
  void setControl14(int state) {
    controlServo(pwm1, 14, state, 1);
  }

  // 简化的控制函数 - PWM2 (15-28)
  void setControl15(int state) {
    controlServo(pwm2, 0, state, 2);
  }
  void setControl16(int state) {
    controlServo(pwm2, 1, state, 2);
  }
  void setControl17(int state) {
    controlServo(pwm2, 2, state, 2);
  }
  void setControl18(int state) {
    controlServo(pwm2, 3, state, 2);
  }
  void setControl19(int state) {
    controlServo(pwm2, 4, state, 2);
  }
  void setControl20(int state) {
    controlServo(pwm2, 5, state, 2);
  }
  void setControl21(int state) {
    controlServo(pwm2, 6, state, 2);
  }
  void setControl22(int state) {
    controlServo(pwm2, 8, state, 2);
  }
  void setControl23(int state) {
    controlServo(pwm2, 9, state, 2);
  }
  void setControl24(int state) {
    controlServo(pwm2, 10, state, 2);
  }
  void setControl25(int state) {
    controlServo(pwm2, 11, state, 2);
  }
  void setControl26(int state) {
    controlServo(pwm2, 12, state, 2);
  }
  void setControl27(int state) {
    controlServo(pwm2, 13, state, 2);
  }
  void setControl28(int state) {
    controlServo(pwm2, 14, state, 2);
  }

  // 控制所有舵机的函数
  void controlAllServos(int state) {
    if (state == 1) {
      Serial.println("=== 下发是开启指令 ===");
      
      // 开启时直接设置所有舵机
      setControl1(state);
      setControl2(state);
      setControl3(state);
      setControl4(state);
      setControl5(state);
      setControl6(state);
      setControl7(state);
      setControl8(state);
      setControl9(state);
      setControl10(state);
      setControl11(state);
      setControl12(state);
      setControl13(state);
      setControl14(state);
      setControl15(state);
      setControl16(state);
      setControl17(state);
      setControl18(state);
      setControl19(state);
      setControl20(state);
      setControl21(state);
      setControl22(state);
      setControl23(state);
      setControl24(state);
      setControl25(state);
      setControl26(state);
      setControl27(state);
      setControl28(state);
      
    } else {
      Serial.println("=== 下发是关闭指令（安全分阶段关闭）===");
      
      // 关闭时采用安全的分阶段策略，避免第1段和第5段与第6段冲突
      // 阶段1：先关闭第1段和第5段（避免它们与第6段冲突）
      Serial.println("阶段1：关闭第1段和第5段");
      setControl2(0);   // 位置1 第1段
      setControl6(0);   // 位置1 第5段
      setControl9(0);   // 位置2 第1段
      setControl13(0);  // 位置2 第5段
      setControl16(0);  // 位置3 第1段
      setControl20(0);  // 位置3 第5段
      setControl23(0);  // 位置4 第1段
      setControl27(0);  // 位置4 第5段
      
      // 阶段2：等待舵机安全到位
      delay(150);
      
      // 阶段3：关闭第6段（横中段）
      Serial.println("阶段2：关闭第6段");
      setControl7(0);   // 位置1 第6段
      setControl14(0);  // 位置2 第6段
      setControl21(0);  // 位置3 第6段
      setControl28(0);  // 位置4 第6段
      
      // 阶段4：关闭其他所有段
      Serial.println("阶段3：关闭其他所有段");
      setControl1(0);
      setControl3(0);
      setControl4(0);
      setControl5(0);
      setControl8(0);
      setControl10(0);
      setControl11(0);
      setControl12(0);
      setControl15(0);
      setControl17(0);
      setControl18(0);
      setControl19(0);
      setControl22(0);
      setControl24(0);
      setControl25(0);
      setControl26(0);
      
      Serial.println("=== 安全关机完成 ===");
    }
    
    // 重置位置状态记录
    for (int i = 0; i < 4; i++) {
      lastDisplayedDigits[i] = -1;
    }
  }

  // =================== 数字时钟功能 ===================

  // 7段数码管的段定义 (0, 1, 2, 3, 4, 5, 6)
  //     0
  //   5   1
  //     6
  //   4   2
  //     3

  // 数字0-9的7段显示模式 (1=显示段, 0=不显示段)
  // 顺序: 0, 1, 2, 3, 4, 5, 6
  bool digitPatterns[10][7] = {
    { 1, 1, 1, 1, 1, 1, 0 },  // 0: 012345
    { 0, 1, 1, 0, 0, 0, 0 },  // 1: 12
    { 1, 1, 0, 1, 1, 0, 1 },  // 2: 01346
    { 1, 1, 1, 1, 0, 0, 1 },  // 3: 01236
    { 0, 1, 1, 0, 0, 1, 1 },  // 4: 1256
    { 1, 0, 1, 1, 0, 1, 1 },  // 5: 02356
    { 1, 0, 1, 1, 1, 1, 1 },  // 6: 023456
    { 1, 1, 1, 0, 0, 0, 0 },  // 7: 012
    { 1, 1, 1, 1, 1, 1, 1 },  // 8: 0123456
    { 1, 1, 1, 1, 0, 1, 1 }   // 9: 012356
  };

  // 显示单个数字 (digit: 0-9, position: 第几个数字位置 1-4)
  void displayDigit(int digit, int position) {
    if (digit < 0 || digit > 9 || position < 1 || position > 4) {
      return;  // 参数检查
    }

    // 计算每个数字位置的起始控制器编号
    int startControl = (position - 1) * 7 + 1;

    Serial.print("显示数字 ");
    Serial.print(digit);
    Serial.print(" 在位置 ");
    Serial.println(position);

    // 检查是否需要特殊处理数字0
    // 任何包含第6段的数字切换到0都需要安全模式，因为第6段关闭会与第5段冲突
    // 包含第6段的数字：2,3,4,5,6,8,9
    int lastDigit = lastDisplayedDigits[position - 1];
    bool lastDigitHasSegment6 = (lastDigit == 2 || lastDigit == 3 || lastDigit == 4 || 
                                lastDigit == 5 || lastDigit == 6 || lastDigit == 8 || lastDigit == 9);
    bool needSafeMode = (digit == 0 && lastDigitHasSegment6);
    
    if (needSafeMode) {
      // 从包含第6段的数字切换到数字0的三阶段安全显示：
      // 阶段1：先关闭第1段、第5段和第6段（避免冲突）
      // 阶段2：等待舵机安全到位
      // 阶段3：重新开启所需的段
      
      int segment1ControlNum = startControl + 1; // 第1段（右上段）
      int segment5ControlNum = startControl + 5; // 第5段（左上段）
      int segment6ControlNum = startControl + 6; // 第6段（横中段）
      
      Serial.print("数字");
      Serial.print(lastDigit);
      Serial.println("→0：需要使用安全模式（第1段和第5段与第6段冲突）");
      
      // 阶段1：先关闭第1段、第5段和第6段
      switch (segment1ControlNum) {
        case 2: setControl2(0); break;
        case 9: setControl9(0); break;
        case 16: setControl16(0); break;
        case 23: setControl23(0); break;
      }
      
      switch (segment5ControlNum) {
        case 6: setControl6(0); break;
        case 13: setControl13(0); break;
        case 20: setControl20(0); break;
        case 27: setControl27(0); break;
      }
      
      switch (segment6ControlNum) {
        case 7: setControl7(0); break;
        case 14: setControl14(0); break;
        case 21: setControl21(0); break;
        case 28: setControl28(0); break;
      }
      
      // 阶段2：等待舵机安全到位
      delay(100);
      
      // 阶段3：开启所有需要的段（0,1,2,3,4,5），注意第6段保持关闭
      for (int i = 0; i < 7; i++) {
        int controlNum = startControl + i;
        int state = digitPatterns[digit][i] ? 1 : 0;

        switch (controlNum) {
          case 1: setControl1(state); break;
          case 2: setControl2(state); break;
          case 3: setControl3(state); break;
          case 4: setControl4(state); break;
          case 5: setControl5(state); break;
          case 6: setControl6(state); break;
          case 7: setControl7(state); break;
          case 8: setControl8(state); break;
          case 9: setControl9(state); break;
          case 10: setControl10(state); break;
          case 11: setControl11(state); break;
          case 12: setControl12(state); break;
          case 13: setControl13(state); break;
          case 14: setControl14(state); break;
          case 15: setControl15(state); break;
          case 16: setControl16(state); break;
          case 17: setControl17(state); break;
          case 18: setControl18(state); break;
          case 19: setControl19(state); break;
          case 20: setControl20(state); break;
          case 21: setControl21(state); break;
          case 22: setControl22(state); break;
          case 23: setControl23(state); break;
          case 24: setControl24(state); break;
          case 25: setControl25(state); break;
          case 26: setControl26(state); break;
          case 27: setControl27(state); break;
          case 28: setControl28(state); break;
        }
      }
      Serial.println("数字0：使用三阶段安全显示模式（避免第1段和第5段与第6段关闭冲突）");
    } else {
      // 其他数字正常显示
      for (int i = 0; i < 7; i++) {
        int controlNum = startControl + i;
        int state = digitPatterns[digit][i] ? 1 : 0;

        // 调用对应的setControl函数
        switch (controlNum) {
          case 1: setControl1(state); break;
          case 2: setControl2(state); break;
          case 3: setControl3(state); break;
          case 4: setControl4(state); break;
          case 5: setControl5(state); break;
          case 6: setControl6(state); break;
          case 7: setControl7(state); break;
          case 8: setControl8(state); break;
          case 9: setControl9(state); break;
          case 10: setControl10(state); break;
          case 11: setControl11(state); break;
          case 12: setControl12(state); break;
          case 13: setControl13(state); break;
          case 14: setControl14(state); break;
          case 15: setControl15(state); break;
          case 16: setControl16(state); break;
          case 17: setControl17(state); break;
          case 18: setControl18(state); break;
          case 19: setControl19(state); break;
          case 20: setControl20(state); break;
          case 21: setControl21(state); break;
          case 22: setControl22(state); break;
          case 23: setControl23(state); break;
          case 24: setControl24(state); break;
          case 25: setControl25(state); break;
          case 26: setControl26(state); break;
          case 27: setControl27(state); break;
          case 28: setControl28(state); break;
        }
      }
    }
    
    // 更新位置状态记录
    lastDisplayedDigits[position - 1] = digit;
  }

  // 显示4位数字 (例如时间 12:34 显示为 1234)
  void displayTime(int hour, int minute) {
    int digit1 = hour / 10;    // 小时十位
    int digit2 = hour % 10;    // 小时个位
    int digit3 = minute / 10;  // 分钟十位
    int digit4 = minute % 10;  // 分钟个位

    Serial.print("显示时间: ");
    Serial.print(hour);
    Serial.print(":");
    if (minute < 10) Serial.print("0");
    Serial.println(minute);

    displayDigit(digit1, 1);  // 第1个数字位置
    displayDigit(digit2, 2);  // 第2个数字位置
    displayDigit(digit3, 3);  // 第3个数字位置
    displayDigit(digit4, 4);  // 第4个数字位置
  }

  // 显示4位数字 (直接输入4位数)
  void displayNumber(int number) {
    if (number < 0 || number > 9999) {
      number = 0;  // 超出范围则显示0000
    }

    int digit1 = (number / 1000) % 10;  // 千位
    int digit2 = (number / 100) % 10;   // 百位
    int digit3 = (number / 10) % 10;    // 十位
    int digit4 = number % 10;           // 个位

    Serial.print("显示数字: ");
    Serial.println(number);

    displayDigit(digit1, 1);
    displayDigit(digit2, 2);
    displayDigit(digit3, 3);
    displayDigit(digit4, 4);
  }


  void displayNumberString(const String& input) {
    // 先关闭所有时钟/段
    clearDisplay();

    // 提取数字字符
    String digits = "";
    digits.reserve(8);
    for (size_t i = 0; i < input.length(); i++) {
      char c = input.charAt(i);
      if (c >= '0' && c <= '9') {
        digits += c;
      }
    }

    if (digits.length() == 0) {
      Serial.println("displayNumberString: 无有效数字");
      return;
    }

    // 最多支持4位，超过4位取前4位
    if (digits.length() > 4) {
      digits = digits.substring(0, 4);
    }

    // 右对齐显示在最右侧，左侧保持关闭
    int len = digits.length();
    int startPos = 4 - len + 1;  // 位置范围1~4

    Serial.print("displayNumberString 显示: ");
    Serial.println(digits);

    for (int i = 0; i < len; i++) {
      int d = digits.charAt(i) - '0';
      displayDigit(d, startPos + i);
    }
  }

  // 清空所有显示
  void clearDisplay() {
    Serial.println("清空显示");
    controlAllServos(0);
    // 重置位置状态记录
    for (int i = 0; i < 4; i++) {
      lastDisplayedDigits[i] = -1;
    }
  }

  // 直接设置指定位置的指定段（position:1-4, segmentIndex:0-6）
  void setSegment(int position, int segmentIndex, int state) {
    if (position < 1 || position > 4 || segmentIndex < 0 || segmentIndex > 6) return;
    int controlNum = (position - 1) * 7 + 1 + segmentIndex;
    switch (controlNum) {
      case 1: setControl1(state); break;
      case 2: setControl2(state); break;
      case 3: setControl3(state); break;
      case 4: setControl4(state); break;
      case 5: setControl5(state); break;
      case 6: setControl6(state); break;
      case 7: setControl7(state); break;
      case 8: setControl8(state); break;
      case 9: setControl9(state); break;
      case 10: setControl10(state); break;
      case 11: setControl11(state); break;
      case 12: setControl12(state); break;
      case 13: setControl13(state); break;
      case 14: setControl14(state); break;
      case 15: setControl15(state); break;
      case 16: setControl16(state); break;
      case 17: setControl17(state); break;
      case 18: setControl18(state); break;
      case 19: setControl19(state); break;
      case 20: setControl20(state); break;
      case 21: setControl21(state); break;
      case 22: setControl22(state); break;
      case 23: setControl23(state); break;
      case 24: setControl24(state); break;
      case 25: setControl25(state); break;
      case 26: setControl26(state); break;
      case 27: setControl27(state); break;
      case 28: setControl28(state); break;
    }
  }

  // 显示一个简单的“笑脸”表情
  // 眼睛：第1位与第4位点亮中间段（segment 6）
  // 嘴巴：第2与第3位点亮下左(4)、下(3)、下右(2)
  void displayEmojiSmile() {
    clearDisplay();
    // Eyes
    setSegment(1, 6, 1);
    setSegment(4, 6, 1);
    // Mouth
    setSegment(2, 4, 1);
    setSegment(2, 3, 1);
    setSegment(2, 2, 1);
    setSegment(3, 4, 1);
    setSegment(3, 3, 1);
    setSegment(3, 2, 1);
    Serial.println("显示表情：笑脸");
  }

};

// =================== SimpleTimeManager 类 ===================
class SimpleTimeManager {
private:
  bool timeInitialized;
  unsigned long lastSyncTime;
  const unsigned long SYNC_INTERVAL = 3600000;  // 每小时同步一次 (毫秒)

public:
  SimpleTimeManager()
    : timeInitialized(false), lastSyncTime(0) {}

  // 初始化NTP时间同步（只在WiFi连接成功后调用）
  void initTimeSync() {
    if (WiFi.status() != WL_CONNECTED) {
      return;
    }

    Serial.println("[Time] Initializing NTP sync...");
    configTime(CHINA_OFFSET_SEC, 0, NTP_SERVER);

    // 等待时间同步
    int attempts = 0;
    while (!timeInitialized && attempts < 10) {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        timeInitialized = true;
        lastSyncTime = millis();
        Serial.println("[Time] NTP sync successful!");
        Serial.printf("[Time] Beijing time: %02d:%02d:%02d\n",
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        break;
      }
      delay(1000);
      attempts++;
      Serial.print(".");
    }

    if (!timeInitialized) {
      Serial.println("\n[Time] NTP sync failed");
    }
  }

  // 检查是否需要重新同步时间（在loop中调用）
  void handleTimeSync() {
    // 只有在WiFi连接且初始化成功的情况下才处理
    if (WiFi.status() != WL_CONNECTED) {
      return;
    }

    // 如果还没有初始化，尝试初始化
    if (!timeInitialized) {
      initTimeSync();
      return;
    }

    // 检查是否需要重新同步
    unsigned long currentTime = millis();
    if (currentTime - lastSyncTime > SYNC_INTERVAL) {
      Serial.println("[Time] Re-syncing NTP...");
      configTime(CHINA_OFFSET_SEC, 0, NTP_SERVER);
      lastSyncTime = currentTime;
    }
  }

  // 获取中国北京时间 (GMT+8)
  String getChinaTime() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return "--:--";
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return "--:--";
    }

    char timeStr[16];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeStr);
  }

  // 获取美国东部时间 (GMT-5)
  String getUSATime() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return "--:--";
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    // 转换为UTC再加上美国东部时区偏移
    now = now - CHINA_OFFSET_SEC + USA_OFFSET_SEC;
    gmtime_r(&now, &timeinfo);

    char timeStr[16];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeStr);
  }

  // 获取英国时间 (GMT+0)
  String getUKTime() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return "--:--";
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    // 转换为UTC再加上英国时区偏移
    now = now - CHINA_OFFSET_SEC + UK_OFFSET_SEC;
    gmtime_r(&now, &timeinfo);

    char timeStr[16];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeStr);
  }

  // 获取带秒的中国时间
  String getChinaTimeWithSeconds() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return "--:--:--";
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return "--:--:--";
    }

    char timeStr[16];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    return String(timeStr);
  }

  // 获取中国时间的分钟和秒数（用于时钟显示）
  bool getChinaMinuteSecond(int* minute, int* second) {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return false;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return false;
    }

    *minute = timeinfo.tm_min;
    *second = timeinfo.tm_sec;
    return true;
  }

  // 获取中国时间的完整信息：年、小时、分、秒
  bool getChinaFullTime(int* year, int* hour, int* minute, int* second) {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return false;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return false;
    }

    *year = timeinfo.tm_year + 1900;  // tm_year 是从1900年开始计算
    *hour = timeinfo.tm_hour;
    *minute = timeinfo.tm_min;
    *second = timeinfo.tm_sec;
    return true;
  }

  // 获取中国时间的年份
  int getChinaYear() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return 0;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return 0;
    }

    return timeinfo.tm_year + 1900;
  }

  // 获取中国时间的小时
  int getChinaHour() {
    if (!timeInitialized || WiFi.status() != WL_CONNECTED) {
      return 0;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return 0;
    }

    return timeinfo.tm_hour;
  }

  // 检查时间是否已初始化
  bool isInitialized() const {
    return timeInitialized && WiFi.status() == WL_CONNECTED;
  }

  // 打印所有时区时间（用于调试）
  void printAllTimes() {
    if (!isInitialized()) {
      Serial.println("[Time] Time not initialized or WiFi disconnected");
      return;
    }

    Serial.println("=== 世界时间 ===");
    Serial.println("中国北京: " + getChinaTimeWithSeconds());
    Serial.println("美国东部: " + getUSATime());
    Serial.println("英国伦敦: " + getUKTime());
    Serial.println("===============");
  }
};

// =================== WiFiManager 类 ===================
class WiFiManager {
private:
  const char* apSSID;
  const char* apPassword;
  const char* mdnsName;

  Preferences preferences;
  String storedSSID;
  String storedPassword;
  IPAddress apIP;
  DNSServer dnsServer;

public:
  WiFiManager(const char* ap_ssid, const char* ap_password, const char* mdns_name = MDNS_NAME)
    : apSSID(ap_ssid), apPassword(ap_password), mdnsName(mdns_name), apIP(192, 168, 4, 1) {}

  // 初始化 WiFi 配置
  void begin() {
    loadStoredCredentials();
  }

  // 从 Preferences 加载保存的 WiFi 凭据
  void loadStoredCredentials() {
    preferences.begin("wifi", true);  // read-only
    storedSSID = preferences.getString("ssid", "");
    storedPassword = preferences.getString("password", "");
    preferences.end();

    Serial.println("[WiFi] Loaded credentials - SSID: " + storedSSID);
  }

  // 检查是否是首次连接AP（没有保存的WiFi凭据）
  bool isFirstConnection() const {
    return storedSSID.isEmpty();
  }

  // 保存 WiFi 凭据到 Preferences
  void saveCredentials(const String& ssid, const String& password) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();

    storedSSID = ssid;
    storedPassword = password;
    Serial.println("[WiFi] Credentials saved - SSID: " + ssid);
  }

  // 清除保存的 WiFi 凭据
  void clearCredentials() {
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();

    storedSSID = "";
    storedPassword = "";
    Serial.println("[WiFi] Credentials cleared");
  }

  // 连接到 STA 模式
  bool connectToWiFi(const String& ssid, const String& password, int tryTime = 20) {
    if (ssid.isEmpty() || password.isEmpty()) {
      Serial.println("[WiFi] SSID/Password is empty. Skip STA connecting...");
      return false;
    }

    Serial.println("[WiFi] Connecting to: " + ssid);
    WiFi.mode(WIFI_STA);
    // WiFi.setHostname(SYSTEM_TITLE);
    WiFi.begin(ssid.c_str(), password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < tryTime) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[WiFi] Connected!");
      Serial.print("[WiFi] Local IP: ");
      Serial.println(WiFi.localIP());

      setupMDNS();
      return true;
    } else {
      Serial.println("\n[WiFi] Failed to connect.");
      return false;
    }
  }

  // 使用存储的凭据连接
  bool connectWithStoredCredentials() {
    return connectToWiFi(storedSSID, storedPassword);
  }

  // 启动 AP 模式
  void startAPMode() {
    Serial.println("[WiFi] Starting AP Mode...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);
    Serial.print("[WiFi] AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // DNS 转发所有域名到 AP IP
    dnsServer.start(53, "*", apIP);
  }

  // 设置 mDNS
  void setupMDNS() {
    if (MDNS.begin(mdnsName)) {
      Serial.println("[mDNS] mDNS responder started: http://" + String(mdnsName) + ".local");
      MDNS.addService("http", "tcp", 80);
    } else {
      Serial.println("[mDNS] Error setting up MDNS responder!");
    }
  }

  // 处理 DNS 请求（在 loop 中调用）
  void handleDNS() {
    dnsServer.processNextRequest();
  }

  // 获取存储的 SSID
  String getStoredSSID() const {
    return storedSSID;
  }

  // 获取存储的密码
  String getStoredPassword() const {
    return storedPassword;
  }

  // 检查是否有存储的凭据
  bool hasStoredCredentials() const {
    return !storedSSID.isEmpty() && !storedPassword.isEmpty();
  }

  // 获取当前 WiFi 状态
  String getConnectionStatus() const {
    switch (WiFi.status()) {
      case WL_CONNECTED: return "Connected";
      case WL_NO_SSID_AVAIL: return "SSID not available";
      case WL_CONNECT_FAILED: return "Connection failed";
      case WL_CONNECTION_LOST: return "Connection lost";
      case WL_DISCONNECTED: return "Disconnected";
      default: return "Unknown";
    }
  }

  // 扫描附近的WiFi网络
  String scanWiFiNetworks() {
    Serial.println("[WiFi] Scanning for networks...");
    int networkCount = WiFi.scanNetworks();

    String wifiList = "[";
    if (networkCount > 0) {
      for (int i = 0; i < networkCount; i++) {
        if (i > 0) wifiList += ",";
        wifiList += "{";
        wifiList += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        wifiList += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        wifiList += "\"encryption\":" + String(WiFi.encryptionType(i));
        wifiList += "}";
      }
    }
    wifiList += "]";

    WiFi.scanDelete();  // 清除扫描结果以释放内存
    Serial.println("[WiFi] Found " + String(networkCount) + " networks");
    return wifiList;
  }
};

// =================== WebServerManager 类 ===================
class WebServerManager {
private:
  WebServer server;
  WiFiManager* wifiManager;
  ServoController* servoController;
  Audio* audioPlayer;

  // MP3播放后显示控制变量
  unsigned long mp3DisplayStartTime;
  bool mp3DisplayActive;

public:
  WebServerManager(int port = 80)
    : server(port), mp3DisplayStartTime(0), mp3DisplayActive(false) {}

  // 设置依赖的管理器
  void setWiFiManager(WiFiManager* manager) {
    wifiManager = manager;
  }

  void setServoController(ServoController* controller) {
    servoController = controller;
  }

  void setAudioPlayer(Audio* audio) {
    audioPlayer = audio;
  }

  // 初始化服务器路由
  void begin() {
    setupRoutes();
    server.begin();
    Serial.println("[Server] Web server started");
  }

  // 设置所有路由
  void setupRoutes() {
    server.on("/", HTTP_GET, [this]() {
      handleRoot();
    });
    server.on("/config", HTTP_GET, [this]() {
      handleConfig();
    });
    server.on("/save", HTTP_POST, [this]() {
      handleSave();
    });
    server.on("/reset", HTTP_GET, [this]() {
      handleReset();
    });
    server.on("/scan", HTTP_GET, [this]() {
      handleWiFiScan();
    });

    // 时钟控制路由
    server.on("/led1/onall", HTTP_GET, [this]() {
      handleAllServos(1);
    });
    server.on("/led1/offall", HTTP_GET, [this]() {
      handleAllServos(0);
    });

    // MP3播放控制路由
    server.on("/mp3/play", HTTP_GET, [this]() {
      handleMP3Play();
    });

    server.onNotFound([this]() {
      handleRoot();
    });
  }

  // 处理客户端请求（在 loop 中调用）
  void handleClient() {
    server.handleClient();
  }

  // 检查MP3显示超时（在 loop 中调用）
  bool checkMP3DisplayTimeout() {
    if (mp3DisplayActive) {
      unsigned long currentTime = millis();
      if (currentTime - mp3DisplayStartTime >= 1000 * 10) {
        mp3DisplayActive = false;
        Serial.println("[MP3] 10秒显示时间结束，恢复时间显示");
        return true;  // 返回true表示需要恢复时间显示
      }
    }
    return false;  // 返回false表示不需要恢复或者没有激活MP3显示
  }

  // 检查MP3显示是否激活
  bool isMP3DisplayActive() const {
    return mp3DisplayActive;
  }

  // 主页处理
  void handleRoot() {
    // 如果是AP模式（无论是首次连接还是连接失败），直接重定向到配置页面
    // 只有在WiFi连接成功时才显示主页
    if (WiFi.getMode() == WIFI_AP) {
      server.sendHeader("Location", "/config");
      server.send(302, "text/plain", "");
      return;
    }

    String html = generateMainPage();
    server.send(200, "text/html", html);
  }

  // 配置页面处理
  void handleConfig() {
    String html = generateConfigPage();
    server.send(200, "text/html", html);
  }

  // 保存配置处理
  void handleSave() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
      String newSSID = server.arg("ssid");
      String newPassword = server.arg("password");

      wifiManager->saveCredentials(newSSID, newPassword);

      // 断开当前 AP，并尝试连接新的 Wi-Fi
      WiFi.softAPdisconnect(true);
      bool connected = wifiManager->connectToWiFi(newSSID, newPassword);

      if (connected) {
        // 连接成功后重定向到主页控制页面
        String successHtml = generateSuccessPage();
        server.send(200, "text/html", successHtml);
      } else {
        wifiManager->startAPMode();
        String failHtml = generateErrorPage("Wi-Fi 连接失败", "无法连接到指定的WiFi网络，请检查密码是否正确", "/config", "重新配置");
        server.send(200, "text/html", failHtml);
      }
    } else {
      String invalidHtml = generateErrorPage("输入无效", "请确保输入了正确的WiFi名称和密码", "/config", "重新输入");
      server.send(200, "text/html", invalidHtml);
    }
  }

  // 重置处理
  void handleReset() {
    wifiManager->clearCredentials();

    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>恢复出厂设置 - " + String(SYSTEM_TITLE) + "</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  background: linear-gradient(135deg, #ff7b7b 0%, #ff416c 100%); ";
    html += "  overflow: hidden; position: fixed; top: 0; left: 0; ";
    html += "  -webkit-overflow-scrolling: touch; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #fff5f5 0%, #ffffff 100%); ";
    html += "  border: 1px solid #ffcdd2; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(255,65,108,0.2), 0 8px 16px rgba(255,65,108,0.1); ";
    html += "  padding: 40px; position: absolute; top: 50%; left: 20px; right: 20px; ";
    html += "  transform: translateY(-50%); text-align: center; }";
    html += "h1 { color: #d32f2f; margin-bottom: 20px; font-size: 28px; font-weight: 600; }";
    html += ".icon { font-size: 64px; margin-bottom: 20px; animation: spin 2s linear infinite; }";
    html += ".message { color: #666; font-size: 18px; margin-bottom: 30px; line-height: 1.5; }";
    html += ".progress-container { ";
    html += "  width: 100%; height: 8px; background: #ffcdd2; border-radius: 4px; ";
    html += "  margin: 20px 0; overflow: hidden; }";
    html += ".progress-bar { ";
    html += "  height: 100%; background: linear-gradient(90deg, #ff416c 0%, #ff4757 100%); ";
    html += "  width: 0%; border-radius: 4px; ";
    html += "  animation: progress 3s ease-in-out forwards; }";
    html += ".countdown { ";
    html += "  font-size: 24px; font-weight: 600; color: #d32f2f; margin-top: 20px; }";
    html += "@keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }";
    html += "@keyframes progress { 0% { width: 0%; } 100% { width: 100%; } }";
    html += "@keyframes fadeIn { 0% { opacity: 0; transform: scale(0.9); } 100% { opacity: 1; transform: scale(1); } }";
    html += ".container { animation: fadeIn 0.5s ease-out; }";
    html += "@media (max-width: 480px) { ";
    html += "  .container { left: 15px; right: 15px; padding: 30px; }";
    html += "  h1 { font-size: 24px; }";
    html += "  .icon { font-size: 48px; }";
    html += "  .message { font-size: 16px; }";
    html += "  .countdown { font-size: 20px; }";
    html += "}";
    html += "</style>";
    html += "<script>";
    html += "let countdown = 3;";
    html += "function updateCountdown() {";
    html += "  const element = document.getElementById('countdown');";
    html += "  if (countdown > 0) {";
    html += "    element.textContent = countdown + ' 秒后重启...';";
    html += "    countdown--;";
    html += "    setTimeout(updateCountdown, 1000);";
    html += "  } else {";
    html += "    element.textContent = '正在重启设备...';";
    html += "  }";
    html += "}";
    html += "window.onload = function() { updateCountdown(); };";
    html += "</script>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<div class='icon'>🔄</div>";
    html += "<h1>正在恢复出厂设置</h1>";
    html += "<p class='message'>系统正在清除所有配置信息并准备重启<br/>请稍等片刻，不要断开电源</p>";
    html += "<div class='progress-container'>";
    html += "<div class='progress-bar'></div>";
    html += "</div>";
    html += "<div class='countdown' id='countdown'>3 秒后重启...</div>";
    html += "</div>";
    html += "</body></html>";

    server.send(200, "text/html", html);
    delay(3000);  // 给用户时间看到页面
    ESP.restart();
  }

  // 舵机控制处理
  void handleServoControl(int servoNum, int state) {
    if (!servoController) {
      String errorHtml = generateErrorPage("系统错误", "舵机控制器未正确初始化，请重启设备", "/", "返回主页");
      server.send(500, "text/html", errorHtml);
      return;
    }

    switch (servoNum) {
      case 1: servoController->setControl1(state); break;
      case 2: servoController->setControl2(state); break;
      case 3: servoController->setControl3(state); break;
      default: break;
    }

    String actionText = state ? "开启" : "关闭";
    String successHtml = generateActionSuccessPage("舵机控制成功", "舵机 " + String(servoNum) + " 已" + actionText);
    server.send(200, "text/html", successHtml);
  }

  // 所有舵机控制处理
  void handleAllServos(int state) {
    if (!servoController) {
      String errorHtml = generateErrorPage("系统错误", "舵机控制器未正确初始化，请重启设备", "/", "返回主页");
      server.send(500, "text/html", errorHtml);
      return;
    }

    servoController->controlAllServos(state);

    String actionText = state ? "开启" : "关闭";
    String successHtml = generateActionSuccessPage("时钟控制成功", "所有舵机已" + actionText);
    server.send(200, "text/html", successHtml);
  }

  // WiFi扫描处理
  void handleWiFiScan() {
    if (!wifiManager) {
      server.send(500, "application/json", "{\"error\":\"WiFi管理器未初始化\"}");
      return;
    }

    String wifiList = wifiManager->scanWiFiNetworks();
    server.send(200, "application/json", wifiList);
  }

  // MP3播放处理
  void handleMP3Play() {
    if (!audioPlayer) {
      server.send(500, "application/json", "{\"error\":\"音频播放器未初始化\"}");
      return;
    }

    // 获取参数
    String playUrl = server.arg("play_url");
    String numberStr = server.arg("number");

    if (playUrl.isEmpty()) {
      server.send(400, "application/json", "{\"error\":\"缺少play_url参数\"}");
      return;
    }

    // 停止当前播放
    audioPlayer->stopSong();

    // 开始播放新的URL
    bool success = audioPlayer->connecttohost(playUrl.c_str());

    if (success) {
      String response = "{\"status\":\"success\",\"message\":\"开始播放音频\",\"url\":\"" + playUrl + "\"";
      if (!numberStr.isEmpty()) {
        response += ",\"number\":" + numberStr;
      }
      response += "}";
      server.send(200, "application/json", response);

      Serial.println("[MP3] 开始播放: " + playUrl);
      if (!numberStr.isEmpty()) {
        // 显示编号到舵机时钟
        if (servoController) {          
          servoController->displayNumberString(numberStr); 
          Serial.println("[MP3] 舵机显示编号: " + String(numberStr));
        }

        // 设置10秒后恢复时间显示的标志
        mp3DisplayStartTime = millis();
        mp3DisplayActive = true;
      }

    } else {
      String errorResponse = "{\"status\":\"error\",\"message\":\"无法连接到音频源\",\"url\":\"" + playUrl + "\"}";
      server.send(500, "application/json", errorResponse);

      Serial.println("[MP3] 播放失败: " + playUrl);
    }
  }

private:
  // 生成操作成功页面 HTML
  String generateActionSuccessPage(const String& title, const String& message) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>" + title + " - " + String(SYSTEM_TITLE) + "</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
    html += "  overflow: hidden; position: fixed; top: 0; left: 0; ";
    html += "  -webkit-overflow-scrolling: touch; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #f8f9ff 0%, #ffffff 100%); ";
    html += "  border: 1px solid #e1e5e9; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(102,126,234,0.2), 0 8px 16px rgba(102,126,234,0.1); ";
    html += "  padding: 40px; position: absolute; top: 50%; left: 20px; right: 20px; ";
    html += "  transform: translateY(-50%); text-align: center; }";
    html += "h1 { color: #667eea; margin-bottom: 20px; font-size: 28px; font-weight: 600; }";
    html += ".icon { font-size: 64px; margin-bottom: 20px; animation: pulse 2s ease-in-out infinite; }";
    html += ".message { color: #666; font-size: 18px; margin-bottom: 30px; line-height: 1.5; }";
    html += ".countdown { ";
    html += "  font-size: 18px; font-weight: 500; color: #667eea; margin-bottom: 20px; }";
    html += ".back-btn { ";
    html += "  display: inline-block; padding: 15px 30px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
    html += "  color: white; text-decoration: none; border-radius: 12px; font-size: 16px; font-weight: 600; ";
    html += "  transition: all 0.3s ease; box-shadow: 0 4px 12px rgba(102,126,234,0.3); }";
    html += ".back-btn:hover { transform: translateY(-2px); box-shadow: 0 8px 20px rgba(102,126,234,0.4); }";
    html += ".back-btn:active { transform: translateY(0); }";
    html += "@keyframes pulse { 0%, 100% { transform: scale(1); } 50% { transform: scale(1.1); } }";
    html += "@keyframes fadeIn { 0% { opacity: 0; transform: scale(0.9); } 100% { opacity: 1; transform: scale(1); } }";
    html += ".container { animation: fadeIn 0.5s ease-out; }";
    html += "@media (max-width: 480px) { ";
    html += "  .container { left: 15px; right: 15px; padding: 30px; }";
    html += "  h1 { font-size: 24px; }";
    html += "  .icon { font-size: 48px; }";
    html += "  .message { font-size: 16px; }";
    html += "  .countdown { font-size: 16px; }";
    html += "  .back-btn { padding: 12px 24px; font-size: 14px; }";
    html += "}";
    html += "</style>";
    html += "<script>";
    html += "let countdown = 2;";
    html += "function updateCountdown() {";
    html += "  const element = document.getElementById('countdown');";
    html += "  if (countdown > 0) {";
    html += "    element.textContent = countdown + ' 秒后自动返回主页';";
    html += "    countdown--;";
    html += "    setTimeout(updateCountdown, 1000);";
    html += "  } else {";
    html += "    window.location.href = '/';";
    html += "  }";
    html += "}";
    html += "window.onload = function() { updateCountdown(); };";
    html += "</script>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<div class='icon'>🎯</div>";
    html += "<h1>" + title + "</h1>";
    html += "<p class='message'>" + message + "</p>";
    html += "<div class='countdown' id='countdown'>2 秒后自动返回主页</div>";
    html += "<a href='/' class='back-btn'>立即返回主页</a>";
    html += "</div>";
    html += "</body></html>";
    return html;
  }

  // 生成成功页面 HTML
  String generateSuccessPage() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>连接成功 - " + String(SYSTEM_TITLE) + "</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  background: linear-gradient(135deg, #4caf50 0%, #2e7d32 100%); ";
    html += "  overflow: hidden; position: fixed; top: 0; left: 0; ";
    html += "  -webkit-overflow-scrolling: touch; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #f1f8e9 0%, #ffffff 100%); ";
    html += "  border: 1px solid #c8e6c9; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(46,125,50,0.2), 0 8px 16px rgba(46,125,50,0.1); ";
    html += "  padding: 40px; position: absolute; top: 50%; left: 20px; right: 20px; ";
    html += "  transform: translateY(-50%); text-align: center; }";
    html += "h1 { color: #2e7d32; margin-bottom: 20px; font-size: 28px; font-weight: 600; }";
    html += ".icon { font-size: 64px; margin-bottom: 20px; animation: bounce 1s ease-in-out infinite alternate; }";
    html += ".message { color: #666; font-size: 18px; margin-bottom: 30px; line-height: 1.5; }";
    html += ".countdown { ";
    html += "  font-size: 20px; font-weight: 600; color: #2e7d32; margin-bottom: 20px; }";
    html += ".jump-btn { ";
    html += "  display: inline-block; padding: 15px 30px; background: linear-gradient(135deg, #4caf50 0%, #2e7d32 100%); ";
    html += "  color: white; text-decoration: none; border-radius: 12px; font-size: 16px; font-weight: 600; ";
    html += "  transition: all 0.3s ease; box-shadow: 0 4px 12px rgba(76,175,80,0.3); }";
    html += ".jump-btn:hover { transform: translateY(-2px); box-shadow: 0 8px 20px rgba(76,175,80,0.4); }";
    html += ".jump-btn:active { transform: translateY(0); }";
    html += "@keyframes bounce { 0% { transform: translateY(0); } 100% { transform: translateY(-10px); } }";
    html += "@keyframes fadeIn { 0% { opacity: 0; transform: scale(0.9); } 100% { opacity: 1; transform: scale(1); } }";
    html += ".container { animation: fadeIn 0.5s ease-out; }";
    html += "@media (max-width: 480px) { ";
    html += "  .container { left: 15px; right: 15px; padding: 30px; }";
    html += "  h1 { font-size: 24px; }";
    html += "  .icon { font-size: 48px; }";
    html += "  .message { font-size: 16px; }";
    html += "  .countdown { font-size: 18px; }";
    html += "  .jump-btn { padding: 12px 24px; font-size: 14px; }";
    html += "}";
    html += "</style>";
    html += "<script>";
    html += "let countdown = 3;";
    html += "function updateCountdown() {";
    html += "  const element = document.getElementById('countdown');";
    html += "  if (countdown > 0) {";
    html += "    element.textContent = countdown + ' 秒后自动跳转到主页';";
    html += "    countdown--;";
    html += "    setTimeout(updateCountdown, 1000);";
    html += "  } else {";
    html += "    window.location.href = '/';";
    html += "  }";
    html += "}";
    html += "window.onload = function() { updateCountdown(); };";
    html += "</script>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<div class='icon'>✅</div>";
    html += "<h1>Wi-Fi 配置成功！</h1>";
    html += "<p class='message'>已成功连接到WiFi网络<br/>正在为您跳转到主页控制页面</p>";
    html += "<div class='countdown' id='countdown'>3 秒后自动跳转到主页</div>";
    html += "<a href='/' class='jump-btn'>立即跳转到主页</a>";
    html += "</div>";
    html += "</body></html>";
    return html;
  }

  // 生成错误页面 HTML
  String generateErrorPage(const String& title, const String& message, const String& backUrl, const String& backText) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>" + title + " - " + String(SYSTEM_TITLE) + "</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  background: linear-gradient(135deg, #ffa726 0%, #ff7043 100%); ";
    html += "  overflow: hidden; position: fixed; top: 0; left: 0; ";
    html += "  -webkit-overflow-scrolling: touch; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #fff8e1 0%, #ffffff 100%); ";
    html += "  border: 1px solid #ffcc02; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(255,112,67,0.2), 0 8px 16px rgba(255,112,67,0.1); ";
    html += "  padding: 40px; position: absolute; top: 50%; left: 20px; right: 20px; ";
    html += "  transform: translateY(-50%); text-align: center; }";
    html += "h1 { color: #e65100; margin-bottom: 20px; font-size: 28px; font-weight: 600; }";
    html += ".icon { font-size: 64px; margin-bottom: 20px; }";
    html += ".message { color: #666; font-size: 18px; margin-bottom: 30px; line-height: 1.5; }";
    html += ".back-btn { ";
    html += "  display: inline-block; padding: 15px 30px; background: linear-gradient(135deg, #ff7043 0%, #e64a19 100%); ";
    html += "  color: white; text-decoration: none; border-radius: 12px; font-size: 16px; font-weight: 600; ";
    html += "  transition: all 0.3s ease; box-shadow: 0 4px 12px rgba(255,112,67,0.3); }";
    html += ".back-btn:hover { transform: translateY(-2px); box-shadow: 0 8px 20px rgba(255,112,67,0.4); }";
    html += ".back-btn:active { transform: translateY(0); }";
    html += "@keyframes fadeIn { 0% { opacity: 0; transform: scale(0.9); } 100% { opacity: 1; transform: scale(1); } }";
    html += ".container { animation: fadeIn 0.5s ease-out; }";
    html += "@media (max-width: 480px) { ";
    html += "  .container { left: 15px; right: 15px; padding: 30px; }";
    html += "  h1 { font-size: 24px; }";
    html += "  .icon { font-size: 48px; }";
    html += "  .message { font-size: 16px; }";
    html += "  .back-btn { padding: 12px 24px; font-size: 14px; }";
    html += "}";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<div class='icon'>⚠️</div>";
    html += "<h1>" + title + "</h1>";
    html += "<p class='message'>" + message + "</p>";
    html += "<a href='" + backUrl + "' class='back-btn'>" + backText + "</a>";
    html += "</div>";
    html += "</body></html>";
    return html;
  }

  // 生成主页 HTML
  String generateMainPage() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>" + String(SYSTEM_TITLE) + "</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
    html += "  overflow-x: hidden; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #f8f9ff 0%, #ffffff 100%); ";
    html += "  border: 1px solid #e1e5e9; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(0,0,0,0.12), 0 8px 16px rgba(0,0,0,0.08); ";
    html += "  padding: 30px; margin: 20px; min-height: calc(100vh - 40px); }";
    html += "h1 { text-align: center; color: #333; margin-bottom: 10px; font-size: 28px; font-weight: 600; }";
    html += ".subtitle { text-align: center; color: #666; margin-bottom: 30px; font-size: 16px; }";
    html += ".section { margin-bottom: 30px; }";
    html += ".section h2 { color: #444; margin-bottom: 15px; font-size: 20px; font-weight: 600; ";
    html += "  border-bottom: 2px solid #667eea; padding-bottom: 8px; }";
    html += ".control-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-bottom: 20px; }";
    html += ".control-btn { ";
    html += "  display: block; padding: 15px 20px; text-decoration: none; color: white; ";
    html += "  border-radius: 12px; font-size: 16px; font-weight: 600; text-align: center; ";
    html += "  transition: all 0.3s ease; box-shadow: 0 4px 12px rgba(0,0,0,0.15); }";
    html += ".control-btn:hover { transform: translateY(-2px); box-shadow: 0 8px 20px rgba(0,0,0,0.25); }";
    html += ".control-btn:active { transform: translateY(0); }";
    html += ".btn-success { background: linear-gradient(135deg, #28a745 0%, #20c997 100%); }";
    html += ".btn-danger { background: linear-gradient(135deg, #dc3545 0%, #c82333 100%); }";
    html += ".btn-primary { background: linear-gradient(135deg, #007bff 0%, #0056b3 100%); }";
    html += ".btn-warning { background: linear-gradient(135deg, #ffc107 0%, #e0a800 100%); color: #212529; }";
    html += ".status-card { ";
    html += "  background: white; border: 1px solid #e1e5e9; border-radius: 12px; ";
    html += "  padding: 20px; box-shadow: 0 2px 8px rgba(0,0,0,0.05); margin-bottom: 15px; }";
    html += ".status-item { display: flex; justify-content: space-between; align-items: center; ";
    html += "  padding: 8px 0; border-bottom: 1px solid #f0f0f0; }";
    html += ".status-item:last-child { border-bottom: none; }";
    html += ".status-label { font-weight: 500; color: #555; }";
    html += ".status-value { color: #333; font-weight: 600; }";
    html += ".status-connected { color: #28a745; }";
    html += ".status-disconnected { color: #dc3545; }";
    html += ".single-btn { width: 100%; margin-bottom: 10px; }";
    html += "@media (max-width: 768px) { ";
    html += "  .container { margin: 15px; padding: 20px; min-height: calc(100vh - 30px); }";
    html += "  h1 { font-size: 24px; }";
    html += "  .control-grid { grid-template-columns: 1fr; gap: 12px; }";
    html += "  .control-btn { padding: 12px 16px; font-size: 14px; }";
    html += "  .section h2 { font-size: 18px; }";
    html += "}";
    html += "</style></head><body>";

    html += "<div class='container'>";
    html += "<h1>" + String(SYSTEM_TITLE) + "</h1>";
    html += "<p class='subtitle'>智能时钟控制中心</p>";

    // 时钟控制区域
    // html += "<div class='section'>";
    // html += "<h2>⏰ 时钟控制</h2>";
    // html += "<div class='control-grid'>";
    // html += "<a href='/led1/onall' class='control-btn btn-success'>🟢 开机</a>";
    // html += "<a href='/led1/offall' class='control-btn btn-danger'>🔴 关机</a>";
    // html += "</div>";

    // html += "<div class='control-grid'>";
    // html += "<a href='/led1/onall' class='control-btn btn-success'>中国时间</a>";
    // html += "<a href='/led1/offall' class='control-btn btn-danger'>美国时间</a>";
    // html += "</div>";

    // html += "<div class='control-grid'>";
    // html += "<a href='/led1/onall' class='control-btn btn-success'>英国时间</a>";
    // html += "<a href='/led1/offall' class='control-btn btn-danger'>日本时间</a>";
    // html += "</div>";

    // html += "<div class='control-grid'>";
    // html += "<a href='/led1/onall' class='control-btn btn-success'>显示分钟</a>";
    // html += "<a href='/led1/offall' class='control-btn btn-danger'>做个表情</a>";
    // html += "</div>";

    // html += "<div class='control-grid'>";
    // html += "<a href='/led1/onall' class='control-btn btn-success'>倒计时 1 分钟</a>";
    // html += "<a href='/led1/offall' class='control-btn btn-danger'>倒计时 5 分钟</a>";
    // html += "</div>";

    // html += "</div>";


    // 系统设置区域
    html += "<div class='section'>";
    html += "<h2>⚙️ 系统设置</h2>";
    html += "<a href='/config' class='control-btn btn-primary single-btn'>📶 配置 Wi-Fi</a>";
    html += "<a href='/reset' class='control-btn btn-warning single-btn'>🔄 恢复出厂设置</a>";
    html += "</div>";

    // 系统状态区域
    html += "<div class='section'>";
    html += "<h2>📊 系统状态</h2>";
    html += "<div class='status-card'>";
    if (wifiManager) {
      html += "<div class='status-item'>";
      html += "<span class='status-label'>WiFi 网络</span>";
      html += "<span class='status-value'>" + wifiManager->getStoredSSID() + "</span>";
      html += "</div>";

      html += "<div class='status-item'>";
      html += "<span class='status-label'>连接状态</span>";
      String status = wifiManager->getConnectionStatus();
      String statusClass = (WiFi.status() == WL_CONNECTED) ? "status-connected" : "status-disconnected";
      html += "<span class='status-value " + statusClass + "'>" + status + "</span>";
      html += "</div>";

      if (WiFi.status() == WL_CONNECTED) {
        html += "<div class='status-item'>";
        html += "<span class='status-label'>IP 地址</span>";
        html += "<span class='status-value'>" + WiFi.localIP().toString() + "</span>";
        html += "</div>";
      }
    }
    html += "</div>";
    html += "</div>";

    html += "</div>";
    html += "</body></html>";
    return html;
  }

  // 生成配置页面 HTML
  String generateConfigPage() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    html += "<title>Wi-Fi 配置</title>";
    html += "<style>";
    html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
    html += "html, body { ";
    html += "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; ";
    html += "  height: 100%; width: 100%; color: #333; margin: 0; padding: 0; ";
    html += "  overflow: hidden; position: fixed; top: 0; left: 0; ";
    html += "  -webkit-overflow-scrolling: touch; }";
    html += ".container { ";
    html += "  background: linear-gradient(135deg, #f8f9ff 0%, #ffffff 100%); ";
    html += "  border: 1px solid #e1e5e9; border-radius: 20px; ";
    html += "  box-shadow: 0 20px 40px rgba(0,0,0,0.12), 0 8px 16px rgba(0,0,0,0.08); ";
    html += "  padding: 30px; position: absolute; top: 50%; left: 20px; right: 20px; ";
    html += "  transform: translateY(-50%); max-height: calc(100vh - 40px); ";
    html += "  overflow-y: auto; }";
    html += "h1 { text-align: center; color: #333; margin-bottom: 10px; font-size: 24px; font-weight: 600; }";
    html += ".subtitle { text-align: center; color: #666; margin-bottom: 30px; font-size: 14px; }";
    html += ".form-group { margin-bottom: 20px; }";
    html += "label { display: block; margin-bottom: 8px; color: #555; font-weight: 500; font-size: 14px; }";
    html += "input[type=text], input[type=password] { ";
    html += "  width: 100%; padding: 15px; border: 2px solid #e1e5e9; border-radius: 12px; ";
    html += "  font-size: 16px; transition: all 0.3s ease; background: white; ";
    html += "  box-shadow: 0 2px 4px rgba(0,0,0,0.05); }";
    html += "input[type=text]:focus, input[type=password]:focus { ";
    html += "  outline: none; border-color: #667eea; background: white; ";
    html += "  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1), 0 2px 4px rgba(0,0,0,0.05); }";
    html += "select { ";
    html += "  width: 100%; padding: 15px; border: 2px solid #e1e5e9; border-radius: 12px; ";
    html += "  font-size: 16px; transition: all 0.3s ease; background: white; ";
    html += "  box-shadow: 0 2px 4px rgba(0,0,0,0.05); appearance: none; ";
    html += "  background-image: url('data:image/svg+xml;charset=US-ASCII,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 4 5\"><path fill=\"%23666\" d=\"M2 0L0 2h4zm0 5L0 3h4z\"/></svg>'); ";
    html += "  background-repeat: no-repeat; background-position: right 15px center; background-size: 12px; }";
    html += "select:focus { ";
    html += "  outline: none; border-color: #667eea; background-color: white; ";
    html += "  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1), 0 2px 4px rgba(0,0,0,0.05); }";
    html += "input[type=submit], .refresh-btn { ";
    html += "  width: 100%; padding: 16px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
    html += "  color: white; border: none; border-radius: 12px; font-size: 16px; font-weight: 600; ";
    html += "  cursor: pointer; transition: all 0.3s ease; margin-top: 10px; ";
    html += "  box-shadow: 0 4px 12px rgba(102, 126, 234, 0.3); }";
    html += "input[type=submit]:hover, .refresh-btn:hover { transform: translateY(-2px); ";
    html += "  box-shadow: 0 8px 20px rgba(102, 126, 234, 0.4); }";
    html += "input[type=submit]:active, .refresh-btn:active { transform: translateY(0); }";
    html += ".wifi-select-container { display: flex; gap: 12px; align-items: stretch; margin-bottom: 15px; }";
    html += ".wifi-select { flex: 1; }";
    html += ".refresh-btn-small { ";
    html += "  width: auto; padding: 15px 24px; margin: 0; min-width: 100px; ";
    html += "  background: linear-gradient(135deg, #28a745 0%, #20c997 100%); ";
    html += "  color: white; border: none; border-radius: 12px; font-size: 16px; font-weight: 600; ";
    html += "  cursor: pointer; transition: all 0.3s ease; ";
    html += "  box-shadow: 0 4px 12px rgba(40, 167, 69, 0.3); ";
    html += "  white-space: nowrap; }";
    html += ".refresh-btn-small:hover { transform: translateY(-2px); ";
    html += "  box-shadow: 0 8px 20px rgba(40, 167, 69, 0.4); }";
    html += ".loading { opacity: 0.6; pointer-events: none; }";
    html += ".back-link { text-align: center; margin-top: 20px; }";
    html += ".back-link a { color: #667eea; text-decoration: none; font-size: 14px; }";
    html += ".back-link a:hover { text-decoration: underline; }";
    html += "@media (max-width: 480px) { ";
    html += "  .container { ";
    html += "    left: 15px; right: 15px; padding: 25px; ";
    html += "    max-height: calc(100vh + 30px); ";
    html += "  } ";
    html += "  h1 { font-size: 22px; } ";
    html += "  input[type=text], input[type=password], select { padding: 12px; font-size: 16px; } ";
    html += "  input[type=submit], .refresh-btn { padding: 14px; } ";
    html += "  .wifi-select-container { gap: 8px; }";
    html += "  .refresh-btn-small { min-width: 80px; padding: 12px 16px; font-size: 14px; }";
    html += "}";
    html += "</style></head><body>";

    html += "<div class='container'>";

    // 检查是否是首次连接或连接失败
    if (wifiManager && wifiManager->isFirstConnection()) {
      html += "<h1>欢迎使用" + String(SYSTEM_TITLE) + "</h1>";
      html += "<p class='subtitle'>首次使用需要连接到您的 WiFi 网络</p>";
    } else if (WiFi.getMode() == WIFI_AP) {
      html += "<h1>Wi-Fi 连接失败</h1>";
      html += "<p class='subtitle'>请重新配置您的 WiFi 网络设置</p>";
    } else {
      html += "<h1>Wi-Fi 配置</h1>";
      html += "<p class='subtitle'>更新您的 WiFi 网络设置</p>";
    }

    html += "<form action='/save' method='POST'>";
    html += "<div class='form-group'>";
    html += "<label for='ssid'>WiFi 列表</label>";
    html += "<div class='wifi-select-container'>";
    html += "<div class='wifi-select'>";
    html += "<select id='wifiSelect' onchange='updateSSID(this.value)'>";
    html += "<option value=''>选择WiFi网络...</option>";
    html += "</select>";
    html += "</div>";
    html += "<button type='button' class='refresh-btn refresh-btn-small' onclick='refreshWiFiList()'>刷新</button>";
    html += "</div>";
    html += "<input type='text' id='ssid' name='ssid' value='" + (wifiManager ? wifiManager->getStoredSSID() : "") + "' required placeholder='请选择或输入 WiFi 名称'>";
    html += "</div>";
    html += "<div class='form-group'>";
    html += "<label for='password'>Wi-Fi 密码</label>";
    html += "<input type='password' id='password' name='password' value='" + (wifiManager ? wifiManager->getStoredPassword() : "") + "' required placeholder='请输入 WiFi 密码'>";
    html += "</div>";

    if (wifiManager && wifiManager->isFirstConnection()) {
      html += "<input type='submit' value='连接 WiFi'>";
    } else if (WiFi.getMode() == WIFI_AP) {
      html += "<input type='submit' value='重新连接 WiFi'>";
    } else {
      html += "<input type='submit' value='更新 WiFi'>";
    }
    html += "</form>";

    // 只有非首次连接且不在AP模式时才显示返回主页链接
    if (wifiManager && !wifiManager->isFirstConnection() && WiFi.getMode() != WIFI_AP) {
      html += "<div class='back-link'><a href='/'>← 返回主页</a></div>";
    }

    html += "</div>";

    // JavaScript 功能
    html += "<script>";
    html += "let wifiNetworks = [];";
    html += "function updateSSID(ssid) {";
    html += "  document.getElementById('ssid').value = ssid;";
    html += "}";
    html += "function refreshWiFiList() {";
    html += "  const refreshBtn = document.querySelector('.refresh-btn-small');";
    html += "  const wifiSelect = document.getElementById('wifiSelect');";
    html += "  refreshBtn.classList.add('loading');";
    html += "  refreshBtn.textContent = '扫描中...';";
    html += "  fetch('/scan')";
    html += "    .then(response => response.json())";
    html += "    .then(networks => {";
    html += "      wifiNetworks = networks;";
    html += "      updateWiFiSelect();";
    html += "      refreshBtn.classList.remove('loading');";
    html += "      refreshBtn.textContent = '刷新';";
    html += "    })";
    html += "    .catch(error => {";
    html += "      console.error('扫描WiFi失败:', error);";
    html += "      refreshBtn.classList.remove('loading');";
    html += "      refreshBtn.textContent = '刷新';";
    html += "    });";
    html += "}";
    html += "function updateWiFiSelect() {";
    html += "  const wifiSelect = document.getElementById('wifiSelect');";
    html += "  wifiSelect.innerHTML = '<option value=\"\">选择WiFi网络...</option>';";
    html += "  wifiNetworks.forEach(network => {";
    html += "    const option = document.createElement('option');";
    html += "    option.value = network.ssid;";
    html += "    const signalStrength = network.rssi > -50 ? '📶' : network.rssi > -70 ? '📶' : '📶';";
    html += "    const security = network.encryption > 0 ? '🔒' : '🔓';";
    html += "    option.textContent = `${network.ssid} ${signalStrength} ${security}`;";
    html += "    wifiSelect.appendChild(option);";
    html += "  });";
    html += "}";
    html += "// 页面加载时自动扫描WiFi";
    html += "window.onload = function() {";
    html += "  refreshWiFiList();";
    html += "};";
    html += "</script>";

    html += "</body></html>";
    return html;
  }
};

// 创建实例
ServoController servoController;
WiFiManager wifiManager(SYSTEM_TITLE, AP_PASSWD);
WebServerManager webServerManager;
SimpleTimeManager timeManager;
Audio audio;

// 全局语音命令变量
String currentVoiceCmd = "cn";  // 默认显示中国时间

// 表情限时显示管理：5秒后自动切回中国时间
const unsigned long EMOJI_DISPLAY_MS = 5000;
bool emojiTimedActive = false;
unsigned long emojiStartMillis = 0;

void setup() {
  Serial.begin(115200);

  // 初始化舵机控制器
  servoController.begin();

  // 初始化 WiFi 管理器
  wifiManager.begin();

  Serial2.begin(9600, SERIAL_8N1, VOICE_SERIAL_RX, VOICE_SERIAL_TX);  // 语音模块串口

  // 初始化音频（使用 I2S 输出）
  audio.setPinout(MP3_I2S_BCLK, MP3_I2S_LRC, MP3_I2S_DOUT);
  audio.setVolume(20);  // 0~21

  // 尝试连接已保存的 WiFi
  bool connected = false;
  if (wifiManager.hasStoredCredentials()) {
    connected = wifiManager.connectWithStoredCredentials();
  }

  // 如果没有连接成功，则开启 AP 模式
  if (!connected) {
    wifiManager.startAPMode();
  }

  // 设置 Web 服务器管理器的依赖
  webServerManager.setWiFiManager(&wifiManager);
  webServerManager.setServoController(&servoController);
  webServerManager.setAudioPlayer(&audio);

  // 启动 Web 服务器
  webServerManager.begin();
}

// 全部关闭
// servoController.clearDisplay();
// 第几位显示几
// servoController.displayDigit(1, 1);
// 显示时间 12:34
// servoController.displayTime(12, 34);
// 显示 8888
// servoController.displayNumber(8888);
// 显示 1234
// servoController.displayNumber(1234);

void loop() {
  // 处理音频播放（必须放在 loop 中不断调用）
  audio.loop();

  // 处理 DNS 请求（AP 模式下的强制门户）
  wifiManager.handleDNS();

  // 处理 Web 服务器请求
  webServerManager.handleClient();

  // 处理时间同步（只在WiFi连接时）
  timeManager.handleTimeSync();

  // 检查MP3显示超时
  bool shouldRestoreTimeDisplay = webServerManager.checkMP3DisplayTimeout();

  // 检查表情限时超时：超时后切回中国时间
  if (emojiTimedActive && (millis() - emojiStartMillis >= EMOJI_DISPLAY_MS)) {
    emojiTimedActive = false;
    currentVoiceCmd = "cn";
    Serial.println("[Emoji] 表情显示到时，切回中国时间");
    shouldRestoreTimeDisplay = true;
  }

  // 如果 UART2 中有可读的数据
  if (Serial2.available() > 0) {
    // 读取一行（假设语音模块发送的是可见字符并以换行或回车结尾）
    String voiceCmd = Serial2.readStringUntil('\n');
    voiceCmd.trim();  // 去掉可能的空格、换行

    Serial.print("Received from voice module: ");
    Serial.println(voiceCmd);

    if (voiceCmd == "hello") {
      Serial.println("Hello! Current voice command mode: " + currentVoiceCmd);
    } else if (voiceCmd == "cn") {
      currentVoiceCmd = "cn";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to China time mode");
    } else if (voiceCmd == "us") {
      currentVoiceCmd = "us";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to USA time mode");
    } else if (voiceCmd == "uk") {
      currentVoiceCmd = "uk";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to UK time mode");
    } else if (voiceCmd == "fz") {
      currentVoiceCmd = "fz";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to seconds display mode");
    } else if (voiceCmd == "xs") {
      currentVoiceCmd = "xs";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to hours display mode");
    } else if (voiceCmd == "close") {
      currentVoiceCmd = "close";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to close display mode");
    } else if (voiceCmd == "standby") {
      currentVoiceCmd = "standby";
      emojiTimedActive = false;  // 取消表情超时
      Serial.println("Switched to standby display mode");
    } else if (voiceCmd == "smile") {
      currentVoiceCmd = "smile";
      Serial.println("Switched to emoji: smile");
      emojiTimedActive = true; emojiStartMillis = millis();
    } else if (voiceCmd == "sad") {
      currentVoiceCmd = "sad";
      Serial.println("Switched to emoji: sad");
      emojiTimedActive = true; emojiStartMillis = millis();
    } else if (voiceCmd == "wink") {
      currentVoiceCmd = "wink";
      Serial.println("Switched to emoji: wink");
      emojiTimedActive = true; emojiStartMillis = millis();
    } else if (voiceCmd == "surprised") {
      currentVoiceCmd = "surprised";
      Serial.println("Switched to emoji: surprised");
      emojiTimedActive = true; emojiStartMillis = millis();
    } else if (voiceCmd == "neutral") {
      currentVoiceCmd = "neutral";
      Serial.println("Switched to emoji: neutral");
      emojiTimedActive = true; emojiStartMillis = millis();
    } else {
      // 未识别的指令
      Serial.println("Unknown command: " + voiceCmd);
      Serial.println("Available commands: hello, cn, us, uk, fz, xs, close, standby, smile, sad, wink, surprised, neutral");
    }
  }

  // 更新时钟显示（每秒更新一次，或者MP3显示超时后立即更新）
  static unsigned long lastClockUpdate = 0;
  bool forceUpdate = shouldRestoreTimeDisplay;
  bool normalUpdate = (millis() - lastClockUpdate > 1000) && !webServerManager.isMP3DisplayActive();

  if (forceUpdate || normalUpdate) {
    if (timeManager.isInitialized()) {
      // 根据当前语音命令显示不同内容
      if (currentVoiceCmd == "cn") {
        // 中国时间：显示分钟和秒数
        int year, hour, minute, second;
        if (timeManager.getChinaFullTime(&year, &hour, &minute, &second)) {
          servoController.displayTime(hour, minute);
          Serial.printf("[Clock-CN] %04d年 %02d:%02d:%02d (显示分钟:秒数 %02d:%02d)\n",
                        year, hour, minute, second, minute, second);
        }
      } else if (currentVoiceCmd == "us") {
        // 美国时间：显示小时和分钟
        String usTime = timeManager.getUSATime();
        if (usTime != "--:--") {
          // 解析美国时间字符串 "HH:MM"
          int colonPos = usTime.indexOf(':');
          if (colonPos > 0) {
            int usHour = usTime.substring(0, colonPos).toInt();
            int usMinute = usTime.substring(colonPos + 1).toInt();
            servoController.displayTime(usHour, usMinute);
            Serial.printf("[Clock-US] 美国时间 (显示小时:分钟 %02d:%02d)\n", usHour, usMinute);
          }
        }
      } else if (currentVoiceCmd == "uk") {
        // 英国时间：显示小时和分钟
        String ukTime = timeManager.getUKTime();
        if (ukTime != "--:--") {
          // 解析英国时间字符串 "HH:MM"
          int colonPos = ukTime.indexOf(':');
          if (colonPos > 0) {
            int ukHour = ukTime.substring(0, colonPos).toInt();
            int ukMinute = ukTime.substring(colonPos + 1).toInt();
            servoController.displayTime(ukHour, ukMinute);
            Serial.printf("[Clock-UK] 英国时间 (显示小时:分钟 %02d:%02d)\n", ukHour, ukMinute);
          }
        }
      } else if (currentVoiceCmd == "fz") {
        // 分钟秒数模式：显示中国时间的分钟和秒数
        int year, hour, minute, second;
        if (timeManager.getChinaFullTime(&year, &hour, &minute, &second)) {
          servoController.displayTime(minute, second);
          Serial.printf("[Clock-FZ] 分钟秒数模式 %04d年 %02d:%02d:%02d (显示 %02d:%02d)\n",
                        year, hour, minute, second, minute, second);
        }
      } else if (currentVoiceCmd == "xs") {
        // 小时模式：显示中国时间的小时和分钟
        int year, hour, minute, second;
        if (timeManager.getChinaFullTime(&year, &hour, &minute, &second)) {
          servoController.displayTime(hour, minute);
          Serial.printf("[Clock-XS] 小时模式 %04d年 %02d:%02d:%02d (显示 %02d:%02d)\n",
                        year, hour, minute, second, hour, minute);
        }
      } else if (currentVoiceCmd == "close") {
        servoController.clearDisplay();
      } else if (currentVoiceCmd == "standby") {
        servoController.displayNumber(8888);
      } else if (currentVoiceCmd == "smile") {
        servoController.displayEmojiSmile();
      }
    } else {
      if (currentVoiceCmd == "close") {
        servoController.clearDisplay();
      } else if (currentVoiceCmd == "standby") {
        servoController.displayNumber(8888);
      } else if (currentVoiceCmd == "smile") {
        servoController.displayEmojiSmile();
      } 
    }
    lastClockUpdate = millis();
  }
}