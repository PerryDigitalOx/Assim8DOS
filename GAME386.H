////////////////[  FUNCTION PROTOTYPES for GAME386.ASM  ]//////////////////
extern far int getvideotype( void );
extern far detect386( void );
extern far mouse_reset( void );
extern far hide_mouse( void );
extern far show_mouse( void );
extern far set_mouse_pos( int x, int y );
extern far set_mouse_range( int x1, int y1, int x2, int y2 );
extern far SetVgaMode( int mode );
extern far SetPal( char * palette );
extern far Clear( int c );

extern far PutBlock( int x, int y, int width, int height, char far* buffer );
extern far PutSprite( int x, int y, int width, int height, char far* buffer );
