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
    Add the macro definitions,public funtion and private variable for the char display direction.
*/
#include <Arduino.h>
#include <avr/pgmspace.h>

#ifndef TFT_h
#define TFT_h

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define MEGA
#else
#define SEEEDUINO
#endif

//Basic Colors
#define RED		0xf800
#define GREEN		0x07e0
#define BLUE		0x001f
#define BLACK		0x0000
#define YELLOW		0xffe0
#define WHITE		0xffff

//Other Colors
#define CYAN		0x07ff	
#define ORANGE		0xfbe0
#define BRIGHT_RED	0xf810	
#define GRAY1		0x8410  
#define GRAY2		0x4208  

//TFT resolution 240*320
#define MIN_X	0
#define MIN_Y	0
#define MAX_X	240
#define MAX_Y	320

// Define some special characters for the big font
// Arrows and connecting line that are solid
#define BGF_BACK_SLD 0x80 // 128 decimal
#define BGF_LINE_SLD 0x81
#define BGF_DVDR_SLD 0x82
#define BGF_FORW_SLD 0x83
#define BGF_UP_SLD   0x84
#define BGF_DOWN_SLD 0x85
// Arrows and connecting line that are hollow
#define BGF_BACK_HLW 0x86
#define BGF_LINE_HLW 0x87
#define BGF_DVDR_HLW 0x88
#define BGF_FORW_HLW 0x89
#define BGF_UP_HLW   0x8a
#define BGF_DOWN_HLW 0x8b

struct fontInfo {
    unsigned char rows;
    unsigned char cols;
    unsigned char minChar;
    unsigned char maxChar;
    unsigned char unknownChar;
    unsigned char *raster;
};

extern fontInfo simpleFont;
extern fontInfo bigFont;

enum Direction {
    LEFT2RIGHT,
    RIGHT2LEFT,
    DOWN2UP,
    UP2DOWN,
    VERTICAL,
};

class TFT
{
public:
    static const int RES_X = 240;
    static const int RES_Y = 320;

    void init (void);
    bool outOfBounds(unsigned int x, unsigned int y);
    void setXY(unsigned int poX, unsigned int poY);
    void setPixel(unsigned int poX, unsigned int poY,unsigned int color);
    void drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color);
    void drawVerticalLine(unsigned int poX, unsigned int poY, int length, unsigned int color);
    void drawHorizontalLine(unsigned int poX, unsigned int poY, int length, unsigned int color);
    void drawRectangle(unsigned int poX, unsigned int poY, int width, int height, unsigned int color);
    void fillRectangle(unsigned int poX, unsigned int poY, int width, int height, unsigned int color);
    void drawCircle(int poX, int poY, int r,unsigned int color);
    void fillCircle(int poX, int poY, int r,unsigned int color);
    void drawChar(unsigned char ascii, unsigned int poX, unsigned int poY, unsigned char scale, 
		  unsigned int fgcolor, unsigned int bgcolor);
    void drawString(char *string, unsigned int poX, unsigned int poY, unsigned char scale, 
		    unsigned int fgcolor, unsigned int bgcolor);
    void setTextDirection(Direction = LEFT2RIGHT);
    void setTextFont(fontInfo *font);
private:
    void all_pin_input(void);
    void all_pin_output(void);
    void all_pin_low(void);
    void sendCommand(unsigned char index);
    void sendData(unsigned int data);
    void pushData(unsigned char data);
    unsigned char getData(void);
    unsigned int readRegister(unsigned int index);
    void beginDrawing(void);
    void setDrawOrientation(Direction direction);
    void printPt(unsigned int poX, unsigned int poY, char *msg);
    void clearWindow(void);
    void setWindow(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
    Direction  _textDirection;
    fontInfo *_font;
};

extern TFT Tft;

#endif
