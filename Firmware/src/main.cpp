#include "mrf_hardware.hpp"

using namespace mrf_replicate;

static MRFHardware g_mrf;

struct Lens
{
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

void drawExternalUI(Adafruit_SSD1306& display_ext, U8G2_FOR_ADAFRUIT_GFX& u8g2_ext)
{

  int progessBarWidth = 90;
  int progressBarHeight = 17;
  int progressBarX = 34;
  int progressBarY = 15;

  display_ext.clearDisplay();

  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(BLACK);
  u8g2_ext.setBackgroundColor(WHITE);
  u8g2_ext.setFont(u8g2_font_6x10_mf);

  u8g2_ext.setCursor(2, 8);
  display_ext.fillRect(0, 0, 128, 10, WHITE);
  u8g2_ext.print("png");

  display_ext.drawLine(33, 0, 33, 10, BLACK);

  u8g2_ext.setCursor(37, 8);
  u8g2_ext.print(lenses[0].name);

  int bat_per = 100;
  if (bat_per == 100)
  {
    display_ext.drawLine(100, 0, 100, 10, BLACK);
    u8g2_ext.setCursor(104, 8);
  }
  else if (bat_per < 10)
  {
    display_ext.drawLine(111, 0, 111, 10, BLACK);
    u8g2_ext.setCursor(115, 8);
  }
  else
  {
    display_ext.drawLine(103, 0, 103, 10, BLACK);
    u8g2_ext.setCursor(107, 8);
  }

  u8g2_ext.print(bat_per);
  u8g2_ext.print(F("%"));

  u8g2_ext.setCursor(8, 30);

  int frame_progress = 5;
  int prev_frame_progress = 15;
  if (frame_progress > 0)
  {
    float progressPercentage = frame_progress * 100;
    int progressWidth = progessBarWidth * (progressPercentage / 100);

    display_ext.drawRect(progressBarX, progressBarY, progessBarWidth, progressBarHeight, WHITE);
    display_ext.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, WHITE);

    if (frame_progress != prev_frame_progress) {
      if (progressPercentage > 0 && progressPercentage < 100) {
        // Transition from red to green
        int greenValue = frame_progress * 255;
        int redValue = (1 - frame_progress) * 255;
        // Set the LED color using greenValue and redValue
        // sspixel.setPixelColor(0, sspixel.Color(redValue, greenValue, 0));
      }
      prev_frame_progress = frame_progress;
    }
  }
  else
  {
    // sspixel.setPixelColor(0, sspixel.Color(0, 0, 255));
    u8g2_ext.setCursor(60, 30);
  }

  int film_counter = 10;
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf);
  if (film_counter == 0 && frame_progress == 0)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Load film."));

    // sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
  }
  else if (film_counter == 99)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Roll end."));

    // sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
  }
  else
  {
    u8g2_ext.print(film_counter);
  }

  // sspixel.show();
  display_ext.display();
}

void setup(void) {
  g_mrf.init();

  delay(3000); // Slight delay or the displays won't work

  Serial.begin(9600);
  delay(500);
  Serial.printf("Hello 0\r\n");

  g_mrf.display_ext().begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Boot up sceen
  g_mrf.display_ext().clearDisplay();
  g_mrf.display_ext().setTextSize(2); // Draw 2X-scale text
  g_mrf.display_ext().setTextColor(SSD1306_WHITE);
  g_mrf.display_ext().setCursor(20, 10);
  g_mrf.display_ext().print(F("MRF "));
  g_mrf.display_ext().println("4.5");
  g_mrf.display_ext().display();

  delay(1000);

  g_mrf.u8g2_ext().begin(g_mrf.display_ext());

  g_mrf.display_ext().clearDisplay();
  g_mrf.display_ext().display();

  delay(1000);
}

void loop(void) {
  drawExternalUI(g_mrf.display_ext(), g_mrf.u8g2_ext());
  delay(1000);  
}
