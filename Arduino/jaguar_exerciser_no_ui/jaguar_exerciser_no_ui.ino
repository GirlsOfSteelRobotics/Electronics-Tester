

// JAGUAR ============================================================

// Set the PWM frequency to be 122 Hz, or a period of 1/122 Hz = 8.192 ms
// The calculation is:
// 8 MHz (clock speed)/256 (prescaler setting 4)/256 counter steps = 122.07 Hz
unsigned char prescaler = 4;

// For full reverse, send 0.67 ms high pulse
// 0.67ms (from data sheet) * 256/8.192 ms full cycle = 21 counts
int minCount = 21;
// For neutral (stopped), send 1.5 ms high pulse
// 1.5 ms (from data sheet) * 256/8.192 ms full cycle = 47 counts
int midCount = 47;
// For full forward, send 2.33 ms high pulse
// 2.33 ms (from data sheet) * 256/8.192 ms full cycle = 73 counts
int maxCount = 73;

// Which pin will we drive?
int pwmPin = 44;

// Where is the Jaguar speed slider set to right now? -6..+6
int jaguarSetting = 0;

// State of the on-screen LED display
int jaguarLEDperiod;
int jaguarLEDcolor;
unsigned long jaguarLEDtimer;
bool jaguarLEDblinkoff = false;

unsigned long timeElapsed(unsigned long start, unsigned long stop)
{
  // Did the timer wrap around? (happens every 70 minutes)
  if (stop >= start) {
    return stop - start;
  } 
  else {
    // Measure the time until it wrapped, plus any after it wrapped
    return (0xFFFF - start + 1) + stop;
  }
}

/* Translate the jaguar setting value (-6..+6) to the corresponding
 * PWM output value and update the PWM
 */
void updatePWM(int setting)
{
  // This works because 4*max setting is 24,
  // nearly the same as maxCount-midCount=26
  int value = midCount + 4*setting;
  // Stretch the value up or down by 26-24=2 
  // to make sure we achieve top speed 
  // (non-blinking LED on the Jaguar)
  if (setting > 0)
    value += 2;
  else if (setting < 0)
    value -= 2;
  analogWrite(pwmPin, value);
}

/* Translate the jaguar setting value (-6..+6) to the
 * landscape X coordinate on the slider where the current
 * position should be marked.
 * The center position is used for jaguarSetting == 0.
 * Slider starts at X=48 and each slot is half of a 
 * double-sized character, 16 pixels
 */
int settingToX(int setting)
{
  int slot = setting + 6; // Convert -6..+6 to 0..12
  return 48 + slot*16;
}

void zeroSetting(void) {
  // Short-circuit out if we're already at zero
  if (jaguarSetting == 0)
    return;
  // Reset it to zero
  jaguarSetting = 0;
  updatePWM(jaguarSetting);
}

void incrementSetting(void) {
  // Short-circuit out if we're already at max value
  if (jaguarSetting >= +6)
    return;
  jaguarSetting++;
  updatePWM(jaguarSetting);
}

void decrementSetting(void) {
  // Short-circuit out if we're already at min value
  if (jaguarSetting <= -6)
    return;
  jaguarSetting--;
  updatePWM(jaguarSetting);
}

void
setup(void) 
{
  // Put the prescaler setting for Timer/Counter 5 in the bottom three bits
  // of the B control register, leaving the rest unchanged. This affects all
  // PWM pins controlled by T/C 5, digital pins 44, 45, and 46.
  TCCR5B = TCCR5B & 0b11111000 | prescaler;

  pinMode(pwmPin, OUTPUT);

  // Initialize the timer used to blink the LED on and off
  jaguarLEDtimer = millis();

  // Start us out in the neutral state (solid yellow LED)
  jaguarSetting = 0;
  updatePWM(jaguarSetting);
}

void
loop(void)
{
  zeroSetting();
  for (int i = 0; i <= +6; i++) {
    incrementSetting();
    delay(250);
  }
  delay(500);
  for (int i = +6; i >= -6; i--) {
    decrementSetting();
    delay(250);
  }
  delay(500);
  for (int i = -6; i < 0; i++) {
    incrementSetting();
    delay(250);
  }
}


