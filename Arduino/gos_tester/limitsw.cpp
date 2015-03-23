#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// LIMITSW SCREEN =====================================================

void
draw_limitsw() {
    draw_title("Limit Switch");
}

enum ScreenState
loop_limitsw() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_LIMITSW;

    // Clean up the screen
    remove_label("Add limitsw screen here", 5,110);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_SENSORS;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
