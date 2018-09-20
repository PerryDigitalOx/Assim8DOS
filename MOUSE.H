// M O U S E . H /////////////////////////////////////////////////////////////
//#include <bios.h>

// D E F I N E S  ////////////////////////////////////////////////////////////
// mouse sub-function calls
#define MOUSE_INT                0x33 //mouse interrupt number
#define MOUSE_BUTT_POS           0x03 // get buttons and postion
#define MOUSE_CHANGE             0x09 // change mouse graphic
#define MOUSE_SET_SENSITIVITY    0x1A // set the sensitivity of mouse 0-100
#define MOUSE_MOTION_REL         0x0B // query motion counters to compute
												  // relative motion
// defines to make reading buttons easier
#define MOUSE_LEFT_BUTTON        0x01 // left button mask
#define MOUSE_RIGHT_BUTTON       0x02 // right button mask
#define MOUSE_CENTER_BUTTON      0x04 // center button mask

// F U N C T I O N   P R O T O T Y P E //////////////////////////////////////
void check_mouse( int *x, int *y, int *buttons );
//void mouse_set_graphics_cursor(char far *mask);

