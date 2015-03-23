#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// VICTOR SCREEN =====================================================

void
draw_victor() {
    draw_title("Victor test");
    add_small_label("Add victor screen here", 5,110);
}

enum ScreenState
loop_victor() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_VICTOR;

    // Clean up the screen
    remove_small_label("Add victor screen here", 5,110);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_MOTOR;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
