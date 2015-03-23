#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// HALL EFFECT SCREEN =====================================================

void
draw_halleffect() {
    draw_title("Hall Effect");
    add_label("Add halleffect screen here", 5,110);
}

enum ScreenState
loop_halleffect() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // If no button pressed, just return without changing the screen
    if (button == BTN_None_Pressed)
	return SCR_HALLEFFECT;

    // Clean up the screen
    remove_label("Add halleffect screen here", 5,110);

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_BACK:
      return SCR_SENSORS;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
