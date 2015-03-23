#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// MAIN SCREEN =====================================================

void
draw_mainscr() {
    remove_back_button(BTN_BACK);
    draw_title("Select test mode");
    // Make all the labels the same width so the buttons turn out
    // to be the same size
    add_labeled_button("Signal  ", "Cables     ", 80,60,  BTN_CABLES);
    add_labeled_button("Sensors/", "Encoders   ", 80,120, BTN_SENSORS);
    add_labeled_button("Motor   ", "Controllers", 80,180, BTN_MOTOR);
}

enum ScreenState
loop_mainscr() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_MAINSCR;

    // All the other buttons change the screen, so enable the back button
    // This is probably the only place in the code where we need to add it
    // since it just sticks around on the sub-screens
    add_back_button(BTN_BACK);

    // Also remove all the buttons added by this screen
    remove_labeled_button(BTN_CABLES);
    remove_labeled_button(BTN_SENSORS);
    remove_labeled_button(BTN_MOTOR);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_CABLES:
      return SCR_CABLESEL;
    case BTN_SENSORS:
      return SCR_SENSORS;
    case BTN_MOTOR:
      return SCR_MOTOR;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
