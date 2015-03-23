#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// START ============================================================

unsigned long startTime;

void
draw_start() {
    draw_title("Start Screen");

    // Save the boot time for calculating startup delays
    startTime = millis();  
}

enum ScreenState
loop_start() {
    // Has a second or so passed since we started displaying this screen?
    if (millis() - startTime < 750) {
	return SCR_START;
    } else {
	return SCR_MAINSCR;
    }
}
