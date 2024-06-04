#ifndef __MRF_REPLICATE_H__
#define __MRF_REPLICATE_H__

#include <Adafruit_SSD1306.h>

class MRFHardware {
public:
  MRFHardware() {};

  ~MRFHardware() {};

private:
  const uint8_t ext_screen_w_ = 128;
  const uint8_t ext_screen_h_ = 32;

  Adafruit_SSD1306 display_ext_;
};

#endif // #ifdef __MRF_REPLICATE_H__