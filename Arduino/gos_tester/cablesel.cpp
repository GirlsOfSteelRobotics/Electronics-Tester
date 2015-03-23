#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// CABLESEL SCREEN =====================================================

void
draw_cablesel() {
    draw_title("Select cable");
    add_labeled_button("PWM    ", 
		       "F-M 3x1",  64,60,  BTN_CABLE1);
    add_labeled_button("PWM Y  ", 
		       "F-M&M  ",  64,120, BTN_CABLE2);
    add_labeled_button("Encoder", 
		       "5x1-3x2",  64,180, BTN_CABLE3);
    add_labeled_button("Accel. ", 
		       "F-F 4x1", 192,60,  BTN_CABLE4);
    add_labeled_button("Gyro   ", 
		       "F-F 3x2", 192,120, BTN_CABLE5);
}

enum ScreenState
loop_cablesel() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_CABLESEL;

    // Remove buttons added by this screen
    remove_labeled_button(BTN_CABLE1);
    remove_labeled_button(BTN_CABLE2);
    remove_labeled_button(BTN_CABLE3);
    remove_labeled_button(BTN_CABLE4);
    remove_labeled_button(BTN_CABLE5);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_MAINSCR;
    case BTN_CABLE1:
    case BTN_CABLE2:
    case BTN_CABLE3:
    case BTN_CABLE4:
    case BTN_CABLE5:
      return SCR_CABLETEST;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
