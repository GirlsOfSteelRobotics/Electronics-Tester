#include <TouchButton.h>
#include <Wire.h>
#include "gos_tester.h"
#include "drawing.h"

// LIDAR SCREEN =====================================================

// The I2C 7-bit address of the LIDAR sensor from the data sheet
const static byte ADDR = 0x62;
// Control register number for requesting a new measurement
const static byte CTL_REG_NUM = 0x00;
// Values for control register to request a measurement
const static byte CTL_REG_VALUE = 0x04;
// Register number for the first distance data register
// Also set the high bit to enable the auto-increment feature,
// meaning the second read will obtain the following register
// High byte is in 0x0F, low byte is in 0x0F+1=0x10
const static byte DATA_REG_NUM = 0x0F & 0x80;

bool
write_register_lidar(byte regNum, byte value)
{
    // Send the register number we want to read
    Wire.beginTransmission(ADDR);
    if (Wire.write(regNum) != 1) {
	Serial.println("Failed to write register number!");
	return false;
    }
    if (Wire.write(value) != 1) {
	Serial.println("Failed to write register value!");
	return false;
    }
    if (Wire.endTransmission(ADDR) != 0) {
	Serial.println("Failed to end transmission!");
	return false;
    }
    return true;
}

bool
read_wide_register(byte regNum, int *value)
{
    // Maximum times to attempt a read before failing
    // Allow at least 20 ms for the measurement to complete with default settings
    const static byte MAX_READS = 20;
    int attempts = 0;

    // Send the register number we want to read
    Wire.beginTransmission(ADDR);
    if (Wire.write(regNum) != 1) {
	Serial.println("Failed to write register number 2!");
	return false;
    }
    if (Wire.endTransmission(ADDR) != 0) {
	Serial.println("Failed to end transmission 2!");
	return false;
    }
    // Now read two bytes
    Wire.requestFrom(ADDR, (byte) 2);
    while (Wire.available() < 2) {
	if (++attempts > MAX_READS) {
	    return false;
	}
	Serial.println("Delaying: none available");
	delay(2);
    }
    // Read most significant byte first, shifting it up to the high byte
    *value = Wire.read();
    *value = *value << 8;
    // Now read the least significant byte
    *value |= Wire.read();
    return true;
}

/*
 * Draw or remove the data labels
 */
void
label_units(bool drawMe)
{
    if (drawMe) {
	add_label("cm:", 64,120);
	add_label("in:", 64,180);
    } else {
	remove_label("cm:", 64,120);
	remove_label("in:", 64,180);
    }
}

/*
 * Draw or remove the "Sensor Not Found" message
 * If removing, clear the entire scren width so the
 * same function can also erase the numeric values.
 */
void
label_error(bool drawMe)
{
    if (drawMe) {
	add_label("LIDAR Sensor ", 64,120);
	add_label("Not Attached!", 64,180);
    } else {
	remove_label("LIDAR Sensor ", 64,120);
	remove_label("Not Attached!", 64,180);
    }
}

void
label_values(int cm, int in)
{
    add_label(num_to_string(cm, 13), 112,120);
    add_label(num_to_string(in, 13), 112,180);
}

/* updateScreen
 * Normal processing loop for this screen.
 * Request a measurement and display the results.
 */
void update_screen_lidar(void)
{
    // Measurement values are read in centimeters
    // but we'll also display them in inches
    int cm, in;

    if (! write_register_lidar(CTL_REG_NUM, CTL_REG_VALUE)) {
        // requesting a measurement failed, so diplay an error
        label_error(true);
    } else {
        if (read_wide_register(DATA_REG_NUM, &cm)) {
	    Serial.print("cm: ");
	    Serial.print(cm);
            in = cm / 2.54;
	    label_values(cm, in);
	} else {
	    // the read failed, so display an error
	    label_error(true);
	}
    }
}

void
draw_lidar() {
    draw_title("LIDAR Distance");
}

enum ScreenState
loop_lidar() {
    short button;
    enum ScreenState nextState;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_None_Pressed:
	update_screen_lidar();
	// If no button pressed, don't switch to a different screen
	nextState = SCR_LIDAR;
	break;
    case BTN_BACK:
	// Clean up the screen
        label_units(false);
        label_error(false);
	nextState = SCR_SENSORS;
	break;
    default:
	/* NOT REACHED */
	nextState = SCR_START;
    }

    return nextState;
}
