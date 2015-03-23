#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// SENSORS SCREEN =====================================================

void
draw_sensors() {
    draw_title("Select sensor");
    // Make all the labels the same width so the buttons turn out
    // to be the same size
    add_labeled_button("Gyro   ", 
		       "scope  ",  64,60,  BTN_GYRO);
    add_labeled_button("Acceler", 
		       "ometer ",  64,120, BTN_ACCEL);
    add_labeled_button("Limit  ", 
		       "Switch ",  64,180, BTN_LIMITSW);
    add_labeled_button("Hall   ", 
		       "Effect ", 192,60,  BTN_HALLEFFECT);
    add_labeled_button("Encoder", 
		       "       ", 192,120, BTN_ENCODER);
}

enum ScreenState
loop_sensors() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_SENSORS;

    // Remove buttons added by this screen
    remove_labeled_button(BTN_GYRO);
    remove_labeled_button(BTN_ACCEL);
    remove_labeled_button(BTN_LIMITSW);
    remove_labeled_button(BTN_HALLEFFECT);
    remove_labeled_button(BTN_ENCODER);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_MAINSCR;
    case BTN_GYRO:
      return SCR_GYRO;
    case BTN_ACCEL:
      return SCR_ACCEL;
    case BTN_LIMITSW:
      return SCR_LIMITSW;
    case BTN_HALLEFFECT:
      return SCR_HALLEFFECT;
    case BTN_ENCODER:
      return SCR_ENCODER;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
