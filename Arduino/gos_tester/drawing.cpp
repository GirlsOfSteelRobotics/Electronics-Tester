#include <stdint.h>
#include <TFT.h>
#include <TouchButton.h>

#include "drawing.h"

void
draw_title(char *title) {
  unsigned short i, j;
  /* Make the title exactly as wide as the screen, truncating
   * characters or padding with spaces.
   *
   * Start at x=64 in landscape coordinates: the back button takes 50
   * pixels so round up to give it a little extra.  Each character is
   * 16 pixels wide, so max is 320-64/16=16 chars plus one for the
   * null terminator.
   */
  char paddedTitle[16+1];
  for (i = 0; i < (sizeof(paddedTitle)-1) && title[i]; i++) {
    paddedTitle[i] = title[i];
  }
  for (j = i; j < (sizeof(paddedTitle)-1); j++) {
    paddedTitle[j] = ' ';
  }
  paddedTitle[j] = '\0';

  Tft.drawString(paddedTitle, 12,319-64, 1, WHITE, BLACK);
}

/* Centralized function for putting the back button (<-) on the
 * upper left corner of the screen.
 * In landscape coordinates, the button is at (1,0) width=50, height=36
 * The text is at (10,7)
 * We avoid (0,0) button placement because the final row of pixels is 
 * partially obscured by the LCD's border, making lines look thin.
 */
void
add_back_button(short id) {
  char arrow[] = {BGF_BACK_SLD, BGF_LINE_SLD, 0x00};

  Tft.drawString(arrow, 7,319-10, 1, YELLOW, BLACK);
  Tbutton.addButton(0,319-50-1, 36,50, id, true);
}

/* Centralized function for removing the back button (<-) from the
 * screen. This is only useful when drawing the main screen.
 */
void
remove_back_button(short id) {
  Tbutton.removeAndClearButton(id);
}

void
add_labeled_button(char *label, unsigned int landX, unsigned int landY, short id) {
    add_labeled_button(label, NULL, landX, landY, id);
}

/* Add a label and button to the screen. The label is given as two lines.
 * NOTE: this routine uses coordinates in the landscape orientation,
 * not the normal TFT coordinates.
 */
void
add_labeled_button(char *label1, char *label2, unsigned int landX, unsigned int landY, short id) {
    int width;

    if (!label1)
	return;
    width = strlen(label1);
    if (label2 && strlen(label2) > width)
	width = strlen(label2);
    Tbutton.addButton(landY, 319-landX-(3+(16*width)+3-1), 3+22+22+3, 3+16*width+3, id, true);
    if (!label2) {
	// If only one label, center it in the button by adding half char height (11)
	Tft.drawString(label1, landY+3+11, 319-landX-3, 1, YELLOW, BLACK);
    } else {
	// For two labels, stack them one on top of the other
	Tft.drawString(label1, landY+3, 319-landX-3, 1, YELLOW, BLACK);
	Tft.drawString(label2, landY+3+22, 319-landX-3, 1, YELLOW, BLACK);
    }
}

/* Add a double-sized label and button to the screen. The label is given as one line.
 * NOTE: this routine uses coordinates in the landscape orientation,
 * not the normal TFT coordinates.
 */
void
add_big_labeled_button(char *label, unsigned int landX, unsigned int landY, short id) {
    int width;

    if (!label)
	return;
    width = strlen(label);
    Tbutton.addButton(landY, 319-landX-(3+(16*width*2)+3-1), 3+22+22+3, 3+16*width*2+3, id, true);
    // One label, center it in the button by adding half char height (11)
    Tft.drawString(label, landY+3, 319-landX-3, 2, YELLOW, BLACK);
}

/* Remove a button from the screen
 */
void
remove_labeled_button(short id) {
  Tbutton.removeAndClearButton(id);
}

/* Draw a fixed label on the screen at the given landscape orientation coordinates.
 * This won't be for a button, so color it white.
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 */
void
add_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.drawString(label, landY, 319-landX, 1, WHITE, BLACK);
}

/* Draw a small fixed label on the screen at the given landscape orientation coordinates.
 * This won't be for a button, so color it white.
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 * Shift into the smaller font long enough to draw the label.
 */
void
add_small_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.setTextFont(&simpleFont);
    Tft.drawString(label, landY, 319-landX, 1, WHITE, BLACK);
    Tft.setTextFont(&bigFont);
}

/* Draw a small fixed label on the screen at the given landscape orientation coordinates.
 * This won't be for a button, so color it white.
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 * Shift into the smaller font long enough to draw the label.
 */
void
add_small_label(char *label, unsigned int landX, unsigned int landY, unsigned int fgcolor, unsigned int bgcolor)
{
    Tft.setTextFont(&simpleFont);
    Tft.drawString(label, landY, 319-landX, 1, fgcolor, bgcolor);
    Tft.setTextFont(&bigFont);
}

/* Draw a fixed label on the screen at the given landscape orientation coordinates.
 * This won't be for a button, so color it white.
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 */
void
add_big_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.drawString(label, landY, 319-landX, 2, WHITE, BLACK);
}

/* Clear a previously drawn fixed label from the screen.
 * Specify the same label and landscape orientation coordinates as was used for add_label().
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 * Black on black text will erase everything.
 */
void
remove_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.drawString(label, landY, 319-landX, 1, BLACK, BLACK);
}

/* Clear a previously drawn fixed label from the screen.
 * Specify the same label and landscape orientation coordinates as was used for add_big_label().
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 * Black on black text will erase everything.
 */
void
remove_big_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.drawString(label, landY, 319-landX, 2, BLACK, BLACK);
}

/* Clear a previously drawn fixed label from the screen.
 * Specify the same label and landscape orientation coordinates as was used for add_big_label().
 * Translate the landscape coordinates to the normal LCD portrait coordinate system.
 * Black on black text will erase everything.
 */
void
remove_small_label(char *label, unsigned int landX, unsigned int landY)
{
    Tft.setTextFont(&simpleFont);
    Tft.drawString(label, landY, 319-landX, 1, BLACK, BLACK);
    Tft.setTextFont(&bigFont);
}

void 
draw_circle(unsigned int landX, unsigned int landY, unsigned int color)
{
    Tft.fillCircle(landY, 319-landX, 9, color);
}

char *
num_to_string(int value, int length)
{
    // Absolute maximim width for the string returned by this function
    const static int MAX_LEN = 25;
    static char staticString[MAX_LEN];
    byte index = 0;
    byte digit;

    // Disallow lengths that are too long, keeping in mind the final NULL.
    if (length < 2)
	length = 2;
    if (length >= MAX_LEN)
	length = MAX_LEN-1;
    // Leading minus or plus sign
    if (value < 0) {
	staticString[index++] = '-';
	value *= -1;
    } else {
	staticString[index++] = '+';
    }
    if (value == 0)
	// Special case to handle the case of zero
	staticString[index++] = '0';
    else {
	// Convert value to decimal digits (in wrong order)
	while (value > 0) {
	    digit = value % 10;
	    value = value / 10;
	    staticString[index++] = digit + '0';
	}
	// Now reverse the string in place to get digits in right order
	for (int i = 1; i <= index/2; i++) {
	    char tmp = staticString[i];
	    staticString[i] = staticString[index-i];
	    staticString[index-i] = tmp;
	}
    }
    // Pad the rest with spaces
    for (int i = index; i < length; i++) {
	staticString[i] = ' ';
    }
    // Null terminate the string
    staticString[length] = '\0';

    return staticString;
}
