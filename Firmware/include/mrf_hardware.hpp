#pragma once

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <BH1750.h>
#include <TFMPlus.h>

#define SCREEN_WIDTH 128        // OLED _main_display width, in pixels
#define SCREEN_HEIGHT 128        // OLED _main_display height, in pixels
#define SCREEN_HEIGHT_EXT 32
#define OLED_RESET -1
#define BLACK 0
#define WHITE 1

#define RXD2 RX
#define TXD2 TX

#define LIDAR_OFFSET 10

#define DISTANCE_MIN 15
#define DISTANCE_MAX 12

#define FWVERSION "4.5"

namespace mrf {

enum struct UiMode {
  UI_MAIN,
  UI_CONFIG,
  UI_CALIB,
  UI_UNKNOWN,
};

struct FilmFormat {
  int id;
  String name;
  int sensor[22];
  int frame[22];
  int frame_fill[4];
};

FilmFormat film_formats[] = {
  {35, "PANO", {0, 50, 90, 130, 168, 206, 242, 278, 312, 346, 378, 410, 442, 474, 504, 534, 564, 594, 622, 650, 678, 710}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 99}, {37,100}},
  {645, "6x4.5", {0, 158, 184, 210, 234, 257, 280, 302, 324, 346, 368, 388, 408, 428, 448, 468, 486, 650}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 99}, {100,64}},
  {66, "6x6", {0, 164, 198, 230, 262, 292, 320, 348, 376, 402, 428, 454, 480, 650}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 99}, {100,86}},
  {67, "6x7", {0, 166, 208, 248, 286, 322, 356, 390, 422, 454, 484, 650}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 99}, {100,100}},
};

struct Lens {
  int id;
  String name;
  int sensor_reading[7];
  float distance[7];
  float apertures[9];
  int framelines[4];
  bool calibrated;
};

// Lens and film format definitions
Lens lenses[] = {
    {5063, "50/6.3", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 6.3, 8, 11, 16, 22, 32, 0, 0}, {0, 17, 128, 109}, false},
    {6563, "65/6.3", {330, 312, 299, 287, 276, 268, 261}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 6.3, 8, 11, 16, 22, 32, 0, 0}, {0, 17, 128, 109}, true},
    {7556, "75/5.6", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 5.6, 8, 11, 16, 22, 32, 45, 0}, {9, 24, 110, 95}, false},
    {9035, "90/3.5", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {3.5, 4, 5.6, 8, 11, 16, 22, 32, 0}, {18, 32, 92, 79}, false},
    {10035, "100/3.5", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {3.5, 4, 5.6, 8, 11, 16, 22, 32, 0}, {23, 36, 84, 71}, false},
    {10028, "100/2.8", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {2.8, 4, 5.6, 8, 11, 16, 22, 32, 0}, {23, 36, 84, 71}, false},
    {12747, "127/4.7", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {4.7, 5.6, 8, 11, 16, 22, 32, 45, 64}, {31, 43, 66, 57}, false},
    {15056, "150/5.6", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {5.6, 8, 11, 16, 22, 32, 45, 0, 0}, {36, 48, 56, 47}, false},
    {25005, "250/5.0", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {5, 8, 11, 16, 22, 32, 45, 0, 0}, {47, 57, 34, 29}, false}  
};

class MrfHardware {
public:
  static MrfHardware& instance() {
    static MrfHardware instance;
    return instance;
  }

private:
  MrfHardware() :
      _ui_mode(UiMode::UI_MAIN),
      _encoder(5, 6),
#ifndef WITHOUT_MAIN_DISPLAY
      _main_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000),
#endif
      _ext_display(SCREEN_WIDTH, SCREEN_HEIGHT_EXT, &Wire, OLED_RESET),
      _prev_encoder_value(0),
      _encoder_value(0),
      _film_counter(10),
      _last_activity_time(0),
      _lidar_serial(2) {
  }

  ~MrfHardware() = default;

public:
  void begin() {
    delay(100);

#ifndef WITHOUT_MAIN_DISPLAY
    _main_display.begin(0x3D, true);
    _main_display.oled_command(0xC8);
    _main_display.setRotation(3);
    _main_display_u8g2.begin(_main_display);
    _main_display.clearDisplay();
    _main_display.display();
    delay(1000);
#endif

#ifndef WITHOUT_EXT_DISPLAY
    _ext_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    _ext_display.clearDisplay();
    _ext_display.setTextSize(2); // Draw 2X-scale text
    _ext_display.setTextColor(SSD1306_WHITE);
    _ext_display.setCursor(20, 10);
    _ext_display.print(F("MRF "));
    _ext_display.println(FWVERSION);
    _ext_display.display();
    delay(1000);
    _ext_display_u8g2.begin(_ext_display);
    _ext_display.clearDisplay();
    _ext_display.display();
    delay(1000);
#endif

#ifndef WITHOUT_LIDAR
    _lidar_serial.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(20);
    _tfminiplus.begin(&_lidar_serial);
    delay(1000);
#endif

#ifndef WITHOUT_MPU6050
    _mpu.begin();
    delay(1000);
#endif

#ifndef WITHOUT_BH1750
    _light_meter.begin();
#endif

    _last_activity_time = millis();
  }

  void loop() {
    if (UiMode::UI_MAIN == _ui_mode) {
      set_distance();
#ifndef WITHOUT_MAIN_DISPLAY
      draw_main_ui();
#endif
    }

    draw_external_ui();
  }

  inline long get_encoder_pos() {
    return _encoder.read();
  }

  void set_film_counter() {
    int encoder_position = -_encoder.read();
    if (encoder_position != _prev_encoder_value && encoder_position > _prev_encoder_value) {
      _encoder_value = encoder_position;
      _prev_encoder_value = _encoder_value;

      for (int i = 0; i < sizeof(film_formats[_selected_format].sensor) / sizeof(film_formats[_selected_format].sensor[0]); i++) {
        if (film_formats[_selected_format].sensor[i] == _encoder_value) {
          _film_counter = film_formats[_selected_format].frame[i];
          _frame_progress = 0;
        } else if (film_formats[_selected_format].sensor[i] < _encoder_value && _encoder_value < film_formats[_selected_format].sensor[i + 1]) {
          // Check if the encoder value is within +/- 2 of the next frame
          if (abs(_encoder_value - film_formats[_selected_format].sensor[i + 1]) <= 1) {
            // Snap to the next frame
            _film_counter = film_formats[_selected_format].frame[i + 1];
            _frame_progress = 0;
          } else {
            _film_counter = film_formats[_selected_format].frame[i];
            _frame_progress = static_cast<float>(_encoder_value - film_formats[_selected_format].sensor[i]) / (film_formats[_selected_format].sensor[i + 1] - film_formats[_selected_format].sensor[i]);
          }    
        }
      }

      _last_activity_time = millis();

      if (_sleep_mode == true) {
        _sleep_mode = false;
      }

      // TODO: Save profers  
      // savePrefs();
    }
  }

  void set_light_meter() {
    _lux = _light_meter.readLightLevel();

    if (_lux != _prev_lux) {
      _prev_lux = _lux;
    if (_lux <= 0) {
      _shutter_speed = "Dark!";
    } else {
      if (_aperture == 0) {
        cycle_apertures("up");
      }

      float speed = round(((_aperture * _aperture) * K) / (_lux * _iso) * 1000.0) / 1000.0;

      struct SpeedRange {
        float lower;
        float upper;
        const char *print_speed_range;
      };

      SpeedRange speed_ranges[] = {
          {0.001, 0.002, "1/1000"},
          {0.002, 0.004, "1/500"},
          {0.004, 0.008, "1/250"},
          {0.008, 0.016, "1/125"},
          {0.016, 0.033, "1/60"},
          {0.033, 0.066, "1/30"},
          {0.066, 0.125, "1/15"},
          {0.125, 0.250, "1/8"},
          {0.250, 0.500, "1/4"},
          {0.500, 1, "1/2"}};

      char print_speed[10];
      dtostrf(speed, 4, 1, print_speed);

      for (int i = 0; i < sizeof(speed_ranges) / sizeof(speed_ranges[0]); i++)
      {
        if (speed_ranges[i].lower <= speed && speed < speed_ranges[i].upper)
        {
          strcpy(print_speed, speed_ranges[i].print_speed_range);
          break;
        }
      }

     
      if (speed >= 1) {
        char print_speed_raw[10];
        dtostrf(speed, 4, 2, print_speed_raw);
        _shutter_speed = strcat(print_speed_raw, " sec.");
      } else {
        _shutter_speed = strcat(print_speed, " sec.");
      }
       
    }
  }
}

private:
  int_fast16_t get_focus_radius() {
    int minRadius = 4;
    int maxRadius = 30;

    int radius = min(maxRadius, max(minRadius, abs(_distance - _lens_distance_raw)));

    return radius;
  }

  String cm_to_readable(int cm) {
    if (cm < 100) {
      return String(cm) + "cm";
    } else {
      return String(float(cm) / 100, 1) + "m";
    }
  }

  void set_distance() {
    if (_tfminiplus.getData(_distance)) { // Get data from Lidar
      Serial.println(_distance);
      _distance = _distance + LIDAR_OFFSET;
      if (_distance != _prev_distance) {
        if (_distance <= LIDAR_OFFSET) {
          _distance_cm = "> " + String(DISTANCE_MAX) + "m";
        } else if (_distance < DISTANCE_MIN) {
          _distance_cm = "< " + String(DISTANCE_MIN) + "cm";
        } else {
          _distance_cm = cm_to_readable(_distance);
        }
        _prev_distance = _distance;
      }
    } else {
      _distance_cm = "...";
    }
  }

#ifndef WITHOUT_MAIN_DISPLAY
  void draw_main_ui() {
    _main_display.clearDisplay();

    _main_display_u8g2.setFontMode(1);
    _main_display_u8g2.setFontDirection(0);
    _main_display_u8g2.setForegroundColor(BLACK);
    _main_display_u8g2.setBackgroundColor(WHITE);
    _main_display_u8g2.setFont(u8g2_font_4x6_mf);
    _main_display.fillRect(0, 0, 128, 15, WHITE);
    _main_display.drawLine(64, 0, 64, 128, BLACK);
    _main_display_u8g2.setCursor(2, 7);
    _main_display_u8g2.print(F("ISO"));
    _main_display_u8g2.print(_iso);
    _main_display_u8g2.setCursor(46, 7);
    _main_display_u8g2.print(F("f"));
    if (_aperture == static_cast<int>(_aperture)) {
      _main_display_u8g2.print(static_cast<int>(_aperture));
    } else {
      _main_display_u8g2.print(_aperture, 1);
    }
    _main_display_u8g2.setCursor(2, 14);
    _main_display_u8g2.print(_shutter_speed);
    _main_display_u8g2.setCursor(68, 7);
    _main_display_u8g2.print(F("Dist:"));
    _main_display_u8g2.print(_distance_cm);
    _main_display_u8g2.setCursor(68, 14);
    _main_display_u8g2.print(F("Lens:"));
    _main_display_u8g2.print(_lens_distance_cm);

    //ReticlePosition reticlePosition = calculateReticlePosition(distance);

    _main_display.fillRect(
        lenses[_selected_lens].framelines[0], 
        lenses[_selected_lens].framelines[1], 
        lenses[_selected_lens].framelines[2],
        lenses[_selected_lens].framelines[3], 
        WHITE
    );

    int new_width = int(lenses[_selected_lens].framelines[2] * film_formats[_selected_format].frame_fill[1] / 100);
    int new_height =  int(lenses[_selected_lens].framelines[3] * film_formats[_selected_format].frame_fill[0] / 100);
    int width_offset = lenses[_selected_lens].framelines[0] + (lenses[_selected_lens].framelines[2] - new_width) / 2;
    int heigh_offset = lenses[_selected_lens].framelines[1] + (lenses[_selected_lens].framelines[3] - new_height) / 2;
  
    _main_display.fillRect(
        width_offset,
        heigh_offset,
        new_width,
        new_height, 
        BLACK
    );
    _main_display.drawRect(
        width_offset,
        heigh_offset,
        new_width,
        new_height,
        WHITE
    );

    // Calculate the center of the rectangle
    int rectCenterX = lenses[_selected_lens].framelines[0] + lenses[_selected_lens].framelines[2] / 2;
    int rectCenterY = lenses[_selected_lens].framelines[1] + lenses[_selected_lens].framelines[3] / 2 - 5;
 
    // Draw a circle at the center of the rectangle
    _main_display.fillCircle(rectCenterX, rectCenterY, 4, WHITE);
    _main_display.drawCircle(rectCenterX, rectCenterY, get_focus_radius(), WHITE);

#ifndef WITHOUT_MPU6050
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp);
    float x = a.acceleration.x;
    float y = a.acceleration.y;
    float z = a.acceleration.z;
#else
    float x = -99.f;
    float y = -99.f;
    float z = -99.f;
#endif

    // Convert accelerometer readings into angles
    float pitch_scale = 15;
    float roll_scale = 0.5;
    float pitch = atan2(x, sqrt(x*x + z*z));
    float roll = atan2(y, sqrt(x*x + z*z));

    // Define the deadzone
    float deadzone = 0.03;

    // Apply the deadzone to the pitch and roll
    if (abs(pitch) < deadzone) {
      pitch = 0;
    }
    if (abs(roll) < deadzone) {
      roll = 0;
    }

    pitch = pitch * pitch_scale;
    roll = roll * roll_scale;

    // Define the length of the line
    float length = SCREEN_WIDTH - 10;

    // Calculate the start and end points of the line
    float startX = rectCenterX - length/2 * cos(roll);
    float startY =  rectCenterY - length/2 * sin(roll) + pitch;
    float endX = rectCenterX + length/2 * cos(roll);
    float endY =  rectCenterY + length/2 * sin(roll) + pitch;

    // Draw the line on the display
    _main_display.drawLine(startX, startY - 1, endX, endY - 1, WHITE);  
    _main_display.drawLine(startX, startY, endX, endY, WHITE);  
    _main_display.drawLine(startX, startY + 1, endX, endY + 1, WHITE);  

    // Define the length of the vertical line
    int vertLineLength = 30;

    // Calculate the start and end points of the vertical line
    int vertLineStartY = rectCenterY - vertLineLength / 2;
    int vertLineEndY = rectCenterY + vertLineLength / 2;

    // Draw the vertical line on the display
    _main_display.drawLine(rectCenterX, vertLineStartY, rectCenterX, vertLineEndY, WHITE);

    _main_display.display();
  }
#endif

#ifndef WITHOUT_EXT_DISPLAY
  void draw_external_ui() {
    int progessBarWidth = 90;
    int progressBarHeight = 17;
    int progressBarX = 34;
    int progressBarY = 15;

    _ext_display.clearDisplay();

    _ext_display_u8g2.setFontMode(1);
    _ext_display_u8g2.setFontDirection(0);
    _ext_display_u8g2.setForegroundColor(BLACK);
    _ext_display_u8g2.setBackgroundColor(WHITE);
    _ext_display_u8g2.setFont(u8g2_font_6x10_mf);

    _ext_display_u8g2.setCursor(2, 8);
    _ext_display.fillRect(0, 0, 128, 10, WHITE);
    _ext_display_u8g2.print(film_formats[_selected_format].name);

    _ext_display.drawLine(33, 0, 33, 10, BLACK);

    _ext_display_u8g2.setCursor(37, 8);
    _ext_display_u8g2.print(lenses[_selected_lens].name);

    if (_bat_per == 100) {
      _ext_display.drawLine(100, 0, 100, 10, BLACK);
      _ext_display_u8g2.setCursor(104, 8);
    } else if (_bat_per < 10) {
      _ext_display.drawLine(111, 0, 111, 10, BLACK);
      _ext_display_u8g2.setCursor(115, 8);
    } else {
      _ext_display.drawLine(103, 0, 103, 10, BLACK);
      _ext_display_u8g2.setCursor(107, 8);
    }

    _ext_display_u8g2.print(_bat_per);
    _ext_display_u8g2.print(F("%"));

    _ext_display_u8g2.setCursor(8, 30);

    if (_frame_progress > 0) {
      float progressPercentage = _frame_progress * 100;
      int progressWidth = progessBarWidth * (progressPercentage / 100);

      _ext_display.drawRect(progressBarX, progressBarY, progessBarWidth, progressBarHeight, WHITE);
      _ext_display.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, WHITE);

      if (_frame_progress != _prev_frame_progress) {
        if (progressPercentage > 0 && progressPercentage < 100) {
          // Transition from red to green
          int greenValue = _frame_progress * 255;
          int redValue = (1 - _frame_progress) * 255;
          // Set the LED color using greenValue and redValue
          // sspixel.setPixelColor(0, sspixel.Color(redValue, greenValue, 0));
        }
        _prev_frame_progress = _frame_progress;
      }
    } else {
      // sspixel.setPixelColor(0, sspixel.Color(0, 0, 255));
      _ext_display_u8g2.setCursor(60, 30);
    }

    _ext_display_u8g2.setForegroundColor(WHITE);
    _ext_display_u8g2.setBackgroundColor(BLACK);
    _ext_display_u8g2.setFont(u8g2_font_10x20_mf);
    if (_film_counter == 0 && _frame_progress == 0) {
      _ext_display_u8g2.setCursor(8, 30);
      _ext_display_u8g2.print(F(" Load film."));

      // sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
    } else if (_film_counter == 99) {
      _ext_display_u8g2.setCursor(8, 30);
      _ext_display_u8g2.print(F(" Roll end."));

      // sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
    } else {
      _ext_display_u8g2.print(_film_counter);
    }

    // sspixel.show();
    _ext_display.display();
  }
#endif

  void cycle_apertures(String direction) {
    if (direction == "up") {
      _aperture_index++;
      if (_aperture_index >= sizeof(lenses[_selected_lens].apertures) / sizeof(lenses[_selected_lens].apertures[0])) {
        _aperture_index = 0;
      }
      if (lenses[_selected_lens].apertures[_aperture_index] == 0) {
        cycle_apertures("up");
      }
    } else if (direction == "down") {
      _aperture_index--;
      if (_aperture_index < 0) {
        _aperture_index = sizeof(lenses[_selected_lens].apertures) / sizeof(lenses[_selected_lens].apertures[0]) - 1;
      }
      if (lenses[_selected_lens].apertures[_aperture_index] == 0) {
        cycle_apertures("down");
      }
    }
  }

private:
  UiMode _ui_mode;

  Encoder _encoder;

#ifndef WITHOUT_MAIN_DISPLAY
  Adafruit_SH1107 _main_display;
  U8G2_FOR_ADAFRUIT_GFX _main_display_u8g2;
#endif

#ifndef WITHOUT_EXT_DISPLAY
  Adafruit_SSD1306 _ext_display;
  U8G2_FOR_ADAFRUIT_GFX _ext_display_u8g2;
#endif

#ifndef WITHOUT_ENCODER
  long _encoder_value;
  long _prev_encoder_value;
#endif

#ifndef WITHOUT_LIDAR
  TFMPlus _tfminiplus;
  HardwareSerial _lidar_serial;
#endif

#ifndef WITHOUT_MPU6050
  Adafruit_MPU6050 _mpu;
#endif

#ifndef WITHOUT_BH1750
  BH1750 _light_meter;
#endif
  float _lux = 0.f;
  float _prev_lux = 0.f;
  String _shutter_speed = "...";
  int _aperture_index = 0;
  const int K = 20;

  int _film_counter;

  float _frame_progress = 0;
  float _prev_frame_progress = 0;

  unsigned long _last_activity_time;

  bool _sleep_mode = false;

  int _iso = 400;

  float _prev_aperture = -0.f;
  float _aperture = -0.f;

  int _selected_lens = 1;
  int _selected_format = 3;

  // LiDAR distance
  int _prev_distance = 0;
  int16_t _distance = 0;    // Distance to object in centimeters
  int16_t _strength = 0;    // Strength or quality of return signal
  int16_t _temperature = 0; // Internal temperature of Lidar sensor chip
  String _distance_cm = "...";

  // Lens distance
  int _prev_lens_sensor_reading = 0;
  int _lens_sensor_reading = 0;
  int _lens_distance_raw = 0;
  String _lens_distance_cm = "...";

  int _bat_per = 0;
};

}