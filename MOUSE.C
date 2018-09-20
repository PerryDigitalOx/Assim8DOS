
#include <bios.h>
#include "mouse.h"

static far int mousex, mousey, mousebut;

void check_mouse( int *x, int *y, int *b )
{
	union REGS regs;

	regs.x.ax = 0x03; // subfunction 3: get position and buttons
	int86(MOUSE_INT, &regs, &regs);
	*x       = regs.x.cx;
	*y       = regs.x.dx;
	*b       = regs.x.bx;
}


/*
void mouse_set_graphics_cursor(char far *mask)
{
_DX = FP_OFF(mask);
_ES = FP_SEG(mask);
_CX = 0;
_BX = 0;
_AX = 9;
__int__(0x33);
}

unsigned int mouse_pic[32]= {
	0XFFFF, 0XFFFF, 0XFFFF, 0X9FFF, 0X8FFF, 0X87FF, 0X83FF, 0X81FF, 
	0X80FF, 0X81FF, 0X83FF, 0X83FF, 0X83FF, 0XF1FF, 0XF1FF, 0XFBFF, 
	0X0000, 0X0000, 0X0000, 0X0000, 0X2000, 0X3000, 0X3800, 0X3C00, 
	0X3E00, 0X3C00, 0X3800, 0X2800, 0X0800, 0X0400, 0X0400, 0X0000, 
};*/

/*   unsigned int pattern[32]={0x0000,0x0000,0x0000,0x0ff0,0x0ff0,0x0ff0,0x0ff0,
		0x0ff0,0x0ff0,0x0ff0,0x0ff0,0x0ff0,0x0ff0,0x0000,0x0000,0x0000,
		0xffff,0x8001,0x9ff9,0x9009,0x9009,0x9009,0x9009,0x9009,
		0x9009,0x9009,0x9009,0x9009,0x9009,0x9ff9,0x8001,0xffff};

		mouse_set_graphics_cursor(right_hand);
*/



//////////////////////////////////////////////////////////////////////////////

/*      case MOUSE_MOTION_REL:
		{
			  // this functions gets the relative mouse motions from the last
			  // call and puts them in the vars x,y respectively
			  inregs.x.ax = 0x03; // subfunction 11: get relative motion
			  int86(MOUSE_INT, &inregs, &outregs);
			  // extract the info and send back to caller via pointers
			  *x       = outregs.x.cx;
			  *y       = outregs.x.dx;
			  return(1);
		}
		case MOUSE_SET_SENSITIVITY:
		{
			  // this function sets the overall "sensitivity" of the mouse.
			  // each axis can have a sensitivity from 1-100.  So the caller
			  // should put 1-100 in both "x" and "y" before calling/
			  // also "buttons" is used to send in the doublespeed value which
			  // ranges from 1-100 also.
			  inregs.x.bx = *x;
			  inregs.x.cx = *y;
			  inregs.x.dx = *buttons;
			  inregs.x.ax = 0x1A; // subfunction 26: set sensitivity
			  int86(MOUSE_INT, &inregs, &outregs);
			  return(1);
		}*/

