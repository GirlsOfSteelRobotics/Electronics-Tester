#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// MOTOR SCREEN =====================================================

void
draw_motor() {
    draw_title("Motor controller");
    // Make all the labels the same width so the buttons turn out
    // to be the same size
    add_labeled_button("Jaguar", 80,60,  BTN_JAGUAR);
    add_labeled_button("Spike ", 80,120, BTN_SPIKE);
    add_labeled_button("Victor", 80,180, BTN_VICTOR);
}

enum ScreenState
loop_motor() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_MOTOR;

    // Remove buttons added by this screen
    remove_labeled_button(BTN_JAGUAR);
    remove_labeled_button(BTN_SPIKE);
    remove_labeled_button(BTN_VICTOR);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_MAINSCR;
    case BTN_JAGUAR:
      return SCR_JAGUAR;
    case BTN_SPIKE:
      return SCR_SPIKE;
    case BTN_VICTOR:
      return SCR_VICTOR;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
