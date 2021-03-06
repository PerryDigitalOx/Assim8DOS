#include "font.h"

/* To find what font array element to use, subtract 32 from */
/* the character's ASCII value (after checking for NULL) */

unsigned char far font[FONT_LETTERS][7] = {  // 7 = max height
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // Space
  { 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x00 }, // !
  { 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00 }, // "
  { 0x48, 0xFC, 0x48, 0x48, 0xFC, 0x48, 0x00 }, // #
  { 0x40, 0x60, 0x80, 0x40, 0x20, 0xC0, 0x40 }, // $
  { 0xC4, 0xC8, 0x10, 0x20, 0x4C, 0x8C, 0x00 }, // %
  { 0x40, 0xA0, 0x40, 0xA4, 0x90, 0x64, 0x00 }, // &
  { 0xC0, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00 }, // '
  { 0x20, 0x40, 0x80, 0x80, 0x40, 0x20, 0x00 }, // (
  { 0x80, 0x40, 0x20, 0x20, 0x40, 0x80, 0x00 }, // )
  { 0x00, 0x90, 0x60, 0xF0, 0x60, 0x90, 0x00 }, // *
  { 0x00, 0x00, 0x40, 0xE0, 0x40, 0x00, 0x00 }, // +
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80 }, // ,
  { 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00 }, // -
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00 }, // .
  { 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00 }, // /
  { 0x60, 0x90, 0xB0, 0xD0, 0x90, 0x60, 0x00 }, // 0
  { 0x40, 0xC0, 0x40, 0x40, 0x40, 0xE0, 0x00 }, // 1
  { 0x60, 0x90, 0x10, 0x60, 0x80, 0xF0, 0x00 }, // 2
  { 0xE0, 0x10, 0x60, 0x10, 0x10, 0xE0, 0x00 }, // 3
  { 0x20, 0xA0, 0xA0, 0xF0, 0x20, 0x20, 0x00 }, // 4
  { 0xF0, 0x80, 0xE0, 0x10, 0x10, 0xE0, 0x00 }, // 5
  { 0x70, 0x80, 0xE0, 0x90, 0x90, 0x60, 0x00 }, // 6
  { 0xF0, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00 }, // 7
  { 0x60, 0x90, 0x60, 0x90, 0x90, 0x60, 0x00 }, // 8
  { 0x60, 0x90, 0x90, 0x70, 0x10, 0xE0, 0x00 }, // 9
  { 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00 }, // :
  { 0x00, 0x40, 0x00, 0x00, 0x40, 0x80, 0x00 }, // ;
  { 0x00, 0x20, 0x40, 0x80, 0x40, 0x20, 0x00 }, // <
  { 0x00, 0xE0, 0x00, 0x00, 0xE0, 0x00, 0x00 }, // =
  { 0x00, 0x80, 0x40, 0x20, 0x40, 0x80, 0x00 }, // >
  { 0x60, 0x90, 0x10, 0x60, 0x00, 0x40, 0x00 }, // ?
  { 0x70, 0x88, 0xB8, 0xB0, 0x84, 0x78, 0x00 }, // @
  { 0x60, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x00 }, // A
  { 0xE0, 0x90, 0xE0, 0x90, 0x90, 0xE0, 0x00 }, // B
  { 0x60, 0x90, 0x80, 0x80, 0x90, 0x60, 0x00 }, // C
  { 0xE0, 0x90, 0x90, 0x90, 0x90, 0xE0, 0x00 }, // D
  { 0xF0, 0x80, 0xE0, 0x80, 0x80, 0xF0, 0x00 }, // E
  { 0xF0, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00 }, // F
  { 0x60, 0x90, 0x80, 0xB0, 0x90, 0x60, 0x00 }, // G
  { 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0x00 }, // H
  { 0xE0, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00 }, // I
  { 0x70, 0x20, 0x20, 0x20, 0xA0, 0x40, 0x00 }, // J
  { 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x90, 0x00 }, // K
  { 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00 }, // L
  { 0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88, 0x00 }, // M
  { 0x90, 0xD0, 0xB0, 0x90, 0x90, 0x90, 0x00 }, // N
  { 0x60, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00 }, // O
  { 0xE0, 0x90, 0x90, 0xE0, 0x80, 0x80, 0x00 }, // P
  { 0x70, 0x88, 0x88, 0x88, 0x90, 0x68, 0x00 }, // Q
  { 0xE0, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x00 }, // R
  { 0x70, 0x80, 0x60, 0x10, 0x10, 0xE0, 0x00 }, // S
  { 0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, // T
  { 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00 }, // U
  { 0x88, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00 }, // V
  { 0x88, 0x88, 0x88, 0xA8, 0xD8, 0x88, 0x00 }, // W
  { 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00 }, // X
  { 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00 }, // Y
  { 0xF0, 0x10, 0x20, 0x40, 0x80, 0xF0, 0x00 }, // Z
  { 0xE0, 0x80, 0x80, 0x80, 0x80, 0xE0, 0x00 }, // [
  { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00 }, // \
  { 0xE0, 0x20, 0x20, 0x20, 0x20, 0xE0, 0x00 }, // ]
  { 0x40, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ^
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00 }, // _
  { 0xC0, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00 }, // `
  { 0x00, 0x00, 0x70, 0x90, 0x90, 0x70, 0x00 }, // a
  { 0x80, 0x80, 0xE0, 0x90, 0x90, 0xE0, 0x00 }, // b
  { 0x00, 0x00, 0x70, 0x80, 0x80, 0x70, 0x00 }, // c
  { 0x10, 0x10, 0x70, 0x90, 0x90, 0x70, 0x00 }, // d
  { 0x00, 0x00, 0x70, 0xA0, 0xC0, 0x70, 0x00 }, // e
  { 0x30, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00 }, // f
  { 0x00, 0x00, 0x60, 0x90, 0x70, 0x10, 0xE0 }, // g
  { 0x80, 0x80, 0xE0, 0x90, 0x90, 0x90, 0x00 }, // h
  { 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00 }, // i
  { 0x00, 0x20, 0x00, 0x20, 0x20, 0x20, 0xC0 }, // j
  { 0x80, 0x80, 0xA0, 0xC0, 0xA0, 0x90, 0x00 }, // k
  { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00 }, // l
  { 0x00, 0x00, 0xD0, 0xA8, 0xA8, 0xA8, 0x00 }, // m
  { 0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x00 }, // n
  { 0x00, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 }, // o
  { 0x00, 0x00, 0xE0, 0x90, 0x90, 0xE0, 0x80 }, // p
  { 0x00, 0x00, 0x70, 0x90, 0x90, 0x70, 0x10 }, // q
  { 0x00, 0x00, 0x60, 0x80, 0x80, 0x80, 0x00 }, // r
  { 0x00, 0x00, 0x70, 0xC0, 0x30, 0xE0, 0x00 }, // s
  { 0x00, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00 }, // t
  { 0x00, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 }, // u
  { 0x00, 0x00, 0x88, 0x88, 0x50, 0x20, 0x00 }, // v
  { 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x58, 0x00 }, // w
  { 0x00, 0x00, 0x90, 0x60, 0x60, 0x90, 0x00 }, // x
  { 0x00, 0x00, 0x90, 0x90, 0x70, 0x10, 0xE0 }, // y
  { 0x00, 0x00, 0xF0, 0x20, 0x40, 0xF0, 0x00 }, // z
  { 0x10, 0x20, 0x20, 0xC0, 0x20, 0x20, 0x10 }, // {
  { 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00 }, // |
  { 0x80, 0x40, 0x40, 0x30, 0x40, 0x40, 0x80 }, // }
  { 0x64, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ~
};

int far font_width[FONT_LETTERS] = {
  5,2,4,7,4,7,6,3,4,4,5,4,3,4,2,7,  // Space thru slash
  5,4,5,5,5,5,5,5,5,5, // 0 thru 9
  2,3,5,4,5,5,7,  // Colon through at (@)
  5,5,5,5,5,5,5,5,4,5,5,5,6,5,5,5,6,5,5,4,5,6,6,6,6,5, // Cap. letters
  4,7,4,4,5,3,  // Right square bracket [ thru open quote (`)
  5,5,5,5,5,5,5,5,2,4,5,2,6,5,5,5,5,4,5,4,5,6,6,5,5,5, // Lowercase ltrs
  5,2,5,7  // Left sqiggle, broken bar, right squiggle, tilde
};

void far Text(int x, int y, char * str, int col, int bgcol)
{
  while(*str) {
	 Print_Letter(x, y, *str, col, bgcol);
	 x += font_width[*str-32];
	 ++str;
  }
}

void far Print_Letter(int x, int y, char letter, int col, int bgcol)
{
  static int y_scan, width;
  static unsigned char far * pix;
  static unsigned char mask;
  static unsigned char * fontptr;
  register int x_scan;
  if(letter <= 31) return;
  letter -= 32;
  pix = (unsigned char far *)0xA0000000+(y*320)+x;
  width = font_width[(int)letter];
  fontptr = font[(int)letter];

  for(y_scan = 0; y_scan != 7; y_scan++)
  {
	 mask = 128;
	 for(x_scan = 0; x_scan != width; x_scan++)
	 {
		if(*fontptr & mask)
			*pix = col;
		else
			if(bgcol!=0) *pix = bgcol;

		mask >>= 1;
		++pix;
	 }
	 ++fontptr;
	 pix += (320-width);
  }
}
