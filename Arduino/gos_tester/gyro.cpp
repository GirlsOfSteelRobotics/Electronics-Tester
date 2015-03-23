#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// GYRO SCREEN =====================================================

// Which pin will we read from?
int gyroPinRead = 8; //ADC8
int gyroPinMode = A8; //ADC8

/* NOTE:
 * We let the user choose between rate and temperature display,
 * expecting them to decide based on whether the tester is plugged
 * into the Rate or Temp connector on the gyroscope board.
 * (We went with a simple three pin connector for the
 * testing pigtail instead of a two-by-three connector.)
 */

// Are we displaying Rate or Temperature on the screen?
enum displayStates {
    RATE,
    TEMP,
};
enum displayStates displayState;

/* gyro_labels
 *
 * Draw the labels on the screen in one of two formats depending on
 * the user's choice of the display state. Either:
 *    Volts:
 *    Rate:
 * or
 *    Volts:
 *    Temp:
 */
void
gyro_labels(enum displayStates displayState, bool drawMe)
{
    char *label;
    if (displayState == RATE)
	label = "Rate:";
    else
	label = "Temp:";

    if (drawMe) {
	add_label("Volts:", 48,120);
	add_label(label,    48,150);
    } else {
	remove_label("Volts:", 48,120);
	remove_label(label,    48,150);
    }
}

/* gyro_values
 *
 * Normal processing every time through a loop_gyro() call.
 */
void 
gyro_values(enum displayStates displayState, bool drawMe)
{
    unsigned int value = analogRead(gyroPinRead);
    unsigned int volts = (((value * 50) >> 4) * 10) >> 6;

    if (drawMe) {
	Serial.print("Value: ");
	Serial.print(value);
	Serial.print("     Volts: ");
	Serial.println(volts);
	add_label(num_to_string(volts, 8), 160,120);
	delay(250);
    } else {
	remove_label("        ", 160,120);
    }

    if (displayState == RATE) {
	if (drawMe)
	    add_label   ("12.5 o/sec", 160,150);
	else
	    remove_label("12.5 o/sec", 160,150);
    } else {
	if (drawMe) {
	    add_label   ("12.5 oF", 160,150);
	    add_label   ("12.5 oC", 160,176);
	} else {
	    remove_label   ("12.5 oF", 160,150);
	    remove_label   ("12.5 oC", 160,176);
	}
    }
}

void
draw_gyro() {
    pinMode(gyroPinMode, INPUT);
    displayState = RATE;

    draw_title("Gyroscope Test");
    add_labeled_button("Display ", "Rate/Temp  ", 80,60,  BTN_RATETEMP);
    // Draw the labels for the initial display state
    gyro_labels(displayState, true);
}

enum ScreenState
loop_gyro() {
    short button;
    enum ScreenState nextState;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_None_Pressed:
	gyro_values(displayState, true);
	// If no button pressed, don't switch to a different screen
	nextState = SCR_GYRO;
	break;
    case BTN_RATETEMP:
	// Erase the values before changing display state
	gyro_values(displayState, false);
	if (displayState == RATE)
	    displayState = TEMP;
	else
	    displayState = RATE;
	// Update the labels
	gyro_labels(displayState, true);
	// Stay on this screen
	nextState = SCR_GYRO;
	break;
    case BTN_BACK:
	// Clean up the screen
	gyro_labels(displayState, false);
	gyro_values(displayState, false);
	remove_labeled_button(BTN_RATETEMP);
	// Back to the sensors screen
	nextState = SCR_SENSORS;
	break;
    default:
	/* NOT REACHED */
	nextState = SCR_START;
    }

    return nextState;
}
