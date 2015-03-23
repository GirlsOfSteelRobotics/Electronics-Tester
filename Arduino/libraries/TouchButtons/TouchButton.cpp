// Touch screen button library with debouncing logic.
// Based on the TouchScreen library (with lots of modifications):
// (c) ladyada / adafruit
// Code under MIT License

#include "pins_arduino.h"
#include "wiring_private.h"
#include <avr/pgmspace.h>

#include "TouchButton.h"

void TouchButton::init(void)
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  initPins(57, A2, A1, 54, 300);
#else 
  initPins(17, A2, A1, 14, 300);
#endif 
}

/* Usage: initPins(XP, YP, XM, YM, resistance); 
   Where, XP = X plus, YP = Y plus, XM = X minus and YM = Y minus 
   For better pressure precision, we need to know the resistance
   between X+ and X-. Use any multimeter to read it.
   The 2.8" TFT Touch shield has 300 ohms across the X plate.
*/
void TouchButton::initPins(uint8_t xp_pin, uint8_t yp_pin, 
			   uint8_t xm_pin, uint8_t ym_pin, 
			   uint16_t rxplate)
{
  _yp = yp_pin;
  _xm = xm_pin;
  _ym = ym_pin;
  _xp = xp_pin;
  _rxplate = rxplate;
  _pressureThreshhold = 10;
  _buttonList = NULL;
}


void TouchButton::readPanel(uint16_t *x_raw, uint16_t *y_raw, uint16_t *z_raw)
{
  uint8_t xp_port = digitalPinToPort(_xp);
  uint8_t yp_port = digitalPinToPort(_yp);
  uint8_t xm_port = digitalPinToPort(_xm);
  uint8_t ym_port = digitalPinToPort(_ym);

  uint8_t xp_mask = digitalPinToBitMask(_xp);
  uint8_t yp_mask = digitalPinToBitMask(_yp);
  uint8_t xm_mask = digitalPinToBitMask(_xm);
  uint8_t ym_mask = digitalPinToBitMask(_ym);

  // Hi-Z Y+ and Y-
  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);
  //digitalWrite(_yp, LOW);
  //digitalWrite(_ym, LOW);
  *portOutputRegister(yp_port) &= ~yp_mask;
  *portOutputRegister(ym_port) &= ~ym_mask;
   
  // Set X+ to VCC and X- to ground
  pinMode(_xp, OUTPUT);
  pinMode(_xm, OUTPUT);
  //digitalWrite(_xp, HIGH);
  //digitalWrite(_xm, LOW);
  *portOutputRegister(xp_port) |= xp_mask;
  *portOutputRegister(xm_port) &= ~xm_mask;
   
  if (x_raw != NULL)
    *x_raw = (1023-analogRead(_yp));

  // Hi-Z X+ and X-
  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  //digitalWrite(_xp, LOW);
  *portOutputRegister(xp_port) &= ~xp_mask;
   
  // Set Y+ to VCC, Y- to ground
  pinMode(_yp, OUTPUT);
  pinMode(_ym, OUTPUT);
  //digitalWrite(_yp, HIGH);
  //digitalWrite(_ym, LOW);
  *portOutputRegister(yp_port) |= yp_mask;
  *portOutputRegister(ym_port) &= ~ym_mask;
  
  if (y_raw != NULL)
    *y_raw = (1023-analogRead(_xm));

  // Set X+ to ground
  pinMode(_xp, OUTPUT);
  //digitalWrite(_xp, LOW);
  *portOutputRegister(xp_port) &= ~xp_mask;
  
  // Set Y- to VCC
  //digitalWrite(_ym, HIGH); 
  *portOutputRegister(ym_port) |= ym_mask;
  
  // Hi-Z Y+ (and leave X- in Hi-Z)
  pinMode(_yp, INPUT);
  //digitalWrite(_yp, LOW);
  *portOutputRegister(yp_port) &= ~yp_mask;
  
  int z1 = analogRead(_xm); 
  int z2 = analogRead(_yp);
  uint16_t ztmp;

  if (_rxplate != 0) {
    float rtouch;
    rtouch = z2;
    rtouch /= z1;
    rtouch -= 1;
    rtouch *= *x_raw;
    rtouch *= _rxplate;
    rtouch /= 1024;
     
    ztmp = rtouch;
  } else {
    ztmp = (1023-(z2-z1));
  }
  if (ztmp <= _pressureThreshhold) {
    ztmp = 0;
  }
  if (z_raw != NULL)
    *z_raw = ztmp;
}


bool TouchButton::isPressed(unsigned int *x, unsigned int *y)
{
  // Save pressed or not pressed state between calls
  // so we can detect when it changes
  static bool pressed = false;
  static unsigned long debounce_time = 0;
  static unsigned int saved_x = 0;
  static unsigned int saved_y = 0;

  bool now_pressed;
  unsigned int x_raw, y_raw, z;
  unsigned long now = millis();

  readPanel(&x_raw, &y_raw, &z);
  if (z != 0) {
    saved_x = map(x_raw, TB_MINX, TB_MAXX, TB_X_RES-1, 0);
    saved_y = map(y_raw, TB_MINY, TB_MAXY, TB_Y_RES-1, 0);
    now_pressed = true;
  } else {
    now_pressed = false;
  }

  if (now > debounce_time) {
    if (pressed != now_pressed) {
      debounce_time = now + TB_DEBOUNCE_DELAY;
      pressed = now_pressed;
    }
  }
  *x = pressed ? saved_x : 0;
  *y = pressed ? saved_y : 0;
  return pressed;
}


short TouchButton::buttonReleased(void)
{
  // Save pressed or not pressed state between calls
  // so we can detect when it changes
  static bool pressed = false;
  static short buttonId = 0;
  
  bool now_pressed;
  unsigned int x, y;

  now_pressed = isPressed(&x, &y);

  // React to transition when someone STARTS pressing on the touchscreen
  if (! pressed && now_pressed) {
    // If we're inside a button, treat this as a press.
    // Otherwise, ignore the press for this iteration;
    // maybe they'll move into a button rectangle later on.
    buttonId = locateButton(x, y);
    if (buttonId != 0) {
      pressed = true;
      highlightButton(buttonId);
    }
#if 0
    Serial.print("Down button at ");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(" in button ID ");
    Serial.println(buttonId);
#endif
    // Always return 0 because they didn't release the button yet
    return 0;
  }

  // React to transition when someone STOPS pressing on the touchscreen
  if (pressed && ! now_pressed) {
    pressed = false;
    unhighlightButton(buttonId);
    // Return the button ID saved when we first detected a press that
    // was inside a button rectangle
    return buttonId;
  }

  // If the pressed/not pressed state hasn't changed, do nothing
  return 0;
}


void TouchButton::addButton(unsigned int topLeftX, unsigned int topLeftY,
			    unsigned int width, unsigned int height, 
			    short id, bool decorate)
{
  button *newButton = (button *) malloc(sizeof(button));
  // Bail out if allocation failed
  if (newButton == NULL)
    return;

  // Fill in values provided for the new button
  newButton->topLeftX = topLeftX;
  newButton->topLeftY = topLeftY;
  newButton->height = height;
  newButton->width = width;
  newButton->id = id;
  newButton->decorate = decorate;
  
  // Add the button to the beginning of the button list
  newButton->next = _buttonList;
  _buttonList = newButton;

  // Decorate the button if necessary
  unhighlightButton(id);
}


void TouchButton::addGlobalButton(short id)
{
  addButton(0, 0, TB_X_RES, TB_Y_RES, id, false);
}


void TouchButton::removeButton(short id)
{
  // Remove any button decorations
  drawButtonRect(id, BLACK);

  // Remove the button list data structure
  removeButtonFromList(id);
}


void TouchButton::removeAndClearButton(short id)
{
  // Remove any button decorations
  drawButtonFilledRect(id, BLACK);

  // Remove the button list data structure
  removeButtonFromList(id);
}


void TouchButton::removeButtonFromList(short id)
{
  button *prevButton, *thisButton;

  prevButton = NULL;
  thisButton = _buttonList;
  
  while(thisButton != NULL) {
    if (thisButton->id == id) {
      if (prevButton == NULL) {
	// First case: the very first button is to be deleted
	_buttonList = thisButton->next;
      } else {
	// Otherwise: deleting other than the first on the list
	prevButton->next = thisButton->next;
      }
      free(thisButton);
    }
    prevButton = thisButton;
    thisButton = thisButton->next;
 }
}


void TouchButton::removeAllButtons(void)
{
  button *thisButton, *nextButton;

  thisButton = _buttonList;
  while(thisButton != NULL) {
    nextButton = thisButton->next;
    free(thisButton);
    thisButton = nextButton;
  }
  _buttonList = NULL;
}


void TouchButton::dumpButtonList(char *message)
{
  button *thisButton = _buttonList;
  Serial.print("Dumping button list: ");
  Serial.println(message);
  while(thisButton != NULL) {
    Serial.print(thisButton->id);
    Serial.print(": (");
    Serial.print(thisButton->topLeftX);
    Serial.print(", ");
    Serial.print(thisButton->topLeftY);
    Serial.print(") ");
    Serial.print(thisButton->height);
    Serial.print("h by ");
    Serial.print(thisButton->width);
    Serial.print("w, decorate=");
    Serial.println(thisButton->decorate);
    thisButton = thisButton->next;
  }
}


short TouchButton::locateButton(unsigned int x, unsigned int y)
{
  button *thisButton;

  // Iterate through each button on the button list
  for(thisButton = _buttonList; 
      thisButton != NULL; 
      thisButton = thisButton->next) {
    // Is (x, y) within the button's rectangle?
    if (x >= thisButton->topLeftX && 
	x < (thisButton->topLeftX + thisButton->width) &&
	y >= thisButton->topLeftY && 
	y < (thisButton->topLeftY + thisButton->height)) {
      return thisButton->id;
    }
  }
  return 0;
}


/* Returns a pointer to the button object that has buttonId
 * or returns NULL if there are no buttons or no matching button.
 */
TouchButton::button *TouchButton::getButtonById(short buttonId)
{
  button *thisButton = _buttonList;
  while(thisButton != NULL) {
    if (thisButton->id == buttonId)
      break;
    thisButton = thisButton->next;
  }
  return thisButton;
}


void TouchButton::drawButtonRect(short buttonId, unsigned int color)
{
  button *btn = getButtonById(buttonId);
  // Did we find a matching button ID? Should it be decorated?
  if (btn != NULL && btn->decorate) {
    Tft.drawRectangle(btn->topLeftX, btn->topLeftY,
		      btn->width, btn->height, color);
    Tft.drawRectangle(btn->topLeftX+1, btn->topLeftY+1,
		      btn->width-2, btn->height-2, color);
  }
}


void TouchButton::drawButtonFilledRect(short buttonId, unsigned int color)
{
  button *btn = getButtonById(buttonId);
  // Did we find a matching button ID? Should it be decorated?
  if (btn != NULL && btn->decorate) {
    Tft.fillRectangle(btn->topLeftX, btn->topLeftY,
		      btn->width, btn->height, color);
  }
}


void TouchButton::highlightButton(short buttonId)
{
  drawButtonRect(buttonId, GREEN);
}


void TouchButton::unhighlightButton(short buttonId)
{
  drawButtonRect(buttonId, ORANGE);
}

TouchButton Tbutton = TouchButton();
