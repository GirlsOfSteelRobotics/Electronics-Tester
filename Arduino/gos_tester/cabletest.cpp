#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// CABLETEST SCREEN =====================================================

void
draw_cabletest() {
    draw_title("Cable Testing");
    add_label("Add cabletest screen here", 5,110);
}

enum ScreenState
loop_cabletest() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_CABLETEST;

    // Clean up the screen
    remove_label("Add cabletest screen here", 5,110);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_CABLESEL;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
