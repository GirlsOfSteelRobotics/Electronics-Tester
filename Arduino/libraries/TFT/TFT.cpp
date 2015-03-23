/*
  ST7781R TFT Library. 

  2011 Copyright (c) Seeed Technology Inc.
 
  Authors: Albert.Miao, Visweswara R (with initializtion code from TFT vendor)
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/*
  Modified record:
  2012.3.27 by Frankie.Chu
    Add funtion:setDisplayDirect.
    Add more conditional statements in funtions,fillRectangle,drawChar,drawString 
    to deal with different directions displaying.
*/
#include "TFT.h" 

/*
 * The port definitions below are only used in this file and not by
 * users of the library so they belong here instead of the header file.
 */
#ifdef SEEEDUINO
//==================/CS=====================
#define DDR_CS      DDRB
#define PORT_CS     PORTB
#define CS_BIT      0x04
#define CS_OUTPUT   {DDR_CS|=CS_BIT;}
#define CS_HIGH     {PORT_CS|=CS_BIT;}
#define CS_LOW      {PORT_CS&=~CS_BIT;}
//------------------RS----------------------
#define DDR_RS      DDRB
#define PORT_RS     PORTB
#define RS_BIT      0x08
#define RS_OUTPUT   {DDR_RS|=RS_BIT;}
#define RS_HIGH     {PORT_RS|=RS_BIT;}
#define RS_LOW      {PORT_RS&=~RS_BIT;}
//------------------WR----------------------
#define DDR_WR      DDRB
#define PORT_WR     PORTB
#define WR_BIT      0x10
#define WR_OUTPUT   {DDR_WR|=WR_BIT;}
#define WR_HIGH     {PORT_WR|=WR_BIT;}
#define WR_LOW      {PORT_WR&=~WR_BIT;}
//------------------RD---------------------
#define DDR_RD      DDRB
#define PORT_RD     PORTB
#define RD_BIT      0x20
#define RD_OUTPUT   {DDR_RD|=RD_BIT;}
#define RD_HIGH     {PORT_RD|=RD_BIT;}
#define RD_LOW      {PORT_RD&=~RD_BIT;}
//========================================
#endif

#ifdef MEGA
//==================/CS=====================
#define DDR_CS      DDRB
#define PORT_CS     PORTB
#define CS_BIT      0x10
#define CS_OUTPUT   {DDR_CS|=CS_BIT;}
#define CS_HIGH     {PORT_CS|=CS_BIT;}
#define CS_LOW      {PORT_CS&=~CS_BIT;}
//------------------RS----------------------
#define DDR_RS      DDRB
#define PORT_RS     PORTB
#define RS_BIT      0x20
#define RS_OUTPUT   {DDR_RS|=RS_BIT;}
#define RS_HIGH     {PORT_RS|=RS_BIT;}
#define RS_LOW      {PORT_RS&=~RS_BIT;}
//------------------WR----------------------
#define DDR_WR      DDRB
#define PORT_WR     PORTB
#define WR_BIT      0x40
#define WR_OUTPUT   {DDR_WR|=WR_BIT;}
#define WR_HIGH     {PORT_WR|=WR_BIT;}
#define WR_LOW      {PORT_WR&=~WR_BIT;}
//------------------RD---------------------
#define DDR_RD      DDRB
#define PORT_RD     PORTB
#define RD_BIT      0x80
#define RD_OUTPUT   {DDR_RD|=RD_BIT;}
#define RD_HIGH     {PORT_RD|=RD_BIT;}
#define RD_LOW      {PORT_RD&=~RD_BIT;}
//========================================
#endif

#ifdef MAPLE
 // not yet implemented
#endif

void TFT::all_pin_input(void)
{
#ifdef SEEEDUINO
    DDRD &=~ 0xfc;
    DDRB &=~ 0x03;
#endif

#ifdef MEGA
    DDRE &=~ 0x38;
    DDRG &=~ 0x20;
    DDRH &=~ 0x78;
#endif
}

void TFT::all_pin_output(void)
{
#ifdef SEEEDUINO
    DDRD |= 0xfc;
    DDRB |= 0x03;
#endif

#ifdef MEGA
    DDRE |= 0x38;
    DDRG |= 0x20;
    DDRH |= 0x78;
#endif
}

void TFT::all_pin_low(void)
{
#ifdef SEEEDUINO
    PORTD &=~ 0xfc;
    PORTB &=~ 0x03;
#endif

#ifdef MEGA
    PORTE &=~ 0x38;
    PORTG &=~ 0x20;
    PORTH &=~ 0x78;
#endif
}

/* Send the byte of data to the LCD data pins.
 * Masking the argument with (&0xFF) is not necessary.
 */
void TFT::pushData(unsigned char data)
{
#ifdef SEEEDUINO
    all_pin_low();
    PORTD |= (data<<2);
    PORTB |= (data>>6);
#endif

#ifdef MEGA
    PORTE = (PINE & ~0x38) |
      ((data<<4) & 0x30) |
      ((data & 0x08));
    PORTG = (PING & ~0x20) |
      ((data<<3) & 0x20);
    PORTH = (PINH & ~0x78) |
      ((data>>1) & 0x78);
#endif
}

unsigned char TFT::getData(void)
{
    unsigned char data=0;
    delay(1);
#ifdef SEEEDUINO
    data |= ((PIND&0xfc)>>2);
    data |= ((PINB&0x03)<<6);
#endif

#ifdef MEGA
    data |= (PINE & 0x30)>>4;
    data |= (PING & 0x20)>>3;
    data |= (PINE & 0x08);
    data |= (PINH & 0x78)<<1;
#endif
    return data;
}

void TFT::sendCommand(unsigned char index)
{
    CS_LOW;
    RS_LOW;
    RD_HIGH;
    WR_HIGH;

    WR_LOW;
    pushData(0);
    WR_HIGH;
    WR_LOW;
    pushData(index);
    WR_HIGH;

    CS_HIGH;
}

void TFT::sendData(unsigned int data)
{
    CS_LOW;
    RS_HIGH;
    RD_HIGH;

    WR_LOW;
    pushData(data>>8);
    WR_HIGH;

    WR_LOW;
    pushData(data);
    WR_HIGH;

    CS_HIGH;
}

/* Not called by the code below, but good to have around
 */
unsigned int TFT::readRegister(unsigned int index)
{
    unsigned int data=0;

    CS_LOW;
    RS_LOW;
    RD_HIGH;

    all_pin_output();

    WR_LOW;
    pushData(0);
    WR_HIGH;

    WR_LOW;
    pushData(index);
    WR_HIGH;

    all_pin_input();
    all_pin_low();
    RS_HIGH;

    RD_LOW;
    RD_HIGH;
    data |= getData()<<8;

    RD_LOW;
    RD_HIGH;
    data |= getData();

    CS_HIGH;
    all_pin_output();
    return data;
}

void TFT::init(void)
{
    CS_OUTPUT;
    RD_OUTPUT;
    WR_OUTPUT;
    RS_OUTPUT;

    Tft.all_pin_output();
    Tft.all_pin_low();

    delay(100);

    // Turn off the screen to start via Display Control 1
    sendCommand(0x07);	// Display control 1
    sendData(0x0030);	// PTDE=00 no partial image, BASEE=0 base image disabled
			// GON=1 DTE=1 Normal display CL=0 8-bit off D0,D1=00 base image off
    sendCommand(0x01);	// Driver Output Control
    sendData(0x0100);	// SM=0 SS=1 set shift direction appropriate for the LCD
    // 0x03 Entry Mode is set below by setDrawingOrientation()
    sendCommand(0x04);	// Resize Control
    sendData(0x0000);	// RSZ, RCH, RCV = 0 for no resizing
    sendCommand(0x08);	// Display Control 2
    sendData(0x0302);	// FP=3 BP=2 front and back porch
    sendCommand(0x0A);	// Display Control 4
    sendData(0x0000);	// FMARKOE=0 FMI=0 fmark output pin disabled
    sendCommand(0x0C);	// RGB Display Interface Control 1
    sendData(0x0000);	// ENC=00 RM=0 DM=00 RIM=0 RGB input mode off
    sendCommand(0x0D);	// Frame Marker Position
    sendData(0x0000);	// FMP=0 frame mark output on line zero
    sendCommand(0x0F);	// RGB Display Interface Control 2
    sendData(0x0000);	// VSPL=0 HSPL=0 EPL=0 DPL=0 pin settings for RGB mode

    delay(100);

    sendCommand(0x30);	// Gamma Control 1
    sendData(0x0000);
    sendCommand(0x31);	// Gamma Control 2
    sendData(0x0405);
    sendCommand(0x32);	// Gamma Control 3
    sendData(0x0203);
    sendCommand(0x35);	// Gamma Control 4
    sendData(0x0004);
    sendCommand(0x36);	// Gamma Control 5
    sendData(0x0B07);
    sendCommand(0x37);	// Gamma Control 6
    sendData(0x0000);
    sendCommand(0x38);	// Gamma Control 7
    sendData(0x0405);
    sendCommand(0x39);	// Gamma Control 8
    sendData(0x0203);
    sendCommand(0x3c);	// Gamma Control 9
    sendData(0x0004);
    sendCommand(0x3d);	// Gamma Control 10
    sendData(0x0B07);
    sendCommand(0x20);	// Horizontal DRAM Address Set
    sendData(0x0000);
    sendCommand(0x21);	// Vertical DRAM Address Set
    sendData(0x0000);
    clearWindow();

    delay(100);

    sendCommand(0x60);	// Driver Output Control 2 (see also 0x01 above)
    sendData(0xa700);	// SCN=0 NL=2F GS=1 number of lines is 320, gate scan inverted
    sendCommand(0x61);	// Base Image Display Control
    sendData(0x0001);	// NDL=0 VLE=0 REV=1 disable non-display area output, vertical scrolling, display is normally white
    sendCommand(0x90);	// Panel Interface Control 1
    sendData(0x003A);	// DIV=0 RTNI=58d clock division ratio 1:1, line period is every 58 clocks
    sendCommand(0x95);	// Panel Interface Control 2
    sendData(0x021E);	// DIVE=10 division ratio 1/8; 1E is default value for unlabeled bits
    sendCommand(0x80);	// Partial Image 1 Display Position
    sendData(0x0000);	// PTDP0=0 display start position
    sendCommand(0x81);	// Partial Image 1 Start Address
    sendData(0x0000);	// PTSA0=0
    sendCommand(0x82);	// Partial Image 1 End Address
    sendData(0x0000);	// PTEA0=0
    sendCommand(0x83);	// Partial Image 2 Display Position
    sendData(0x0000);	// PTDP1=0
    sendCommand(0x84);	// Partial Image 2 Start Address
    sendData(0x0000);	// PTSA1=0
    sendCommand(0x85);	// Partial Image 2 End Address
    sendData(0x0000);	// PTEA1=0
    sendCommand(0xB0);	// Power Control 2
    sendData(0x140D);	// VCM=010100 VGLSEL=00 VGHBT=01 set VCOM to -0.925, VGL to -7.5, VGH to 3*AVDD

    sendCommand(0x10);	// Power Control 1
    sendData(0x10E0);	// SAP=1 APE=1 AP=110 enable power supply with min current draw
			// SLP=0 STB=0 sleep and standby both disabled
			// why not 10E0?
    delay(100);

    // Paint the screen black 
    // (The drawing window was reset to the entire screen above)
    beginDrawing();
    for(unsigned int x=0; x<MAX_X; x++) {
        for(unsigned int y=0; y<MAX_Y; y++) {
            sendData(BLACK);
        }
    }
    sendCommand(0x07);	// Display control 1
    sendData(0x0133);	// PTDE=00 no partial image, BASEE=1 base image enabled
			// GON=1 DTE=1 Normal display CL=0 8-bit off D0,D1=11 base image display

    // Initial setting for Entry Mode and associated text direction variable
    _textDirection = LEFT2RIGHT;
    setDrawOrientation(_textDirection);
    
    setTextFont(&bigFont);
    //setTextFont(&simpleFont);
}

void TFT::beginDrawing(void) {
    sendCommand(0x22);	//Start to write to display RAM
}

/*
 * LEFT2RIGHT horizontal in the left to right direction
 * DOWN2UP    vertical in the bottom to top direction
 * RIGHT2LEFT horizontal in the right to left direction
 * UP2DOWN    vertical in the top to bottom direction
 */
void TFT::setDrawOrientation(Direction direction)
{
    static unsigned char _currentDirection = 0xFF;

    if (_currentDirection == direction) {
	//Serial.print("Direction already set to: ");
	//Serial.println(direction);
	return;
    }
    //Serial.print("Direction set to: ");
    //Serial.println(direction);

    _currentDirection = direction;
    sendCommand(0x03);	// Entry mode register
    switch (direction) {
    case LEFT2RIGHT:
	// TRI=0 DFM=0 (two bytes per data transfer) 
	// BGR=1 (red in MSBs) HWM=0 (high speed write mode)
	// ORG=0 (origin not moved) 
	// ID=11 AM=0 (write in "F" order (see datasheet diagrams))
        sendData(0x1030);
        break;
    case RIGHT2LEFT:
	// Same as above except for:
	// ID=00 AM=0 (upside down "F" order)
        sendData(0x1000);
	break;
    case DOWN2UP:
	// Same as above except for:
	// ID=01 AM=1 ("F" upwards)
        sendData(0x1018);
	break;
    case UP2DOWN:
	// Same as above except for:
	// ID=10 AM=1 ("F" downwards)
        sendData(0x1028);
	break;
    case VERTICAL:
	// Same as above except for:
	// ID=11 AM=1 ("F" backwards and downwards)
        sendData(0x1038);
    }
}

bool TFT::outOfBounds(unsigned int x, unsigned int y) 
{
  return (x < 0 || x >= TFT::RES_X || 
	  y < 0 || y >= TFT::RES_Y);
}

void TFT::setXY(unsigned int poX, unsigned int poY)
{
    sendCommand(0x20);	// Horizontal Address Set
    sendData(poX);
    sendCommand(0x21);	// Vertical Address Set
    sendData(poY);
}

void TFT::clearWindow(void)
{
    setWindow(0,0, 239,319);
}

void TFT::setWindow(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
{
    Tft.sendCommand(0x50);	// Horizontal Address Start Position
    Tft.sendData(x0);
    Tft.sendCommand(0x51);	// Horizontal Address End Position
    Tft.sendData(x1);
    Tft.sendCommand(0x52);	// Vertical Address Start Position
    Tft.sendData(y0);
    Tft.sendCommand(0x53);	// Vertical Address End Position
    Tft.sendData(y1);
}

void TFT::setPixel(unsigned int poX, unsigned int poY,unsigned int color)
{
    setXY(poX,poY);
    beginDrawing();
    sendData(color);
}

/* TODO: This seems to be quite broken, drawing at the edge of the screen!!
 */
void TFT::drawCircle(int poX, int poY, int r,unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2; 
    do {
        setPixel(poX-x, poY+y,color); 
        setPixel(poX+x, poY+y,color); 
        setPixel(poX+x, poY-y,color); 
        setPixel(poX-x, poY-y,color); 
        e2 = err;
        if (e2 <= y) { 
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0; 
        }
        if (e2 > x) err += ++x*2+1; 
    } while (x <= 0);
}

void TFT::fillCircle(int poX, int poY, int r,unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {

        drawVerticalLine(poX-x,poY-y,2*y,color);
        drawVerticalLine(poX+x,poY-y,2*y,color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}


void TFT::drawLine(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned int color)
{
    int x = x1-x0;
    int y = y1-y0;
    int dx = abs(x), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */
    for (;;){ /* loop */
        setPixel(x0,y0,color);
        e2 = 2*err;
        if (e2 >= dy) { /* e_xy+e_x > 0 */
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) { /* e_xy+e_y < 0 */
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}

void TFT::drawHorizontalLine(unsigned int poX, unsigned int poY, int length, unsigned int color)
{
    //printPt(poX, poY, "drawHorizontal x,y");
    //printPt(length, -1, "drawHorizontal length, -1");
    if (length < 1)
      return;
    if (length + poX > TFT::RES_X) {
        length = TFT::RES_X - poX;
    }
    setDrawOrientation(LEFT2RIGHT);
    setXY(poX,poY);
    beginDrawing();
    for(int i=0; i<length; i++) {
        sendData(color);
    }
}

void TFT::drawVerticalLine(unsigned int poX, unsigned int poY, int length, unsigned int color)
{
    //printPt(poX, poY, "drawVertical x,y");
    //printPt(length, -1, "drawVertical length, -1");
    if (length < 1)
      return;
    if (length + poY > TFT::RES_Y) {
        length = TFT::RES_Y - poY;
    }
    setDrawOrientation(VERTICAL);
    setXY(poX,poY);
    beginDrawing();
    for(int i=0; i<length; i++) {
        sendData(color);
    }
}

void TFT::printPt(unsigned int poX, unsigned int poY, char *msg)
{
    Serial.print(msg);
    Serial.print(": (");
    Serial.print(poX);
    Serial.print(",");
    Serial.print(poY);
    Serial.println(")");
}

void TFT::drawRectangle(unsigned int poX, unsigned int poY, int width, int height, unsigned int color)
{
    unsigned int endX, endY;

    if (outOfBounds(poX, poY) || width < 1 || height < 1) {
	//Serial.println("Out of bounds or zero size");
	return;
    }
    endX = poX + width - 1;
    endY = poY + height - 1;

    //printPt(poX, poY, "poX,poxY");
    //printPt(endX, endY, "endX,endY");

    drawHorizontalLine(poX, poY,  width, color);
    drawHorizontalLine(poX, endY, width, color);

    // Vertical lines are shortened to avoid double-drawing the corner pixels
    drawVerticalLine(poX,  poY+1, height-2, color);
    drawVerticalLine(endX, poY+1, height-2, color);
}

void TFT::fillRectangle(unsigned int poX, unsigned int poY, int width, int height, unsigned int color)
{
    for(int i=0; i<height; i++) {
	drawHorizontalLine(poX, poY+i, width, color);
    }
}

void TFT::setTextFont(fontInfo *font)
{
    _font = font;
}

/*
 * LEFT2RIGHT horizontal in the left to right direction
 * DOWN2UP    vertical in the bottom to top direction
 * RIGHT2LEFT horizontal in the right to left direction
 * UP2DOWN    vertical in the top to bottom direction
 */
void TFT::setTextDirection(Direction direction) 
{
    _textDirection = direction;
}

/* drawChar
 *
 * Draw a single character on the screen, given its ASCII code.
 * This is implemented using drawString, instead of vice-versa,
 * because the string function optimizes by drawing all characters
 * in the string at once.
 */
void TFT::drawChar(unsigned char ascii, unsigned int poX, unsigned int poY, unsigned char scale, 
		      unsigned int fgcolor, unsigned int bgcolor)
{
    char string[2] = {ascii, 0};
    drawString(string, poX, poY, scale, fgcolor, bgcolor);
}

/* drawString
 *
 * Draw a string on the screen using the selected font.
 *
 * We're given an origin for drawing the text (poX,poY) that's
 * relative to the direction of the text. We let the caller give the 
 * origin as the corner where a human would start writing
 * the text.
 *
 * TODO:
 * - It would be nice to shorten strings that would otherwise not fit on-screen
 */
void TFT::drawString(char *string, unsigned int poX, unsigned int poY, unsigned char scale, 
			unsigned int fgcolor, unsigned int bgcolor)
{
    char *currentChar;
    unsigned char ascii;
    unsigned int topLeftX, topLeftY, botRightX, botRightY;

    prog_uchar *raster = _font->raster;
    unsigned char bytesPerRow = _font->cols/8;
    unsigned char bytesPerChar = _font->rows * bytesPerRow;

    /* Start by setting the drawing position (setXY) to the upper-left
     * corner of the drawing range. Then set the drawing window to start
     * from that corner and extend an appropriate amount down and right.
     */
    switch (_textDirection) {
    case LEFT2RIGHT:
	topLeftX = poX;
	topLeftY = poY;
	botRightX = poX+(_font->cols*strlen(string)*scale)-1;
	botRightY = poY+(_font->rows*scale)-1;
	break;
    case RIGHT2LEFT:
	topLeftX = poX-(_font->cols*strlen(string)*scale)+1;
	topLeftY = poY-(_font->rows*scale)+1;
	botRightX = poX;
	botRightY = poY;
	break;
    case UP2DOWN:
	topLeftX = poX-(_font->rows*scale)-1;
	topLeftY = poY;
	botRightX = poX;
	botRightY = poY+(_font->cols*strlen(string)*scale)-1;
	break;
    case DOWN2UP:
	topLeftX = poX;
	topLeftY = poY-(_font->cols*strlen(string)*scale)+1;
	botRightX = poX+(_font->rows*scale)+1;
	botRightY = poY;
	break;
    }
    /* Check the values to see if drawing the string would result in
     * painting off an edge of the screen. If so, bail out now.
     */
    if (outOfBounds(topLeftX, topLeftY)) {
        //Serial.println("drawBigString: aborting on off-screen top-left");
	return;
    }
    if (outOfBounds(botRightX, botRightY)) {
        //Serial.println("drawBigString: aborting on off-screen bot-right");
	return;
    }
    // The values are good, so set them and continue
    setXY(topLeftX, topLeftY);
    setWindow(topLeftX, topLeftY, botRightX, botRightY);

    /* Now set the entry mode (drawing direction) to be match the direction
     * of the text. We'll always send out the raster of the text in the same
     * order so most of the work of flipping it around is handled here.
     */
    setDrawOrientation(_textDirection);

    beginDrawing();
    // We'll draw the top raster line of the entire string, 
    // then go to the next line, etc.
    // Iterate over the number of lines in the current font's rasters
    for(unsigned int line=0;line<_font->rows;line++) {
	for (unsigned char repeatLine=0; repeatLine<scale; repeatLine++) {
	    // Start at the first character in the string
	    currentChar = string;
	    while (ascii = *currentChar) {
		if (ascii < _font->minChar || ascii > _font->maxChar) {
		    //Unsupported char.
		    ascii = _font->unknownChar;
		}
		for (unsigned char bytenum=0; bytenum < bytesPerRow; bytenum++) {
		    //prog_uchar temp = pgm_read_byte(&bigFontRaster[ascii-0x20][line*2 + bytenum]);
		    prog_uchar temp = pgm_read_byte(raster+(ascii-_font->minChar)*bytesPerChar+(line*bytesPerRow + bytenum));
		    for(unsigned char bit=0;bit<8;bit++) {
			for(unsigned char repeatBits=0; repeatBits<scale; repeatBits++) {
			    if((temp<<bit)&0x80) {
				sendData(fgcolor);
			    } else {
				sendData(bgcolor);
			    }
#if 0
			    delay(1); // Slow it down so we can watch it draw
#endif
			}
		    }
		}
		// Advance to the next character in the string
		currentChar++;
	    }
	}
    }
    // Reset the drawing window so other operations aren't subject to unexpected wrapping
    clearWindow();
}

TFT Tft=TFT();
