#include <stdint.h>
#include <TFT.h>
#include <TouchButton.h>
#include <Wire.h>

#include "gos_tester.h"
#include "drawing.h"

enum ScreenState prevScreen;
enum ScreenState currScreen;

// TOP-LEVEL FUNCTIONS ===============================================

void setup(void)
{
    Serial.begin(9600);
    Tft.init();
    Tbutton.init();
    Wire.begin();

    // Orient the text in the correct direction
    Tft.setTextDirection(DOWN2UP);

    // Select the small font for displaying the heading
    Tft.setTextFont(&simpleFont);
    // Put a permanent heading at the top
    add_small_label("Girls of Steel - FIRST FRC #3504", 64,2, WHITE, RED);

    // Select the large font for displaying all the rest of the text
    Tft.setTextFont(&bigFont);

    // Set up the screen states so the loop() function will draw
    // the starting screen
    prevScreen = SCR_NONE;
    currScreen = SCR_START;
}

void loop(void) 
{
    /* Farm out the work to the appropriate function based on the
     * currently selected screen:
     * - Draw the initial screen if we just switched to it
     * - Call the loop function for the screen
     * - Update the currently selected screen based on the return of
     *   the loop function (change screens if a button was pressed)
     * NOTE: all screen-specific logic goes in loop_*() function, not here
     */
    switch (currScreen) {
    case SCR_NONE:
	/* NOT REACHED */
	break;
    case SCR_START:
	if (currScreen != prevScreen) {
	    draw_start();
	    prevScreen = currScreen;
	}
	currScreen = loop_start();
	break;
    case SCR_MAINSCR:
	if (currScreen != prevScreen) {
	    draw_mainscr();
	    prevScreen = currScreen;
	}
	currScreen = loop_mainscr();
	break;
    case SCR_CABLESEL:
	if (currScreen != prevScreen) {
	    draw_cablesel();
	    prevScreen = currScreen;
	}
	currScreen = loop_cablesel();
	break;
    case SCR_CABLETEST:
	if (currScreen != prevScreen) {
	    draw_cabletest();
	    prevScreen = currScreen;
	}
	currScreen = loop_cabletest();
	break;
    case SCR_SENSORS:
	if (currScreen != prevScreen) {
	    draw_sensors();
	    prevScreen = currScreen;
	}
	currScreen = loop_sensors();
	break;
    case SCR_GYRO:
	if (currScreen != prevScreen) {
	    draw_gyro();
	    prevScreen = currScreen;
	}
	currScreen = loop_gyro();
	break;
    case SCR_ACCEL:
	if (currScreen != prevScreen) {
	    draw_accel();
	    prevScreen = currScreen;
	}
	currScreen = loop_accel();
	break;
    case SCR_LIMITSW:
	if (currScreen != prevScreen) {
	    draw_limitsw();
	    prevScreen = currScreen;
	}
	currScreen = loop_limitsw();
	break;
    case SCR_HALLEFFECT:
	if (currScreen != prevScreen) {
	    draw_halleffect();
	    prevScreen = currScreen;
	}
	currScreen = loop_halleffect();
	break;
    case SCR_ENCODER:
	if (currScreen != prevScreen) {
	    draw_encoder();
	    prevScreen = currScreen;
	}
	currScreen = loop_encoder();
	break;
    case SCR_MOTOR:
	if (currScreen != prevScreen) {
	    draw_motor();
	    prevScreen = currScreen;
	}
	currScreen = loop_motor();
	break;
    case SCR_JAGUAR:
	if (currScreen != prevScreen) {
	    draw_jaguar();
	    prevScreen = currScreen;
	}
	currScreen = loop_jaguar();
	break;
    case SCR_SPIKE:
	if (currScreen != prevScreen) {
	    draw_spike();
	    prevScreen = currScreen;
	}
	currScreen = loop_spike();
	break;
    case SCR_VICTOR:
	if (currScreen != prevScreen) {
	    draw_victor();
	    prevScreen = currScreen;
	}
	currScreen = loop_victor();
	break;
	/*
	  case SCR_:
	  if (currScreen != prevScreen) {
	  draw_();
	  prevScreen = currScreen;
	  }
	  currScreen = loop_();
	  break;
	*/
    }
}
