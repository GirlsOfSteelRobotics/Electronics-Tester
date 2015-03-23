#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// SPIKE SCREEN =====================================================

// Which pin will we drive?
int fwdPin = 44;
int revPin = 46;

void
draw_spike() {
    draw_title("Spike Test");
    // Make all the labels the same width so the buttons turn out
    // to be the same size
    add_labeled_button("Forward",  64,60,  BTN_FORWARD);
    add_labeled_button("Reverse", 192,60,  BTN_REVERSE);
    add_labeled_button("Off    ",  64,120, BTN_OFF);
    add_labeled_button("Both/  ", 
		       "Idle   ", 192,120, BTN_BOTH);
    add_label("M-", 67,183);
    add_label("M+", 67,205);
    add_small_label("LED:", 270,187);

    pinMode(fwdPin, OUTPUT);
    pinMode(revPin, OUTPUT);

    digitalWrite(fwdPin, 0);
    digitalWrite(revPin, 0);
    draw_circle(285,210, ORANGE);
}

enum ScreenState
loop_spike() {
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_None_Pressed:
	// If no button pressed, just return without changing the screen
	return SCR_SPIKE;
    case BTN_BACK:
	// Return to the off state
	digitalWrite(fwdPin, 0);
	digitalWrite(revPin, 0);
	// Clean up the screen
	remove_labeled_button(BTN_FORWARD);
	remove_labeled_button(BTN_REVERSE);
	remove_labeled_button(BTN_OFF);
	remove_labeled_button(BTN_BOTH);
	remove_label("M-", 67,183);
	remove_label("M+", 67,205);
	remove_label("12V", 131,183);
	remove_label("12V", 131,205);
	remove_small_label("LED:", 270,187);
	draw_circle(285,210, BLACK);
	return SCR_MOTOR;
    case BTN_FORWARD:
	digitalWrite(fwdPin, 1);
	digitalWrite(revPin, 0);
	add_label("Gnd", 131,183);
	add_label("12V", 131,205);
	draw_circle(285,210, GREEN);
	return SCR_SPIKE;
    case BTN_REVERSE:
	digitalWrite(fwdPin, 0);
	digitalWrite(revPin, 1);
	add_label("12V", 131,183);
	add_label("Gnd", 131,205);
	draw_circle(285,210, RED);
	return SCR_SPIKE;
    case BTN_OFF:
	digitalWrite(fwdPin, 0);
	digitalWrite(revPin, 0);
	add_label("Gnd", 131,183);
	add_label("Gnd", 131,205);
	draw_circle(285,210, ORANGE);
	return SCR_SPIKE;
    case BTN_BOTH:
	digitalWrite(fwdPin, 1);
	digitalWrite(revPin, 1);
	add_label("12V", 131,183);
	add_label("12V", 131,205);
	draw_circle(285,210, BLACK);
	return SCR_SPIKE;
    default:
      /* NOT REACHED */
      return SCR_START;
    }
}
