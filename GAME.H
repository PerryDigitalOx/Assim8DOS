// G A M E . H //////////////////////////////////////////////////////////////
//*********************[  GLOBAL DEFINES  ]************************
#define VGALOWRES  0x13
#define TEXTRES    0x03
#define VIDMEM ((char far*)0xa0000000L)

#define  EXITTODOS      -1
#define  STARTNEWGAME    1
#define  RESUMEGAME      2
#define  INFO            3
#define  CREDITS         4
#define  STOPGAME        5

#define  ON             TRUE
#define  OFF            FALSE

#define  COMPUTER       0
#define  HUMAN          1

#define  SBCARD        1
#define  NOCARD        0

#define   DELAY1     150


//*********************[  GLOBAL DEFINITIONS  ]************************
extern unsigned char far *bg_buf;
extern unsigned char far *bg_buf2;
extern unsigned char *sp_image[MAXSPRITES];  // Pointers to bitmaps for each sprite


signed char far *getraf( unsigned long int *length);
signed char far *getboard( int board_level );

//SPRITE.C
//void far grab_bg( int x, int y, int w, int h, unsigned char far *dest_buf, char far *src_buf );

#define EMPTY     0   // Value which indicates an empty board square
#define PLAYERX   1   // Value which indicates a piece owned by player "O"
#define PLAYERO   2   // Value which indicates a piece owned by player "X"
#define NA        9
#define BOMBED    8
#define TOMBED    7
#define FLUSHED   6
#define SETBOMB   5

extern unsigned long int raflength[MAXRAFS];
extern unsigned char far *rafdata[MAXRAFS];
extern unsigned char far *palette[MAXPALETTES];

// F U N C T I O N S . H ////////////////////////////////////////////////////
/*void grab_bg(int x, int y, int w, int h, unsigned char far *dstbuffer, char far *srcbuffer )
{
//"Grab" a rect sprite image from a 64K bitmap(SRC) and store in (DEST)sprite.
   int row, column;

//Loop through rows and columns of sprite,storing pixels in sprite buffer
   for(row=0; row<h; row++)
   {
      for(column=0; column<w; column++)
      {
         dstbuffer[row*w+column] = srcbuffer[(y+row)*320+x+column];
      }
   }
}*/
