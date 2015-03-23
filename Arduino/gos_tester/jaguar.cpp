#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

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

// Special characters that make up the slider graphic: <===||===>
char left[]  = {BGF_BACK_SLD, '\0'};
char right[] = {BGF_FORW_SLD, '\0'};
char line[]  = {BGF_LINE_SLD, '\0'};
char divider[]={BGF_DVDR_SLD, '\0'};
char slider[]= {BGF_LINE_SLD, BGF_LINE_SLD, BGF_LINE_SLD, BGF_DVDR_SLD,
		BGF_LINE_SLD, BGF_LINE_SLD, BGF_LINE_SLD, '\0'};

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
    } else {
        // Measure the time until it wrapped, plus any after it wrapped
        return (0xFFFF - start + 1) + stop;
    }
}

void blinkLED(void)
{
    unsigned long now = millis();
    // If the LED is set to blink AND
    // enough time has elapsed to change the on/off state...
    if (jaguarLEDperiod != 0 && 
	timeElapsed(jaguarLEDtimer, now) >= jaguarLEDperiod) {
	// Reset the timer
	jaguarLEDtimer = now;
	// Toggle the on/off state of the blinking LED
	jaguarLEDblinkoff = !jaguarLEDblinkoff;
	// Whenever the timer fires, redraw the LED either on or off
	draw_circle(285,210, jaguarLEDblinkoff ? BLACK : jaguarLEDcolor);
    }
}

void setLEDcolor(int setting)
{
    // Solid LED at neutral, full forward, or full reverse
    // otherwise, blinking twice per second
    if (setting == 0 || setting == +6 || setting == -6) 
	jaguarLEDperiod = 0;
    else
	jaguarLEDperiod = 250;

    // Yellow LED for neutral, green for forward, red for reverse
    if (setting < 0)
	jaguarLEDcolor = RED;
    else if (setting > 0)
	jaguarLEDcolor = GREEN;
    else
	jaguarLEDcolor = YELLOW;

    draw_circle(285,210, jaguarLEDcolor);
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
    setLEDcolor(setting);
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
    // Blank out old setting marker
    add_big_label(line, settingToX(jaguarSetting), 109);
    // Reset it to zero
    jaguarSetting = 0;
    // Draw divider in new position
    add_big_label(divider, settingToX(jaguarSetting), 109);
    updatePWM(jaguarSetting);
}

void incrementSetting(void) {
    // Short-circuit out if we're already at max value
    if (jaguarSetting >= +6)
	return;
    // Blank out old setting marker
    add_big_label(line, settingToX(jaguarSetting), 109);
    jaguarSetting++;
    // Draw divider in new position
    add_big_label(divider, settingToX(jaguarSetting), 109);
    updatePWM(jaguarSetting);
}

void decrementSetting(void) {
    // Short-circuit out if we're already at min value
    if (jaguarSetting <= -6)
	return;
    // Blank out old setting marker
    add_big_label(line, settingToX(jaguarSetting), 109);
    jaguarSetting--;
    // Draw divider in new position
    add_big_label(divider, settingToX(jaguarSetting), 109);
    updatePWM(jaguarSetting);
}

void
draw_jaguar() {
    draw_title("Jaguar Test");
    add_labeled_button("Off",    133,58,  BTN_RESET);
    add_labeled_button("Cycle",  116,170, BTN_CYCLE);
    add_big_labeled_button(left,  10,106, BTN_DOWN);
    add_big_labeled_button(right,272,106, BTN_UP);
    add_big_label(slider, 48,109);

    // LED display
    add_small_label("LED:", 270,187);

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

enum ScreenState
loop_jaguar() {
    short button;
    enum ScreenState nextState;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch(button) {
    case BTN_None_Pressed:
	// If no button pressed, just return without changing the screen
	nextState = SCR_JAGUAR;
	break;
    case BTN_BACK:
	remove_labeled_button(BTN_RESET);
	remove_labeled_button(BTN_CYCLE);
	remove_labeled_button(BTN_UP);
	remove_labeled_button(BTN_DOWN);
	remove_big_label(slider, 48,109);

	remove_small_label("LED:", 270,187);

	jaguarSetting = 0;
	updatePWM(jaguarSetting);

	// Erase the LED
	draw_circle(285,210, BLACK);
	// Return immediately to avoid the blinkLED() function
	// which would re-draw the LED that we just erased
	return SCR_MOTOR;
    case BTN_RESET:
	zeroSetting();
	nextState = SCR_JAGUAR;
	break;
    case BTN_UP:
	incrementSetting();
	nextState = SCR_JAGUAR;
	break;
    case BTN_DOWN:
	decrementSetting();
	nextState = SCR_JAGUAR;
	break;
    case BTN_CYCLE:
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
	nextState = SCR_JAGUAR;
	break;
    }

    // Update the state of the LED every iteration 
    // because it may need to be blinking
    blinkLED();

    return nextState;
}
