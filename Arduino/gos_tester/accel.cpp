#include <TouchButton.h>
#include <Wire.h>
#include "gos_tester.h"
#include "drawing.h"

// ACCEL SCREEN =====================================================

// The I2C 7-bit address of the accelerometer from the data sheet
const static byte ADDR = 0x1d;
// The ID code returned when we successfully read register 0x00
const static byte IDCODE = 0xe5;
// POWER_CTL register number for enabling/disabling measurements
const static byte POWER_CTL = 0x2d;
// Values for POWER_CTL to enable/disable measurement
const static byte POWER_CTL_ENABLE  = 0x08;
const static byte POWER_CTL_DISABLE = 0x00;
// Register number for the first acceleration data register
const static byte DATAX0 = 0x32;
// We'll do a 6-byte read starting from this register
const static byte DATA_CNT = 6;

enum accelStates {
    UNINIT,
    FOUND,
    NOTFOUND,
};

enum accelStates accelState;

bool
write_register(byte regNum, byte value)
{
    // Send the register number we want to read
    Wire.beginTransmission(ADDR);
    if (Wire.write(regNum) != 1) {
	//Serial.println("Failed to write register number!");
	return false;
    }
    if (Wire.write(value) != 1) {
	//Serial.println("Failed to write register value!");
	return false;
    }
    if (Wire.endTransmission(ADDR) != 0) {
	//Serial.println("Failed to end transmission!");
	return false;
    }
    return true;
}

bool
read_register(byte regNum, byte *value)
{
    // Maximum times to attempt a read before failing
    const static byte MAX_READS = 10;
    int attempts = 0;

    // Send the register number we want to read
    Wire.beginTransmission(ADDR);
    if (Wire.write(regNum) != 1) {
	//Serial.println("Failed to write register number!");
	return false;
    }
    if (Wire.endTransmission(ADDR) != 0) {
	//Serial.println("Failed to end transmission!");
	return false;
    }
    // Now read one byte
    Wire.requestFrom(ADDR, (byte) 1);
    while (Wire.available() < 1) {
	if (++attempts > MAX_READS) {
	    return false;
	}
	//Serial.println("Delaying: none available");
	delay(1);
    }
    *value = Wire.read();
    return true;
}

bool
standby_mode(bool enterStandby)
{
    bool wr_ok = write_register(POWER_CTL, enterStandby ? POWER_CTL_DISABLE : POWER_CTL_ENABLE);
    if (!wr_ok) {
	//Serial.print("Failed to set POWER_CTRL (standby mode) to: ");
	//Serial.println(enterStandby ? POWER_CTL_DISABLE : POWER_CTL_ENABLE);
    }
    return wr_ok;
}

bool
read_xyz(int *x, int *y, int *z)
{
    // Maximum times to attempt a read before failing
    const static byte MAX_READS = 25;
    int attempts = 0;
    byte value;

    // Send the register number for where we want to start reading
    Wire.beginTransmission(ADDR);
    if (Wire.write(DATAX0) != 1) {
	//Serial.println("Failed to write DATAX0 register number!");
	return false;
    }
    if (Wire.endTransmission(ADDR) != 0) {
	//Serial.println("Failed to end transmission!");
	return false;
    }
    // Now read six bytes
    Wire.requestFrom(ADDR, (byte) DATA_CNT);
    for (byte dataCtr = 0; dataCtr < DATA_CNT; dataCtr++) {
	while (Wire.available() < 1) {
	    if (++attempts > MAX_READS) {
		return false;
	    }
	    //Serial.println("Delaying: none available");
	    delay(1);
	}
	value = Wire.read();
	//Serial.println(value, HEX);
	// Put each of the six values into the appropriate byte 
	// of the return variables
	switch (dataCtr) {
	case 0:
	    *x = value;
	    break;
	case 1:
	    *x |= (value << 8);
	    break;
	case 2:
	    *y = value;
	    break;
	case 3:
	    *y |= (value << 8);
	    break;
	case 4:
	    *z = value;
	    break;
	case 5:
	    *z |= (value << 8);
	    break;
	}
    }
    return true;
}

bool
verify_id(void) {
    byte idCode;

    if (read_register(0x00, &idCode)) {
	//Serial.print("Read ID code: ");
	//Serial.println(idCode, HEX);
	if (idCode != IDCODE) {
	    //Serial.print("Unexpected ID code read, should be: ");
	    //Serial.println(IDCODE, HEX);
	    return false;
	}
    } else {
	//Serial.println("Failed to read ID code!");
	return false;
    }
    // If we get this far, the ID code was read and is correct
    return true;
}

/*
 * Draw or remove the X:, Y:, and Z: labels
 */
void
label_xyz(bool drawMe)
{
    if (drawMe) {
	add_label("X:", 64,60);
	add_label("Y:", 64,120);
	add_label("Z:", 64,180);
    } else {
	remove_label("X:", 64,60);
	remove_label("Y:", 64,120);
	remove_label("Z:", 64,180);
    }
}

/*
 * Draw or remove the "Accel Not Found" message
 * If removing, clear the entire scren width so the
 * same function can also erase the X, Y, Z numeric values.
 */
void
label_notfound(bool drawMe)
{
    if (drawMe) {
	add_label("             ", 64,60);
	add_label("Accelerometer", 64,120);
	add_label("Not Attached!", 64,180);
    } else {
	remove_label("             ", 64,60);
	remove_label("Accelerometer", 64,120);
	remove_label("Not Attached!", 64,180);
    }
}

void
label_values(int x, int y, int z)
{
    add_label(num_to_string(x, 13), 112,60);
    add_label(num_to_string(y, 13), 112,120);
    add_label(num_to_string(z, 13), 112,180);
}

/* updateScreen
 *
 * Normal processing every time through a loop_accel() call.
 * Implements a state diagram:
 * UNINIT- We just switched to this screen and haven't tried to 
 * read the accelerometer yet -> Attempt to read the device ID.
 * FOUND- The normal state where the last read was successful ->
 * Try to read x,y,z values. If ok, update the screen, otherwise
 * go back to UNINIT state.
 * NOTFOUND- The attempt to read the device ID failed, probably
 * because it's not plugged in -> Try to read it again. If ok, 
 * switch to the FOUND state.
 */
void update_screen(void)
{
    int x, y, z;

    if (accelState == FOUND) {
	if (read_xyz(&x, &y, &z)) {
	    //Serial.print("X, Y, Z: ");
	    //Serial.print(x);
            //Serial.print(" ");
	    //Serial.print(y);
            //Serial.print(" ");
	    //Serial.println(z);
	    label_values(x, y, z);
	} else {
	    // the read failed, so go back to UNINIT state
	    accelState = UNINIT;
	}
    } else { // NOTFOUND or UNINIT
	if (verify_id() && standby_mode(false)) {
	    accelState = FOUND;
	    label_notfound(false);
	    label_xyz(true);
	} else if (accelState == UNINIT) {
	    // The first attempt at reading the accel failed
	    accelState = NOTFOUND;
	    label_xyz(false);
	    label_notfound(true);
	}
    }
}

/*
 * Clean up the screen before returning to the previous menu
 */
void clear_screen(void)
{
    label_xyz(false);
    label_notfound(false);
}

void
draw_accel() {
    draw_title("Accelerometer");
    accelState = UNINIT;
}

enum ScreenState
loop_accel() {
    short button;
    enum ScreenState nextState;

    // See if a button has been pressed and released
    button = Tbutton.buttonReleased();

    // Jump to the appropriate screen depending on the button pressed
    switch (button) {
    case BTN_None_Pressed:
	update_screen();
	// If no button pressed, don't switch to a different screen
	nextState = SCR_ACCEL;
	break;
    case BTN_BACK:
	// Clean up the screen
	clear_screen();
	// Put the accelerometer device into standby mode
	standby_mode(true);
	nextState = SCR_SENSORS;
	break;
    default:
	/* NOT REACHED */
	nextState = SCR_START;
    }

    return nextState;
}
