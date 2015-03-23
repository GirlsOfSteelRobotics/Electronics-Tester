// Touch screen button library with debouncing logic.
// Based on the TouchScreen library (with lots of modifications):
// (c) ladyada / adafruit
// Code under MIT License

#include <TFT.h>

class TouchButton
{
 public:
  // No constructors because this is a singleton class
  // Be sure to call init() in your setup() function!

  void  init(void);
  void  readPanel(uint16_t *x_raw, uint16_t *y_raw, uint16_t *z_raw);
  bool  isPressed(unsigned int *x, unsigned int *y);
  short buttonReleased(void);
  void  addButton(unsigned int topLeftX, unsigned int topLeftY,
		  unsigned int width, unsigned int height, 
		  short id, bool decorate);
  void  addGlobalButton(short id);
  void  removeButton(short id);
  void  removeAndClearButton(short id);
  void  removeAllButtons(void);
  void  dumpButtonList(char *message); // For debugging only

 private:
  static const unsigned int TB_X_RES = 240;
  static const unsigned int TB_Y_RES = 320;
  static const unsigned int TB_MINX = 140;
  static const unsigned int TB_MAXX = 900;
  static const unsigned int TB_MINY = 120;
  static const unsigned int TB_MAXY = 940;
  static const unsigned long TB_DEBOUNCE_DELAY = 250;

  uint8_t _yp, _ym, _xm, _xp;
  uint16_t _rxplate;
  int16_t _pressureThreshhold;

  typedef struct button {
    unsigned int topLeftX;
    unsigned int topLeftY;
    unsigned int height;
    unsigned int width;
    short id;
    bool decorate;
    button *next;
  };
  button *_buttonList;

  void initPins(uint8_t xp_pin, uint8_t yp_pin, 
		uint8_t xm_pin, uint8_t ym_pin,
		uint16_t rxplate);
  short locateButton(unsigned int x, unsigned int y);
  void  removeButtonFromList(short buttonId);
  button * getButtonById(short buttonId);
  void  drawButtonRect(short buttonId, unsigned int color);
  void  drawButtonFilledRect(short buttonId, unsigned int color);
  void  highlightButton(short buttonId);
  void  unhighlightButton(short buttonId);
};

extern TouchButton Tbutton;
