#include <conio.h>  //needed for inportb

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
											  // when 1 - retrace in progress
											  // when 0 - no retrace
#define VGA_VSYNC_MASK 0x08        // masks off unwanted bit of status reg

void Wait_For_Vsync( void )
{
   while(inportb(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK);
   while(!(inportb(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK));
}
