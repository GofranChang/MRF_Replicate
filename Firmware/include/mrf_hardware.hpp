#ifndef __MRF_REPLICATE_H__
#define __MRF_REPLICATE_H__

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <BH1750.h>
#include <Wire.h>

namespace mrf_replicate {

class MRFHardware {
public:
  MRFHardware() {};

  ~MRFHardware() {};

public:
  bool init();

  inline Adafruit_SSD1306& display_ext() {
    return display_ext_;
  }

  inline U8G2_FOR_ADAFRUIT_GFX& u8g2_ext() {
    return u8g2_ext_;
  }

private:
  const uint8_t ext_screen_w_ = 128;
  const uint8_t ext_screen_h_ = 32;

  Adafruit_SSD1306 display_ext_;
  U8G2_FOR_ADAFRUIT_GFX u8g2_ext_;
  BH1750 light_meter_;
};

bool MRFHardware::init() {
  display_ext_ = Adafruit_SSD1306(ext_screen_w_, ext_screen_h_, &Wire, -1);

  light_meter_.begin();

  return true;
}

}

#endif // #ifdef __MRF_REPLICATE_H__