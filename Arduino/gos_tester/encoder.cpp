#include <TouchButton.h>
#include "gos_tester.h"
#include "drawing.h"

// ENCODER SCREEN =====================================================

// HARDWARE SETTINGS

// For Arduino Mega2560:
// Int 5 is on Pin 18 (port D, bit 3); attach to channel A on the encoder
#define CH_A_INT 5
#define CH_A_PIN 18
#define CH_A_READ ((PIND >> 3) & 0x01)
// Int 4 is on Pin 19 (port D, bit 2); attach to channel B on the encoder
#define CH_B_INT 4
#define CH_B_PIN 19
#define CH_B_READ ((PIND >> 2) & 0x01)

volatile bool enc_a;
volatile bool enc_b;
volatile bool prev_enc_a;
volatile bool prev_enc_b;

/* Signed four-byte counter for measuring how many forward or backward ticks we've seen from the encoder
 * The initial value below is set at boot time. (After that, we want the most recent value retained.)
 */
volatile long counter = 0;

/* Signed four-byte floating point for measuring the rate of change of the counter.
 * This one is reset in the setup routine because calculating rate across long periods of time can be flakey.
 */
float rate;

/* History variables for calculating the rate of change of the counter
 */
unsigned long saved_time;
long saved_counter;

int
parse_encoder (void)
{
  if (prev_enc_a && prev_enc_b) {
    if (!enc_a && enc_b) return 1;
    if (enc_a && !enc_b) return -1;
  } else if (!prev_enc_a && prev_enc_b) {
    if (!enc_a && !enc_b) return 1;
    if (enc_a && enc_b) return -1;
  } else if (!prev_enc_a && !prev_enc_b) {
    if (enc_a && !enc_b) return 1;
    if (!enc_a && enc_b) return -1;
  } else if (prev_enc_a && !prev_enc_b) {
    if (enc_a && enc_b) return 1;
    if (!enc_a && !enc_b) return -1;
  }
}

void
interrupt_service_routine(void)
{
    enc_a = CH_A_READ;
    enc_b = CH_B_READ;
    counter += parse_encoder();
    prev_enc_a = enc_a;
    prev_enc_b = enc_b;
}

/* zero_enc
 *
 * Set the tick counter back to zero. Called when Reset button is pressed.
 * We call draw_enc_values() later to update the value on screen.
 */
void
zero_enc(void)
{
    counter = 0;
}

/* zero_rate
 *
 * Reset the rate to zero and re-initialize the history variables to current values
 */
void
zero_rate(void)
{
    rate = 0.0;
    saved_time = millis();
    saved_counter = counter;
}

/* update_rate
 *
 * Get an updated rate of change for the encoder.
 * The encoder's "counter" value is updated during interrupt service routines, so
 * this only needs to track the rate of change.
 */
void
update_rate(void)
{
    // Gather the data once to keep it consistent across the rest of this function
    unsigned long new_time = millis();
    long new_counter = counter;
    
    // Did enough time pass to bother looking for an updated counter?
    if (new_time - saved_time > 1000) {
        rate = (float)(new_counter - saved_counter) / (float)(new_time - saved_time) * 1000.0;
        saved_time = new_time;
        saved_counter = new_counter;
    }
}

/* draw_enc_values
 *
 * Normal processing every time through a loop_encoder() call.
 */
void 
draw_enc_values(bool drawMe)
{
    if (drawMe) {
	add_label(num_to_string(int(counter), 8),  160,120);
        add_label(num_to_string(int(rate),5), 160,150);
        add_label("/sec", 240,150);
    } else {
	remove_label("12345678", 160,120);
	remove_label("12345",    160,150);
        remove_label("/sec",     240,150);
    }
}

/* draw_enc_labels
 *
 * Either draw or erase the labels on the screen, depending on the boolean argument
 *    Ticks:
 *    Rate:
 */
void
draw_enc_labels(bool drawMe)
{
    if (drawMe) {
	add_label("Ticks:",    48,120);
	add_label("Rate: ",    48,150);
    } else {
	remove_label("Ticks:", 48,120);
	remove_label("Rate",   48,150);
    }
}

/* draw_encoder
 *
 * Setup routine called once when the user enters the Encoder Test screen
 */
void
draw_encoder() {
    // Don't zero counter here; better to have reading saved if we leave and come back to this screen
    // The rate and associated variables are reset, however, to avoid flakey rate calculations
    // if the timer or counter happens to have wrapped around while we were on other screens
    zero_rate();
    
    draw_title("Encoder Test");
    add_labeled_button("Reset   ", "to zero    ", 80,60,  BTN_RESET);
    draw_enc_labels(true);
    
    // Enable interrupt handling for encoder input state changes (high to low or vice-versa)
    pinMode(CH_A_PIN, INPUT_PULLUP);
    pinMode(CH_B_PIN, INPUT_PULLUP);
    attachInterrupt(CH_A_INT, interrupt_service_routine, CHANGE);
    attachInterrupt(CH_B_INT, interrupt_service_routine, CHANGE);
}

/* loop_encoder
 *
 * Loop routine called very frequently whenever the user is on the Encoder Test screen
 */
enum ScreenState
loop_encoder() {
    enum ScreenState nextState;
    short button;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_RESET:
        // Set the counter back to zero, often a handy thing to do while testing an encoder
        zero_enc();
        // If the counter is reset, we must also clear the history variables or we'll get an invalid rate
        zero_rate();
        /* Fall through to None Pressed code! (Don't add a break here) */
    case BTN_None_Pressed:
        update_rate();
	draw_enc_values(true);
        // If no button pressed, just return without changing the screen
	nextState = SCR_ENCODER;
        break;
    case BTN_BACK:
        // Remove the interrupt service routines from the encoder pins (don't track changes while on other screens)
        detachInterrupt(CH_A_INT);
        detachInterrupt(CH_B_INT);
	// Clean up the screen before exiting
        remove_labeled_button(BTN_RESET);
        draw_enc_labels(false);
        draw_enc_values(false);
        nextState = SCR_SENSORS;
        break;
    default:
        /* NOT REACHED */
        nextState = SCR_START;
    }
    return nextState;
}
