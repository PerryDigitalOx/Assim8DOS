#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <alloc.h>
#include <time.h>
#include <mem.h>

extern unsigned _stklen = 45000;   // The stack is usually only 4K
#define  USAGESIZE  89200LU

FILE *resfile;

static far int mousex, mousey, mousebut;

#include "resindex.h"
#include "gametags.h"

#include "mouse.h"
#include "sound.h"
#include "game.h"
#include "game386.h"
#include "font.h"
#include "pal.h"
#include "logic.h"
#include "keys.h"
#include "vsync.h"

static int far level1[16] = {0,4,0,1,2,7,3,8,4,1,5,6,7,2,8,3};
static int far level2[16] = {1,0,2,3,1,4,1,0,0,2,3,0,2,1,0,1};
static int far level3[16] = {0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0};

struct{
   unsigned int X;     // Sprite X positions
   unsigned int Y;     // Sprite Y positions
   unsigned int W;     // Sprite WIDTH
   unsigned int H;     // Sprite HEIGHT
   unsigned char far *image;     // Sprite data
}sprite[MAXSPRITES];

unsigned char far *bg_buf;
unsigned char far *bg_buf2;
unsigned char *sp_image[MAXSPRITES];  // Pointers to bitmaps for each sprite

unsigned long int raflength[MAXRAFS];
unsigned char far *rafdata[MAXRAFS];
unsigned char far *palette[MAXPALETTES];

unsigned int far boardx[BSIZE];
unsigned int far boardy[BSIZE];


int registered;
int maxboardscanuse=MAXBOARDS;
//#define DEBUG  1
//#define SKIP   1

#ifdef  DEBUG
void displayfreemem( void );
#endif

void    check_serial_number( void );
int     getkey( void );
int     build_board( board_t , int board_level);
int     calc_x( int xx );
int     calc_y( int yy );
void    capture( board_t board, int player, int to_x, int to_y);
int     check_kbd( void );
void    checkforhit(void);
int     checkpieces( board_t board, int player );
void    decode( unsigned char * stringdst, unsigned char * stringsrc );
void    display_cells( void );
void    display_digits( int number, int x, int y );
int     do_game( board_t board, int player );
void    door_open( int x, int y );
void    door_close( int x, int y );
int     draw_board( board_t );
void    exitwerr( void );
void    explode( int x, int y );
void    flush_kbd( void );
void    flushit( int x, int y, int player );
void    free_memory( void );
SAMPLE *getraf( unsigned long int *length);
SAMPLE *getboard( int board_level );
int     hint( board_t board, int player );
void    intro( void );
void    load_pic( unsigned char far * buffer, unsigned long int picpos );
int     mainmenu( void );
void    put_avail_moves( board_t board, int player, int from_x, int from_y );
void    put_piece( int player, int to_x, int to_y );
void    read_res( void );
void    shootout( int x, int y, int player );
void    spot_screen( void );
void    startup( void );
int     switch_players( int player );
void    toggle_show_moves( void );
void    toggle_sound( void );
void    update_menu_ops( void );
void    v_delay( int times );

unsigned char far * _scrn = (unsigned char far *)0xA0000000L;

unsigned char far *boardin;
unsigned long int lboardin;

int hintsused = 0;
int gameinprogress = FALSE;
int allsound=ON;
int show_avail=ON;
int board_level = 1;
int skip=TRUE;
int bombs_left = 0;
int killers=ON;
int cell=1;
char cell_name[MAXBOARDS+2][13];

int playerXtype, playerOtype, curplayertype;
int playerXdepth, playerOdepth, curplayerdepth;
int master_snd_sys;

///////////////////////////////////////////////////////////
// MAIN Function
///////////////////////////////////////////////////////////
void main( void )
{
    int menustate, player, rndcnt, c,i;
    char cheat[10];
	board_t board;

	registered = FALSE;
	if((resfile = fopen("standard.res","rb"))==NULL)
	{
		printf("\nERROR: Couldn't open resource file!\n");
		exit( 1 );
	}
    check_serial_number();

	startup();

	read_res();

#ifdef  SKIP
   i=0;
	while(kbhit())
	{
		c=getch();
		cheat[i++]=c;
    }
    if(strstr( cheat, "skip" )!=NULL)
        skip=TRUE;
#endif

	player = PLAYERX;
	curplayertype =  playerXtype;
	playerXtype = HUMAN;
	playerOtype = COMPUTER;
	playerOdepth = 1;

	SetVgaMode(VGALOWRES);        // to 320x200x256
	SetPal( palette[TITLEPAL] );

#ifdef SKIP
    if(!skip)
#endif
    intro();

	set_mouse_pos( 0,0 );
	set_mouse_range( 0, 0, 620, 185 );
	
	randomize();

	while( menustate!=EXITTODOS )
	{
		menustate = mainmenu();
		if( menustate==STARTNEWGAME )
		{
            hintsused=0;
			board_level = cell;
			build_board( board, cell );
			player = PLAYERX;
			curplayertype =  playerXtype;
		}
		else if(menustate==INFO )
		{  
			FadeOut( palette[MENUPAL], 5 );
			hide_mouse();
			_scrn = (unsigned char far *)0xA0000000L;
			load_pic( _scrn, PIC5_FOFFSET );
			show_mouse();
			FadeIn( palette[INFOPAL], 5 );
            v_delay(20);
			checkforhit();
			FadeOut( palette[INFOPAL], 3 );
			hide_mouse();
			_scrn = (unsigned char far *)0xA0000000L;
			load_pic( _scrn, PIC6_FOFFSET );
			show_mouse();
			FadeIn( palette[INFOPAL], 3 );
            v_delay(20);
			checkforhit();
			FadeOut( palette[INFOPAL], 3 );
			hide_mouse();
			continue;
		}
		else if( menustate==CREDITS )
		{
			FadeOut( palette[MENUPAL], 5 );
			hide_mouse();
			_scrn = (unsigned char far *)0xA0000000L;
			load_pic( _scrn, PIC7_FOFFSET );
			show_mouse();
			FadeIn( palette[CREDITSPAL], 5 );
            v_delay(20);
			checkforhit();
			FadeOut( palette[CREDITSPAL], 5 );
			hide_mouse();
			continue;
		}
		else if( menustate==EXITTODOS )
		{
            while(sounds_in_queue!=0);
            if(master_snd_sys) playsound(rafdata[ALARMSND], raflength[ALARMSND]);
            FadeToRed( palette[MENUPAL], 1 );
            FadeToRed( palette[MENUPAL], 1 );
            while(sounds_in_queue!=0);
			break;
		}
		else if( menustate==STOPGAME )
		{
			FadeOut( palette[MENUPAL], 5 );
			gameinprogress=FALSE;
			hide_mouse();
			continue;
		}

		FadeOut( palette[MENUPAL], 5 );
		hide_mouse();
		Clear(0);

		_scrn = (unsigned char far *)0xA0000000L;
		load_pic( _scrn, PIC8_FOFFSET );

		draw_board( board );
		PutBlock( 237, 16, sprite[PLAYERX_GFX].W, sprite[PLAYERX_GFX].H, sp_image[PLAYERX_GFX] );
		PutBlock( 237, 38, sprite[PLAYERO_GFX].W, sprite[PLAYERO_GFX].H, sp_image[PLAYERO_GFX] );

		if(allsound)
			PutBlock(233,145,sprite[RADIOON].W,sprite[RADIOON].H,sp_image[RADIOON]);
		else
			PutBlock(233,145,sprite[RADIOOFF].W,sprite[RADIOOFF].H,sp_image[RADIOOFF]);

		if(show_avail)
			PutBlock(233,155,sprite[RADIOON].W,sprite[RADIOON].H,sp_image[RADIOON]);
		else
			PutBlock(233,155,sprite[RADIOOFF].W,sprite[RADIOOFF].H,sp_image[RADIOOFF]);

		Text( 86,6,cell_name[board_level],16,0);
		Text( 85,5,cell_name[board_level],28,0);
#ifdef DEBUG
      displayfreemem();
#endif
		checkpieces( board, player );
		show_mouse();
		FadeIn( palette[GAMEPAL], 5 );
		rndcnt = (rand() % 10);
		if(master_snd_sys&&rndcnt>=7)
		{
			while(sounds_in_queue!=0);
            v_delay( 15 );
			playsound(rafdata[STARTSND], raflength[STARTSND]);
		}
		flush_kbd();
		player = do_game( board, player );
		FadeOut( palette[GAMEPAL], 5 );
		hide_mouse();
	}
    v_delay( 10 );
	FadeOut( palette[MENUPAL], 3 );
	hide_mouse();
	_scrn = (unsigned char far *)0xA0000000L;
	load_pic( _scrn, PIC9_FOFFSET );
	FadeIn( palette[GAMEPAL], 5 );
    v_delay( 60 );

#ifdef  DEBUG
   free_memory();
   displayfreemem();
   getch();
#endif

	FadeOut( palette[GAMEPAL], 5 );
	if(master_snd_sys) Dropdead_Varmint();                     // Release Varmint's interrupt
	SetVgaMode( TEXTRES ); // Reset old graphics mode
	free_memory();
    printf("Thank-you for playing Assimilation!\n");
    if (registered == FALSE)
        printf("\tPlease Register!");
	flush_kbd();
}
//**********************[  END OF MAIN  ]************************


void computers_turn( board_t board, int player )
{
	int i, from_x, from_y, to_x, to_y;

	gameinprogress = TRUE;
	depth = curplayerdepth;
	while(sounds_in_queue!=0);
	hide_mouse();

	i = checkpieces( board, player );     //check to see if there is a move available.
	if(i!=NULL)    //if not give control back to other player.
		return;

    v_delay( 10 );
	play(board, 0, player);    //calculate move

	from_x = best_from_x[0];
	from_y = best_from_y[0];
	to_x = best_to_x[0];
	to_y = best_to_y[0];

	put_avail_moves( board, player, from_x, from_y );

    v_delay( 15 );

	if ((ABS(from_x-to_x)==1&&ABS(from_y-to_y)==1)||(ABS(from_x-to_x)==0&&ABS(from_y-to_y)==1)||(ABS(from_x-to_x)==1&&ABS(from_y-to_y)==0))
	{
		put_piece( player, to_x, to_y );
		if(allsound) playsound(rafdata[MOVESND], raflength[MOVESND]);
		while(sounds_in_queue!=0);
	}
	else if ((ABS(from_x-to_x) == 2 && ABS(from_y-to_y) == 2) || (ABS(from_x-to_x)==2 &&ABS(from_y-to_y) == 0) || (ABS(from_x-to_x)==0 &&ABS(from_y-to_y) == 2))
	{
		put_piece( player, to_x, to_y );
		board[from_x][from_y] = EMPTY;
		PutSprite( boardx[from_x], boardy[from_y], sprite[TILE1].W, sprite[TILE1].H, sp_image[TILE1] );
		if(allsound) playsound(rafdata[JUMPSND], raflength[JUMPSND]);
		while(sounds_in_queue!=0);
	}

	if(board[to_x][to_y]==SETBOMB)
	{
		i = rand() % 3;
		if(i==0)
		{
			shootout( boardx[to_x], boardy[to_y], player );
			board[to_x][to_y] = TOMBED;
		}
		else if(i==1)
		{
			explode( boardx[to_x], boardy[to_y] );
			board[to_x][to_y] = BOMBED;
		}
		else
		{
			flushit( boardx[to_x], boardy[to_y], player );
			board[to_x][to_y] = FLUSHED;
		}
		bombs_left--;

		while(sounds_in_queue!=0);
		draw_board( board );
		show_mouse();
		return;
	}
	else
		board[to_x][to_y] = player;

    // DELAY BEFORE CAPTURE PIECES
    v_delay( 10 );
	capture( board, player, to_x, to_y );
	draw_board( board );
	while(sounds_in_queue!=0);
	show_mouse();
}


int players_turn( board_t board, int player )
{
	int nx, ny, i, operation, from_x, from_y, to_x, to_y, xx, yy, buttons;
	
	operation=0;

// LOOP UNTIL HUMAN PLAYER DOES A VALID MOVE OR BUTTON PRESS
	while(operation==0)  //while no operations have been made...
	{
		check_mouse( &mousex, &mousey, &mousebut );

		i = check_kbd();
		if(i==1)
		{
			hide_mouse();
			PutBlock( 277, 178, sprite[MENUBTN].W, sprite[MENUBTN].H, sp_image[MENUBTN] );
			show_mouse();
			if(master_snd_sys) playsound( rafdata[HITMAINSND], raflength[HITMAINSND] );
			operation = 2;    //set to 2 for go back to menu.
		}
		else if(i==9)
			hint( board, player );

		if( mousebut == 1)
		{
			xx = (mousex/2);     // DIVIDE MOUSE X / 2
			yy = mousey;

			if((xx>279&&xx<310)&&(yy>180&&yy<191))    // IF USER HIT MENU BUTTON
			{
				hide_mouse();
				PutBlock( 277, 178, sprite[MENUBTN].W, sprite[MENUBTN].H, sp_image[MENUBTN] );
				show_mouse();
				if(master_snd_sys) playsound( rafdata[HITMAINSND], raflength[HITMAINSND] );
				operation = 2;
				continue;
			}
			else if((xx>228&&xx<255)&&(yy>180&&yy<191))
			{
				hint( board, player );
			}
			else if((xx>230&&xx<308)&&(yy>145&&yy<151))
			{
				toggle_sound();
                v_delay( 10 );
			}
			else if((xx>230&&xx<308)&&(yy>155&&yy<161))
			{
				toggle_show_moves();
                v_delay( 10 );
			}
			else if ((xx>boardx[0]) && (xx<(boardx[7]+sprite[TILE1].W))&&(yy>boardy[0]) && (yy<(boardy[7]+sprite[TILE1].H)))
			{
				from_x = calc_x( xx );
				from_y = calc_y( yy );
				
				if(board[from_x][from_y]==player)
				{
					hide_mouse();
					put_avail_moves( board, player, from_x, from_y );
                    v_delay( 12 );
					show_mouse();

					mousebut = 0;
					while(mousebut!=1)
					{
						check_mouse( &mousex, &mousey, &mousebut );
						check_kbd();
					}

					to_x = calc_x( mousex/2 );
					to_y = calc_y( mousey );
					
					if(to_x>=BSIZE||to_y>=BSIZE)
					{
						to_x = 0;
						to_y = 0;
						operation = 0;
						draw_board( board );
						continue;      //selected a different piece, start over.
					}

					if( board[to_x][to_y]==NA || board[to_x][to_y]==BOMBED || board[to_x][to_y]==TOMBED || board[to_x][to_y]==FLUSHED || board[to_x][to_y]==OTHER(player))
					{
						if(allsound) playsound(rafdata[INVALIDSND], raflength[INVALIDSND]);

						draw_board( board );
						while(sounds_in_queue!=0);
						continue;      //invalid tiles, start over at top.
					}
					else if( board[to_x][to_y]==player)
					{
						draw_board( board );
						continue;      //selected a different piece, start over.
					}

					// CHECK IF PLAYER MOVED 1 SQUARE AWAY

					if(board[to_x][to_y]==EMPTY||board[to_x][to_y]==SETBOMB)
					{
						if ((ABS(from_x-to_x)==1&&ABS(from_y-to_y)==1)||(ABS(from_x-to_x)==0&&ABS(from_y-to_y)==1)||(ABS(from_x-to_x)==1&&ABS(from_y-to_y)==0))
						{  
							hide_mouse();
							operation = 1; //move was made, switch players
							put_piece( player, to_x, to_y );
							if(allsound) playsound(rafdata[MOVESND], raflength[MOVESND]);
							while(sounds_in_queue!=0);
						}
						else if ((ABS(from_x-to_x) == 2 && ABS(from_y-to_y) == 2) || (ABS(from_x-to_x)==2 &&ABS(from_y-to_y) == 0) || (ABS(from_x-to_x)==0 &&ABS(from_y-to_y) == 2))
						{
							hide_mouse();
							board[from_x][from_y] = EMPTY;
							PutSprite( boardx[from_x], boardy[from_y], sprite[TILE1].W, sprite[TILE1].H, sp_image[TILE1] );
							operation = 1; //move was made, switch players
							put_piece( player, to_x, to_y );
							if(allsound) playsound(rafdata[JUMPSND], raflength[JUMPSND]);
							while(sounds_in_queue!=0);
						}
						else
						{
							draw_board( board );
							continue;   //invalid destination, start over.
						}
					}
					else
					{
						draw_board( board );
						continue;   //invalid destination, start over.
					}
						
					if(board[to_x][to_y]==SETBOMB)
					{
						i = rand() % 2;
						if(i==0)
						{
							shootout( boardx[to_x], boardy[to_y], player );
							board[to_x][to_y] = TOMBED;
						}
						else if(i==1)
						{
							explode( boardx[to_x], boardy[to_y] );
							board[to_x][to_y] = BOMBED;
						}
						else
						{
							flushit( boardx[to_x], boardy[to_y], player );
							board[to_x][to_y] = FLUSHED;
						}

						bombs_left--;
						while(sounds_in_queue!=0);
						draw_board( board );
						show_mouse();
						continue;
					}
					else
						board[to_x][to_y] = player;
					
					gameinprogress = TRUE;
                    // DELAY BEFORE CAPTURE PIECES
                    v_delay( 10 );
                    capture( board, player, to_x, to_y );
					draw_board( board );
					while(sounds_in_queue!=0);
					show_mouse();
				}// IF(PIECE==PLAYER)
				else if(board[from_x][from_y]==OTHER(player))
				{
					if(allsound) playsound(rafdata[INVALIDSND], raflength[INVALIDSND]);
					while(sounds_in_queue!=0);    //invalid sound play
					continue;
				}
			}// IF USER HIT ICONS OR BOARD
		}// IF(BUTTONS==1)
	}// while( no valid move );
	return( operation );
}

int do_game( board_t board, int player)
{
	int xx, yy, buttons, x, y, from_x, from_y, to_x, to_y, i, nx, ny;
	int gameover = FALSE;

	if(player==PLAYERX)     //restore players type and depth for resume game.
	{
		curplayertype=playerXtype;
		curplayerdepth=playerXdepth;
	}
	else
	{
		curplayertype=playerOtype;
		curplayerdepth=playerOdepth;
	}

	while(gameover!=TRUE)      // QUIT
	{
		if(curplayertype==COMPUTER)
		{
			brain = rand() % 2;
			computers_turn( board, player );
		}
		else if(curplayertype==HUMAN)
		{
			i = players_turn( board, player );
			if(i==2)
				gameover=TRUE;
		}

		if(gameover!=TRUE)     //if game still going on, switch players.
			player = switch_players(player);

		i = checkpieces( board, player );    //check if game over, update score...
		if(i!=NULL)
		{
			gameover = TRUE;
			gameinprogress = FALSE;
			continue;
		}

									  //returns TRUE if mainmenu is selected.
		i = check_kbd();       //check in between turns for key presses
		if(i==TRUE)
		{
			hide_mouse();
			PutBlock( 277, 178, sprite[MENUBTN].W, sprite[MENUBTN].H, sp_image[MENUBTN] );
			show_mouse();
			if(master_snd_sys) playsound( rafdata[HITMAINSND], raflength[HITMAINSND] );
			gameover = TRUE;
		}
	}// WHILE(gameover!=TRUE)
	return player;
}

int hint( board_t board, int player )
{
	int from_x, from_y, to_x, to_y, i;

	hide_mouse();

	if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
	PutBlock( 226, 178, sprite[HINTBTN].W, sprite[HINTBTN].H, sp_image[HINTBTN+1] );

    hintselected=TRUE;
	play(board, 0, player);    //calculate move
    hintsused++;

	from_x = best_from_x[0];
	from_y = best_from_y[0];
	to_x = best_to_x[0];
	to_y = best_to_y[0];

	for(i=0;i<5;i++)
	{
		if(player==PLAYERX)
			PutBlock( boardx[from_x], boardy[from_y], sprite[BLUESELECTED].W, sprite[BLUESELECTED].H, sp_image[BLUESELECTED] );
		else
			PutBlock( boardx[from_x], boardy[from_y], sprite[REDSELECTED].W, sprite[REDSELECTED].H, sp_image[REDSELECTED] );
		PutBlock( boardx[to_x], boardy[to_y], sprite[TILEAVAIL].W, sprite[TILEAVAIL].H, sp_image[TILEAVAIL] );
        v_delay( 8 );
		
		if(player==PLAYERX)
			PutBlock( boardx[from_x], boardy[from_y], sprite[PLAYERX_GFX].W, sprite[PLAYERX_GFX].H, sp_image[PLAYERX_GFX] );
		else
			PutBlock( boardx[from_x], boardy[from_y], sprite[PLAYERO_GFX].W, sprite[PLAYERO_GFX].H, sp_image[PLAYERO_GFX] );
		PutBlock( boardx[to_x], boardy[to_y], sprite[TILE1].W, sprite[TILE1].H, sp_image[TILE1] );
        v_delay( 8 );
	}
	PutBlock( 226, 178, sprite[HINTBTN].W, sprite[HINTBTN].H, sp_image[HINTBTN] );
	show_mouse();
    hintselected=FALSE;
	return (0);
}

int switch_players( int player )
{
	if(player == PLAYERX)      // SWITCH PLAYERS
	{
		curplayertype = playerOtype;
		curplayerdepth = playerOdepth;
	}
	else
	{
		curplayertype = playerXtype;
		curplayerdepth = playerXdepth;
	}
	player = OTHER(player);
	return(player);
}

int check_kbd( void )
{
	char ch;

	if(kbhit())
	{
  		ch=getkey();
		if(ch==LM)
			return(1);
		else if(ch==LS)
			toggle_sound();
		else if(ch==LO)
			toggle_show_moves();
		else if(ch==LH&&curplayertype==HUMAN)
			return(9);
	}
	return(0);
}

void capture( board_t board, int player, int to_x, int to_y)
{
	int i,nx,ny,total;
	
	total=0;

	for (i=0; i<BSIZE; i++)
	{
		if(brain==0)
		{
			nx = to_x+aix[i];
			ny = to_y+aiy[i];
		}
		else
		{
			nx = to_x+bix[i];
			ny = to_y+biy[i];
		}

		if (ON_BOARD(nx, ny) && board[nx][ny] == OTHER(player))
		{
			total++;

			board[nx][ny] = player;
				if(sounds_in_queue==0&&allsound) playsound(rafdata[CAPTURESND+player-1], raflength[CAPTURESND+player-1]);

			if(player==PLAYERX)
				PutSprite( boardx[nx], boardy[ny], sprite[PLAYERX_GFX].W, sprite[PLAYERX_GFX].H, sp_image[PLAYERX_GFX] );
			else
				PutSprite( boardx[nx], boardy[ny], sprite[PLAYERO_GFX].W, sprite[PLAYERO_GFX].H, sp_image[PLAYERO_GFX] );
		}
	}
	if(allsound)
	{
		while(sounds_in_queue!=0);
        v_delay( 10 );
		if(total==8)
		{
			if(curplayertype==COMPUTER)
				playsound( rafdata[COMP8SND], raflength[COMP8SND] );
			else
				playsound( rafdata[PLAY8SND], raflength[PLAY8SND] );
		}
		else if(total>=6)
		{
			if(curplayertype==COMPUTER)
				playsound( rafdata[COMP7SND], raflength[COMP7SND] );
			else
				playsound( rafdata[PLAY7SND], raflength[PLAY7SND] );
		}
	}
}

void intro( void )
{
	Black_Pal();
	load_pic( _scrn+(35*320), PIC1_FOFFSET );
	FadeIn( palette[TITLEPAL], 10 );
    v_delay( 60 );
    if(!kbhit())
        v_delay( 85 );
  	load_pic( _scrn+(180*320), PIC2_FOFFSET );
    v_delay( 30 );
    if(!kbhit())
        v_delay( 120 );
  	FadeOut( palette[TITLEPAL], 10 );
	Clear(0);
	load_pic( _scrn+(40*320), PIC3_FOFFSET );
	if(master_snd_sys) playsound( rafdata[INTROSND], raflength[INTROSND] );
	SetPal( palette[TITLE2PAL] );
    v_delay( 60 );
	while(sounds_in_queue!=0);
    if(!kbhit())
        v_delay( 85 );
    SetPal( palette[WHITEPAL] );
    v_delay( 8 );
	FadeOut( palette[WHITEPAL], 5 );
}

int mainmenu( void )
{
	int x, y, menustate;
	char ch, valid, scroll;
	
	Black_Pal();
	_scrn = (unsigned char far *)0xA0000000L;
	load_pic( _scrn, PIC4_FOFFSET );
	update_menu_ops();
	display_cells();
	if(gameinprogress==TRUE)
	{
		PutBlock( 124,132,sprite[RESUMEGAMEGFX].W,sprite[RESUMEGAMEGFX].H,sp_image[RESUMEGAMEGFX] );
		PutSprite( 124,113,sprite[STOPBTNOUT].W,sprite[STOPBTNOUT].H,sp_image[STOPBTNOUT] );
		PutSprite( 281,115, sprite[OPSCOVER].W, sprite[OPSCOVER].H, sp_image[OPSCOVER] );
		PutSprite( 281, 66, sprite[OPSCOVER].W, sprite[OPSCOVER].H, sp_image[OPSCOVER] );
        //PutSprite( 113,174, sprite[OPSCOVER].W, sprite[OPSCOVER].H, sp_image[OPSCOVER] );
        //PutSprite( 127,174, sprite[OPSCOVER].W, sprite[OPSCOVER].H, sp_image[OPSCOVER] );
        //PutSprite( 133,174, sprite[OPSCOVER].W, sprite[OPSCOVER].H, sp_image[OPSCOVER] );
	}
    if(registered==FALSE)
    {
        PutSprite( 50,88, sprite[XPIC].W,sprite[XPIC].H,sp_image[XPIC]);
        PutSprite( 50,160, sprite[XPIC].W,sprite[XPIC].H,sp_image[XPIC]);

        PutSprite( 119,177, sprite[XPIC].W,sprite[XPIC].H,sp_image[XPIC]);
    }

	show_mouse();
	FadeIn( palette[MENUPAL], 5 );
	flush_kbd();

	menustate = 0;
	while(1)
	{
		valid=FALSE;
		scroll=FALSE;
		if(kbhit())
		{
            //ch=getkey();
            ch = getch();
            if(ch=='n')
			{
				menustate = STARTNEWGAME;
				hide_mouse();
				PutSprite(124,56,sprite[NEWBTN].W,sprite[NEWBTN].H, sp_image[NEWBTN] );
				if(master_snd_sys) playsound(rafdata[NEWGAMESND], raflength[NEWGAMESND]);
				show_mouse();
				break;
			}
            else if(ch=='q')
			{
				menustate = EXITTODOS;
				hide_mouse();
				PutBlock(174,168,sprite[QUITBTN].W,sprite[QUITBTN].H, sp_image[QUITBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
            else if(ch=='r'&&gameinprogress==TRUE)
			{
				menustate=RESUMEGAME;
				hide_mouse();
				PutSprite(124,132,sprite[RESBTN].W,sprite[RESBTN].H, sp_image[RESBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				cell=board_level;
				break;
			}
            else if(ch=='s'&&gameinprogress==TRUE)
			{
				menustate=STOPGAME;
				hide_mouse();
				PutSprite(124,113,sprite[STOPBTNIN].W,sprite[STOPBTNIN].H, sp_image[STOPBTNIN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
            else if(ch=='o')
			{
				menustate = INFO;
				hide_mouse();
				PutSprite(124,75,sprite[INFOBTN].W,sprite[INFOBTN].H, sp_image[INFOBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
            else if(ch=='d')
			{
				menustate = CREDITS;
				hide_mouse();
				PutSprite(124,94,sprite[CREDBTN].W,sprite[CREDBTN].H, sp_image[CREDBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
            else if(ch=='h')
			{
				playerXtype=HUMAN;
				valid=TRUE;
			}
            else if(ch=='m')
			{
				playerXtype=COMPUTER;
                playerXdepth=1;
				valid=TRUE;
			}
            else if(ch=='i')
			{
				playerXtype=COMPUTER;
                playerXdepth=2;
				valid=TRUE;
			}
            else if(ch=='c'&&registered == TRUE)
			{
				playerXtype=COMPUTER;
                playerXdepth=3;
				valid=TRUE;
			}
            else if(ch=='a')
			{
				playerOtype=HUMAN;
				valid=TRUE;
			}
            else if(ch=='1')
			{
				playerOtype=COMPUTER;
                playerOdepth=1;
				valid=TRUE;
			}
            else if(ch=='2')
			{
				playerOtype=COMPUTER;
                playerOdepth=2;
				valid=TRUE;
			}
            else if(ch=='3'&&registered == TRUE)
			{
				playerOtype=COMPUTER;
                playerOdepth=3;
				valid=TRUE;
			}
            else if(ch=='k'&&gameinprogress==FALSE&&registered == TRUE)
			{
				killers=!killers;
				valid=TRUE;
			}
			else if(ch==UP&&gameinprogress==FALSE) //ARROW UP
			{
				if((cell-1)>0)
				{
					cell--;
					scroll=TRUE;
				}
			}
			else if(ch==DOWN&&gameinprogress==FALSE) //ARROW DOWN
			{
                if((cell+1)<maxboardscanuse+1)
				{
					cell++;
					scroll=TRUE;
				}
			}
			else if(ch==PGUP&&gameinprogress==FALSE) //PAGE UP
			{
				if((cell-10)>0)
				{
					cell-=10;
					scroll=TRUE;
				}
				else if(cell!=1)
				{
					cell=1;
					scroll=TRUE;
				}
			}
			else if(ch==PGDOWN&&gameinprogress==FALSE) //PAGE DOWN
			{
                if((cell+10)<maxboardscanuse+1)
				{
					cell+=10;
					scroll=TRUE;
				}
                else if(cell!=maxboardscanuse)
				{
                    cell=maxboardscanuse;
					scroll=TRUE;
				}
			}
			else if(ch==HOME&&gameinprogress==FALSE) //HOME
			{
				if(cell!=1)
				{
					cell=1;
					scroll=TRUE;
				}
			}
			else if(ch==END&&gameinprogress==FALSE) //END
			{
                if(cell!=maxboardscanuse)
				{
                    cell=maxboardscanuse;
					scroll=TRUE;
				}
			}
		}

		check_mouse( &mousex, &mousey, &mousebut );

		if(mousebut==1)
		{
			x = (mousex/2);
			y = mousey;

			if((x>175&&y>169) && (x<224&&y<177))
			{
				menustate = EXITTODOS;
				hide_mouse();
				PutBlock(174,168,sprite[QUITBTN].W,sprite[QUITBTN].H, sp_image[QUITBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
			else if((x>125&&y>57) && (x<215&&y<68))
			{
				menustate = STARTNEWGAME;
				hide_mouse();
				PutSprite(124,56,sprite[NEWBTN].W,sprite[NEWBTN].H, sp_image[NEWBTN] );
				if(master_snd_sys) playsound(rafdata[NEWGAMESND], raflength[NEWGAMESND]);
				show_mouse();
				break;
			}
			else if((x>125&&y>76) && (x<215&&y<87))
			{
				menustate = INFO;
				hide_mouse();
				PutSprite(124,75,sprite[INFOBTN].W,sprite[INFOBTN].H, sp_image[INFOBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
			else if((x>125&&y>95) && (x<215&&y<106))
			{
				menustate = CREDITS;
				hide_mouse();
				PutSprite(124,94,sprite[CREDBTN].W,sprite[CREDBTN].H, sp_image[CREDBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
			else if((x>125&&y>133) && (x<215&&y<144) && (gameinprogress==TRUE))
			{
				menustate=RESUMEGAME;
				hide_mouse();
				PutSprite(124,132,sprite[RESBTN].W,sprite[RESBTN].H, sp_image[RESBTN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				cell=board_level;
				break;
			}
			else if(x>42&&x<83)
			{
				if(y>56&&y<62)
				{
					playerXtype=HUMAN;
					valid=TRUE;
				}
				else if(y>67&&y<73)
				{
					playerXtype=COMPUTER;
                    playerXdepth=1;
					valid=TRUE;
				}
				else if(y>78&&y<84)
				{
					playerXtype=COMPUTER;
                    playerXdepth=2;
					valid=TRUE;
				}
                else if(y>89&&y<95&&registered == TRUE)
				{
					playerXtype=COMPUTER;
                    playerXdepth=3;
					valid=TRUE;
				}
				else if(y>128&&y<134)
				{
					playerOtype=HUMAN;
					valid=TRUE;
				}
				else if(y>139&&y<145)
				{
					playerOtype=COMPUTER;
                    playerOdepth=1;
					valid=TRUE;
				}
				else if(y>150&&y<156)
				{
					playerOtype=COMPUTER;
                    playerOdepth=2;
					valid=TRUE;
				}
                else if(y>161&&y<167&&registered == TRUE)
				{
					playerOtype=COMPUTER;
                    playerOdepth=3;
					valid=TRUE;
				}
			}
			else if(x>125&&x<215&&y>114&&y<125&&gameinprogress==TRUE)
			{
				menustate=STOPGAME;
				hide_mouse();
				PutSprite(124,113,sprite[STOPBTNIN].W,sprite[STOPBTNIN].H, sp_image[STOPBTNIN] );
				if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
				show_mouse();
				break;
			}
            else if(x>112&&x<148&&y>179&&y<185&&gameinprogress==FALSE&&registered==TRUE)
			{
				killers=!killers;
				valid=TRUE;
			}
			else if(x>283&&x<293&&y>67&&y<79&&gameinprogress==FALSE)
			{
				if((cell-1)>0)
				{
					cell--;
					scroll=TRUE;
				}
			}
			else if(x>283&&x<293&&y>116&&y<128&&gameinprogress==FALSE)
			{
                if((cell+1)<maxboardscanuse+1)
				{
					cell++;
					scroll=TRUE;
				}
			}
		}
		if(valid==TRUE)
		{
			if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
			hide_mouse();
			PutBlock( 24, 53, sprite[MENUBUF1].W, sprite[MENUBUF1].H, sp_image[MENUBUF1] );
			PutBlock( 24,125, sprite[MENUBUF2].W, sprite[MENUBUF2].H, sp_image[MENUBUF2] );
			update_menu_ops();
			show_mouse();
			while(sounds_in_queue!=0);
            v_delay( 10 );
			flush_kbd();
		}
		if(scroll==TRUE)
		{
			display_cells();
			if(master_snd_sys) playsound(rafdata[DULLTHUDSND], raflength[DULLTHUDSND]);
            v_delay( 12 );
			while(sounds_in_queue!=0);
		}
	}
	return( menustate );
}

void display_cells( void )
{
	hide_mouse();
	Text(260,85,cell_name[cell-1],5,16);
	Text(260,95,cell_name[cell],1,16);
	Text(260,105,cell_name[cell+1],5,16);
	show_mouse();
}

void free_memory( void )
{
	int i;

	farfree( boardin );
   for(i=0;i<MAXSPRITES;i++)
		farfree( sp_image[i] );
   for(i=0;i<MAXRAFS;i++)
      farfree( rafdata[i] );
   for(i=0;i<MAXPALETTES;i++)
		farfree( palette[i] );

}

void read_res( void )
{
	unsigned int i,k,j;
	unsigned char numsprites, numpalettes, x, y, linelen, numboards;
	unsigned long int boardpos;
	unsigned char numrafs, strbuf[80];

	printf("\n\nLoading Data...");
	fseek( resfile, SPRITES_FOFFSET-1, SEEK_SET );
	fread( &numsprites, 1, 1, resfile);
	for(i=0; i<numsprites; i++)
	{
        sp_image[i] = &sprite[i].image[0];
		
		fread(&sprite[i].W, 2, 1, resfile);
		fread(&sprite[i].H, 2, 1, resfile);

		if((sp_image[i] = (char far *) farmalloc((sprite[i].W)*(sprite[i].H)))==NULL)
		{
			printf("\nError loading pics\n");
			exitwerr();
		}
		fread(sp_image[i], sprite[i].W, sprite[i].H, resfile);
	}

//******************* READ IN PALETTES FROM RES FILE ************
	fread( &numpalettes, 1, 1, resfile);
	for(i=0; i<numpalettes; i++)
	{
		if((palette[i] = (unsigned char far *) farmalloc(768))==NULL)
		{
			printf("\nError loading pals\n");
			exitwerr();
		}
		fread( palette[i], 768, 1, resfile);
	}

//**************** LOAD RAF FILES FROM RES FILE ************
   //if(master_snd_sys != SBCARD)
   //   return;

	fseek(resfile, RAFS_FOFFSET-1, SEEK_SET);
	fread(&numrafs, 1, 1, resfile);
	for(i=0; i<numrafs; i++)
	{
		if((rafdata[i] = getraf(&raflength[i]))==NULL)
		{
			printf("\nError loading sounds\n");
			exitwerr();
		}
	}

//**************** READ IN BOARD NAMES ******************
	strcpy(cell_name[0],"<         >");
    strcpy(cell_name[maxboardscanuse+1],cell_name[0]);

    for(i=0;i<maxboardscanuse;i++)
    {
        linelen = fgetc( resfile );
        fread( cell_name[i+1], linelen, 1, resfile );
        decode( cell_name[i+1], cell_name[i+1] );
    }
}

void exitwerr( void )
{
	fclose(resfile);
    v_delay( 20 );
	if(master_snd_sys) Dropdead_Varmint();      // Release Varmint's interrupt
    v_delay( 20 );
	free_memory();
	exit(1);
}

int draw_board( board_t board )
{
	unsigned int clmn, row, x, y;

	hide_mouse();
	row=14;
	for(y=0; y<BSIZE; y++)
	{
		clmn=7;
		for(x=0; x<BSIZE; x++)
		{
			if(board[x][y]==NA)
				PutBlock( clmn, row, sprite[TILENA].W, sprite[TILENA].H, sp_image[TILENA] );
			else if(board[x][y]==PLAYERO)
				PutBlock( clmn, row, sprite[PLAYERO_GFX].W, sprite[PLAYERO_GFX].H, sp_image[PLAYERO_GFX] );
			else if(board[x][y]==PLAYERX)
				PutBlock( clmn, row, sprite[PLAYERX_GFX].W, sprite[PLAYERX_GFX].H, sp_image[PLAYERX_GFX] );
			else if(board[x][y]==EMPTY||board[x][y]==SETBOMB)
				PutBlock( clmn, row, sprite[TILE1].W, sprite[TILE1].H, sp_image[TILE1] );
			else if(board[x][y]==BOMBED)
				PutBlock( clmn, row, sprite[CRATER].W, sprite[CRATER].H, sp_image[CRATER] );
			else if(board[x][y]==TOMBED)
				PutBlock( clmn, row, sprite[RIP+5].W, sprite[RIP+5].H, sp_image[RIP+5] );
			else if(board[x][y]==FLUSHED)
				PutBlock( clmn, row, sprite[DRAIN].W, sprite[DRAIN].H, sp_image[DRAIN] );

			boardx[x]=clmn;
			clmn+=sprite[TILE1].W;
		}
		boardy[y]=row;
		row+=sprite[TILE1].H;
	}
	show_mouse();
	return;
}

int build_board( board_t board, int whichboard )
{
	int x, y, i,j;
	char ch;

	farfree(boardin);
	boardin = getboard( whichboard );

	for (y=0; y<BSIZE; y++)
	{
		for (x=0; x<BSIZE; x++)
		{
			ch = *(boardin)++;
			if( ch == '#')
				board[x][y]=EMPTY;
			else if (ch=='O')
				board[x][y]=PLAYERO;
			else if (ch=='X')
				board[x][y]=PLAYERX;
			else if (ch=='*')
				board[x][y]=NA;
		}
	}
	bombs_left=0;
	if(killers==ON)
	{
		j = rand() % 3 + 1;
		for(i=0;i<j;i++)
		{
			while(board[x=rand() % BSIZE][y=rand() % BSIZE]!=EMPTY);
			board[x][y]=SETBOMB;
		}
		bombs_left=j;
	}
	return 0;
}

int checkpieces( board_t board, int player )
{
	int playerXpieces,playerOpieces,x,y,d,boardtotal,avail_move,newx,newy;
    char string2[10],string3[25];
	char totalstr[22]={"COMPLETE ASSIMILATION"};

	playerXpieces=0;
	playerOpieces=0;
	boardtotal=BSIZE*BSIZE;
	for (x=0; x<BSIZE; x++)
	{
		for (y=0; y<BSIZE; y++)
		{
			if(board[x][y]==PLAYERX)
				playerXpieces++;
			else if(board[x][y]==PLAYERO)
				playerOpieces++;
			else if(board[x][y]==NA)
				boardtotal--;
			else if(board[x][y]==BOMBED)
				boardtotal--;
		}
	}
	display_digits( playerXpieces, 273, 22 );
	display_digits( playerOpieces, 273, 44 );
	
	avail_move = FALSE;
	for (x=0; x<BSIZE; x++)
	{
		for (y=0; y<BSIZE; y++)
		{
			if (board[x][y] == player)
			{
				for (d=0; d<16; d++)
				{
					newx=x+aix[d];
					newy=y+aiy[d];

					if (ON_BOARD(newx, newy)) 
					{
						if(board[newx][newy]==EMPTY || board[newx][newy]==SETBOMB)
						{
							avail_move = TRUE;
						}
					}
				}
			}
		}
	}
	
	if((playerXpieces+playerOpieces)>=boardtotal||playerXpieces==0||playerOpieces==0||avail_move!=TRUE)
	{
        v_delay( 30 );
		if(master_snd_sys) playsound( rafdata[GONGSND], raflength[GONGSND] );
		hide_mouse();
        v_delay( 15 );

		for (x=0; x<BSIZE; x++)
		{
			for (y=0; y<BSIZE; y++)
			{
				if(board[x][y]==SETBOMB)
					PutBlock( boardx[x], boardy[y], sprite[BOMBPIC].W, sprite[BOMBPIC].H, sp_image[BOMBPIC]);
			}
		}
		while(sounds_in_queue!=0);
        v_delay( 40 );
		spot_screen();
		sprite[MSGBOX].X = 160-(sprite[MSGBOX].W / 2);
		sprite[MSGBOX].Y = 100-(sprite[MSGBOX].H / 2);
		PutBlock( sprite[MSGBOX].X, sprite[MSGBOX].Y , sprite[MSGBOX].W, sprite[MSGBOX].H, sp_image[MSGBOX] );
        v_delay( 30 );
		door_open(110, 100);

		if(playerXpieces>playerOpieces)
		{
			PutBlock(110,100,sprite[BLUEDOOR].W,sprite[BLUEDOOR].H,sp_image[BLUEDOOR]);
			PutBlock( 150,105,sprite[WONGFX].W,sprite[WONGFX].H,sp_image[WONGFX]);
			sprintf (string2,"%d - %d", playerXpieces,playerOpieces);
		}
		else if(playerOpieces>playerXpieces)
		{
			PutBlock(110,100,sprite[REDDOOR].W,sprite[REDDOOR].H,sp_image[REDDOOR]);
			PutBlock( 150,105,sprite[WONGFX].W,sprite[WONGFX].H,sp_image[WONGFX]);
			sprintf (string2,"%d - %d", playerOpieces,playerXpieces);
		}
		else
		{
			PutBlock(110,100,sprite[BLUEDOOR].W,sprite[BLUEDOOR].H,sp_image[BLUEDOOR]);
			PutBlock( 150,105,sprite[TIEDGFX].W,sprite[TIEDGFX].H,sp_image[TIEDGFX]);
			sprintf (string2,"");
			PutBlock(190,100,sprite[REDDOOR].W,sprite[REDDOOR].H,sp_image[REDDOOR]);
		}

		Text( 191, 108, string2, 16, 0 );
		Text( 190, 107, string2, 15, 0 );

        if (hintsused!=0&&curplayertype!=HUMAN)
        {
            sprintf(string3,"HINTS USED: %d",hintsused);
            Text( 129, 127, string3, 16, 0 );
            Text( 130, 128, string3, 10, 0 );
        }

        if(playerXpieces==0||playerOpieces==0)
        {
            if (hintsused!=0&&curplayertype!=HUMAN)
            {
                Text(113,90,totalstr,0,0);
                Text(112,89,totalstr,15,0);
            }
            else
            {
                Text(113,130,totalstr,0,0);
                Text(112,129,totalstr,15,0);
            }

            if(master_snd_sys)
            {
                playsound( rafdata[TOTALSND], raflength[TOTALSND]);
                while(sounds_in_queue!=0);
            }
        }

		if(master_snd_sys)
		{

			if(playerXpieces>playerOpieces&&playerXtype==COMPUTER&&playerOtype==HUMAN)
				playsound(rafdata[LAUGHSND], raflength[LAUGHSND]);
			else if(playerOpieces>playerXpieces&&playerOtype==COMPUTER&&playerXtype==HUMAN)
				playsound(rafdata[LAUGHSND], raflength[LAUGHSND]);
			else if(playerOtype==HUMAN&&playerXtype==HUMAN||playerOtype==COMPUTER&&playerXtype==COMPUTER)
				playsound(rafdata[YEAHSND], raflength[YEAHSND]);
			else if(playerXpieces==playerOpieces)
				playsound(rafdata[YEAHSND], raflength[YEAHSND]);
			else
				playsound(rafdata[YEAHSND], raflength[YEAHSND]);
		}
        v_delay( 20 );
		checkforhit();
		door_close(110, 100);
        v_delay( 30 );
		show_mouse();
		return( !NULL );
	}
	else
		return NULL;
}

void display_digits( int number, int x, int y )
{
	char string[3];
	int digit1,digit2;

	sprintf(string,"%2d",number);
	digit2=atoi( &string[1] );    //ones column
	string[1]='\0';
	digit1=atoi( &string[0] );    //tens column

    PutBlock( x, y, sprite[NUMBERS+digit1].W, sprite[NUMBERS+digit1].H, sp_image[NUMBERS+digit1] );
    PutBlock( x+12, y, sprite[NUMBERS+digit2].W, sprite[NUMBERS+digit2].H, sp_image[NUMBERS+digit2] );
	PutBlock( 285, 77, sprite[NUMLEFT].W, sprite[NUMLEFT].H, sp_image[NUMLEFT+bombs_left] );
}

void put_avail_moves( board_t board, int player, int from_x, int from_y )
{
	int i,nx,ny;
	
	if(player==PLAYERX)
		PutBlock( boardx[from_x], boardy[from_y], sprite[BLUESELECTED].W, sprite[BLUESELECTED].H, sp_image[BLUESELECTED] );
	else
		PutBlock( boardx[from_x], boardy[from_y], sprite[REDSELECTED].W, sprite[REDSELECTED].H, sp_image[REDSELECTED] );

	if(allsound) playsound(rafdata[CLICKONSND], raflength[CLICKONSND]);

	if(show_avail==ON)
	{
		for (i=0; i<16; i++)
		{
			if(brain==0)
			{
				nx = from_x+aix[i];
				ny = from_y+aiy[i];
			}
			else
			{
				nx = from_x+bix[i];
				ny = from_y+biy[i];
			}
			
			if (ON_BOARD(nx, ny))
			{
				if(board[nx][ny] == EMPTY || board[nx][ny] == SETBOMB)
					PutBlock( boardx[nx], boardy[ny], sprite[TILEAVAIL].W, sprite[TILEAVAIL].H, sp_image[TILEAVAIL] );
			}
		}
	}
}

int calc_x( int xx )
{                   
	int r;

	for(r=0;r<BSIZE;r++)
	{
		if(xx<(boardx[r]+sprite[TILE1].W))
			break;
	}
	return( r );
}

int calc_y( int yy )
{
	int d;

	for(d=0;d<BSIZE;d++)
	{
		if(yy<(boardy[d]+sprite[TILE1].H))
			break;
	}
	return( d );
}

void put_piece( int player, int to_x, int to_y )
{
	if(player==PLAYERX)
		PutSprite( boardx[to_x], boardy[to_y], sprite[PLAYERX_GFX].W, sprite[PLAYERX_GFX].H, sp_image[PLAYERX_GFX] );
	else
		PutSprite( boardx[to_x], boardy[to_y], sprite[PLAYERO_GFX].W, sprite[PLAYERO_GFX].H, sp_image[PLAYERO_GFX] );
}

void toggle_sound( void )
{
	if(master_snd_sys)
	{
		hide_mouse();
		if(allsound)
			PutBlock( 233, 145, sprite[RADIOOFF].W, sprite[RADIOOFF].H, sp_image[RADIOOFF] );
		else
			PutBlock( 233, 145, sprite[RADIOON].W, sprite[RADIOON].H, sp_image[RADIOON] );
							
		show_mouse();
		allsound=!allsound;
	}
}

void toggle_show_moves( void )
{
	hide_mouse();
	if(show_avail)
		PutBlock( 233, 155, sprite[RADIOOFF].W, sprite[RADIOOFF].H, sp_image[RADIOOFF] );
	else
		PutBlock( 233, 155, sprite[RADIOON].W, sprite[RADIOON].H, sp_image[RADIOON] );
  
	show_mouse();
	show_avail=!show_avail;
}

void startup( void )
{
	unsigned int long totalfreemem;
	char message[]={"Assimilation requires a "};

	clrscr();
    textcolor(WHITE);
    textbackground(BLUE);
    if(registered==TRUE)
        cprintf("ASSIMILATION (C) V1.4 Registered Version                                      \r\n");
    else
        cprintf("ASSIMILATION (C) V1.4 UnRegistered Version                                      \r\n");
    printf("Copyright (C) 1995,1996   All Rights Reserved\n\n");

	if(!getvideotype())
	{
		printf("%sVGA card!\n", message);
		exit(1);
	}
    printf("VGA Detected.\n");

	if(!detect386())
	{
      printf("%s386+ processor!\n",message);
		exit(1);
	}
    printf("80386+ detected.\n");

	if(!mouse_reset())
	{
		printf("%sMOUSE to be installed!\n",message);
		exit(1);
	}
    printf("Mouse detected.\n");

	totalfreemem = (farcoreleft()+USAGESIZE)/1024L;
    if(totalfreemem<550)
	{
      printf("%smin. of 550K of memory free.  %dK Free!\n",message,totalfreemem);
		exit(1);
	}
    printf("%dK free memory detected.\n", totalfreemem);
    if(registered==TRUE)
        printf("Serial Number...Verified!\n");
    else
        printf("Serial Number...Invalid!\n");

    v_delay(60);
	if(SB_Setup())
	{
		Go_Varmint();                            // Install  Varmint's tools
		master_snd_sys = SBCARD;
		allsound=ON;
	}
	else
	{
		printf("\nNo Sound Blaster Found! (Is SETUP correct?)");
		master_snd_sys = NOCARD;
		allsound=OFF;
	}
    v_delay(30);
}

void spot_screen( void )
{
	int x,y;
	
	for(y=0;y<200;y+=2)
	{
		for(x=0;x<320;x+=2)
		{
            _scrn = VIDMEM + (y<<8) + (y<<6) + x;
			*_scrn = 0;
		}
	}
}

void explode( int x, int y )
{
	int i;
	
	if(allsound) playsound(rafdata[EXPLODESND], raflength[EXPLODESND]);

	for(i=0;i<6;i++)
	{
        PutBlock(x,y,sprite[EXPLODE1].W, sprite[EXPLODE1].H,sp_image[EXPLODE1+i] );
        v_delay( 6 );
	}
	for(i=5;i>=2;i--)
	{
        PutBlock( x, y, sprite[EXPLODE1].W, sprite[EXPLODE1].H,sp_image[i+EXPLODE1] );
        v_delay( 7 );
	}
	PutBlock( x, y, sprite[CRATER].W, sprite[CRATER].H, sp_image[CRATER] );
}

void door_open( int x, int y )
{
	int i;

	for(i=0;i<7;i++)
	{
        PutBlock(x,y,sprite[DOOR1].W, sprite[DOOR1].H,sp_image[i+DOOR1] );
        v_delay( 5 );
	}
}

void door_close( int x, int y )
{
	int i;

	for(i=6;i>=0;i--)
	{
        PutBlock( x, y, sprite[DOOR1].W, sprite[DOOR1].H,sp_image[i+DOOR1] );
        v_delay( 5 );
	}
}

SAMPLE *getraf( unsigned long int *length)
{
  int i;
  SAMPLE  *data;

  fread(length,1,4,resfile);                  // length of data
  data = (SAMPLE *)farmalloc(*length+1);  // allocate memory for data
  if(!data)                                 // oops.  Not enough mem!
  {
	 return(NULL);
  }

  fread(data,1,*length,resfile);              // read the data
  for(i = 0; i < *length; i++)             // convert to signed format
  {
	 *(data + i) = ((BYTE)*(data + i))-128;
  }

  return(data);
}

SAMPLE *getboard( int board_level )
{
	SAMPLE  *data;
	unsigned long int boardpos;
	unsigned int x,y;

	boardpos = BOARDS_FOFFSET+((board_level-1)*66);
	fseek(resfile, boardpos, SEEK_SET);

	fread(&x,1,1,resfile);                  // length of data
	fread(&y,1,1,resfile);
	if((data = (SAMPLE *)farmalloc(64))==NULL)  // allocate memory for data
	{
		return(NULL);
	}
	fread(data,8,8,resfile);              // read the data
	decode( data, data );

	return(data);
}

void load_pic( unsigned char far * buffer, unsigned long int picpos )
{
	unsigned int x,y;
	
	fseek(resfile, picpos, SEEK_SET);
	fread(&x,2,1,resfile);                  // length of data
	fread(&y,2,1,resfile);

	fread(buffer, x, y, resfile);              // read the data
}

void update_menu_ops( void )
{
	if(killers==ON)
		PutBlock(99,174,sprite[CHECKON].W,sprite[CHECKON].H,sp_image[CHECKON]);
	else
		PutBlock(99,174,sprite[CHECKOFF].W,sprite[CHECKOFF].H,sp_image[CHECKOFF]);

	if(playerXtype==HUMAN)
		PutSprite(25,54,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
	else if(playerXtype==COMPUTER)
	{
		if(playerXdepth==1)
			PutSprite(25,65,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
		else if(playerXdepth==2)
			PutSprite(25,76,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
		else if(playerXdepth==3)
			PutSprite(25,87,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
	}

	if(playerOtype==HUMAN)
		PutSprite(25,126,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
	else if(playerOtype==COMPUTER)
	{
		if(playerOdepth==1)
			PutSprite(25,137,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
		else if(playerOdepth==2)
			PutSprite(25,148,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
		else if(playerOdepth==3)
			PutSprite(25,159,sprite[ARROW].W,sprite[ARROW].H,sp_image[ARROW] );
	}
}

#ifdef  DEBUG
void displayfreemem( void )
{
	char message[12];
	sprintf(message, "mem: %dK",farcoreleft()/1024L); 
	Text(10,6,message,16,0);
	Text(9,5,message,15,0);
}
#endif

void checkforhit(void)
{
	flush_kbd();
	mousebut=0;
	while(!kbhit()&&mousebut==0)
		check_mouse( &mousex, &mousey, &mousebut );
	if(kbhit()) getkey();
}

void flush_kbd( void )
{
	while(kbhit()) getkey();
}

void decode( unsigned char * stringdst, unsigned char * stringsrc )
{
	int i, l;

	l=strlen(stringsrc);     //-1 for <CR>

	for(i=0;i<l;i++)
	{
		*stringdst++=*stringsrc++-127;
	}
}

void shootout( int x, int y, int player )
{
	int i;

	for(i=0;i<3;i++)
	{
		if(allsound)
		{
			if(i<2)
				playsound( rafdata[SHOT1SND], raflength[SHOT1SND] );
			else
				playsound( rafdata[SHOT2SND], raflength[SHOT2SND] );
		}

		if(player==PLAYERX)
			PutBlock(x,y,sprite[BLUEHIT].W, sprite[BLUEHIT].H,sp_image[BLUEHIT+i] );
		else if(player==PLAYERO)
			PutBlock(x,y,sprite[REDHIT].W, sprite[REDHIT].H,sp_image[REDHIT+i] );
		if(allsound)
			while(sounds_in_queue!=0);
		else
            v_delay( 30 );
	}
    v_delay( 20 );
	for(i=0;i<6;i++)
	{
		PutBlock( x, y, sprite[RIP].W, sprite[RIP].H, sp_image[RIP+i] );
        v_delay( 10 );
	}
}

void flushit( int x, int y, int player )
{
	int i, j;

	if(allsound) playsound( rafdata[FLUSHSND], raflength[FLUSHSND] );
	PutBlock( x, y, sprite[BLUEGOING].W, sprite[BLUEGOING].H, sp_image[BLUEGOING+player-1] );

	for(j=0;j<5;j++)
	{
		for(i=0;i<4;i++)
		{
            v_delay( 10 );
			if(player==PLAYERX)
				PutBlock( x, y, sprite[BLUEDRAIN].W, sprite[BLUEDRAIN].H, sp_image[BLUEDRAIN+i] );
			else
				PutBlock( x, y, sprite[REDDRAIN].W, sprite[REDDRAIN].H, sp_image[REDDRAIN+i] );
		}
	}
	for(i=0;i<4;i++)
	{
        v_delay( 10 );
		if(player==PLAYERX)
			PutBlock( x, y, sprite[BLUEFLUSH].W, sprite[BLUEFLUSH].H, sp_image[BLUEFLUSH+i] );
		else
			PutBlock( x, y, sprite[REDFLUSH].W, sprite[REDFLUSH].H, sp_image[REDFLUSH+i] );
	}
}

int getkey( void )
{
	union REGS regs;

	regs.h.ah=0;
	int86(0x16,&regs,&regs);
	//return(regs.h.al);    //return ASCII code
	return(regs.h.ah);   //return BIOS scan code
}

void v_delay( int times )
{
   int i;

   for(i=0;i<times;i++)
       Wait_For_Vsync();
}

void check_serial_number( void )
{
    FILE *regin;
    unsigned long int serial_number=0;
	char sn_str[10];
    int i;

    if((regin = fopen("assimil8.dat","rb"))==NULL)
	{
        printf("\nERROR: Couldn't open assimil8.dat!\n");
		exit( 1 );
	}

    fread(&serial_number, 4, 1, regin);
    fclose(regin);
	ultoa(serial_number, sn_str, 10);

    //long code, but will throw off the unwise hacker when
    //they are looking through the executable
    if( sn_str[0] == '3' || sn_str[0] == '7')
        if( sn_str[1] == '2' || sn_str[1] == '4')
            if( sn_str[2] == '5' || sn_str[2] == '9')
                if( sn_str[3] == '8' || sn_str[3] == '6')
                    if( sn_str[4] == '0' || sn_str[4] == '9')
                        if( sn_str[5] == '9' || sn_str[5] == '2')
                            if( sn_str[6] == '7' || sn_str[6] == '3')
                                if( sn_str[7] == '1' || sn_str[7] == '3')
                                    if( sn_str[8] == '4' || sn_str[8] == '1')
                                    {
                                        registered = TRUE;
                                    }
                                    else
                                    {
                                        registered = FALSE;
                                        maxboardscanuse = MAXBOARDS-40;
                                    }
}

/***************************************
 * Find the best move for player "player" on the given board
 ***************************************/
int far play(board_t board, int lev, int player)
{
     int x, y, d, i, newx, newy, e, nrpt, stupidity;
	 board_t board2;
	 board_t repeat[BSIZE*BSIZE];

	 best_eval[lev] = -32767;
     stupidity=0;
	 nrpt = 0;

	 for (x=0; x<BSIZE; x++)
	 {
          for (y=0; y<BSIZE; y++)
          {
				if (board[x][y] != player)
				{
					continue;
				}

				/*
				 * We found one of our pieces -- examine all possible moves for it
				 */
				for (d=0; d<16; d++)
				{
					if(brain==0)
					{
						newx=x+aix[d];
						newy=y+aiy[d];
					}
					else
					{
						newx=x+bix[d];
						newy=y+biy[d];
					}

					if (!ON_BOARD(newx, newy) || board[newx][newy]!=EMPTY && board[newx][newy]!=SETBOMB)
					{
						continue;
					}

				// Another valid play, try it out
					COPY_BOARD(board2, board);
					do_move(board2, x, y, newx, newy);

				// Check for duplicate board position
					if (d < 8)
					{
						for (i=0; i<nrpt; i++)
						{
							if (SAME_BOARD(board2, repeat[i])) break;
						}
						if (i < nrpt)
						{
							continue;
						}
						COPY_BOARD(repeat[nrpt++], board2);
					}

				// Play next level, or evaluate current board position
					if (lev+1 < depth)
						e = -play(board2, lev+1, OTHER(player));
					else
                        e = eval(board2, player);

                    if(!hintselected)
                    {
                        if(lev==0)
                            stupidity = level1[rand()%16];
                        else if(lev==1)
                            stupidity = level2[rand()%16];
                        else if(lev==2)
                            stupidity = level3[rand()%16];
                    }

				// If this is the best move so far, remember it
                    if (e > best_eval[lev]+stupidity)
					{
						best_from_x[lev]=x;
						best_from_y[lev]=y;
						best_to_x[lev]=newx;
						best_to_y[lev]=newy;

#ifdef ALPHA_BETA_PRUNING
						if (lev > 0 && -e <= best_eval[lev-1]) return e;
#endif
                        best_eval[lev] = e;
						COPY_BOARD(best_play[lev][lev], board2);
						for (i=lev+1; i<depth; i++)
							COPY_BOARD(best_play[lev][i], best_play[lev+1][i]);
					}
				}
			}
		}

	 // If no valid moves were found, pass and evaluate
	 if (best_eval[lev] == -32767)
	 {
		  COPY_BOARD(board2, board);

		  if (lev+1 < depth)
				e = -play(board2, lev+1, OTHER(player));
		  else
				e = eval(board2, player);

		  if (e > best_eval[lev])
		  {
				best_eval[lev] = e;
				COPY_BOARD(best_play[lev][lev], board2);
				for (i=lev+1; i<depth; i++)
					 COPY_BOARD(best_play[lev][i], best_play[lev+1][i]);
		  }
	 }

	 return (best_eval[lev]);
}


/***************************************
 * Score the given board
 ***************************************/
int far eval(board_t board, int player)
{
	 int x, y, e, me, him, max;

	 // Count the number of pieces that each player has on the board
	 me = 0;
	 him = 0;
	 max = BSIZE*BSIZE;
	 for (x=0; x<BSIZE; x++)
	 {
		  for (y=0; y<BSIZE; y++)
		  {
				if (board[x][y] == NA)
					max--;
				else if(board[x][y]==BOMBED)
					max--;
				else if (board[x][y] == player)
					me++;
				else if (board[x][y] == OTHER(player))
					him++;
		  }
	 }

	 // If the board is not full, the score is the difference in counts.
	 // If the board is full, offset by +/- 1000 to indicate win/loss.
	 
	 if (me+him < max)
	 {
          e = me-him;
	 }
	 else
	 {
		  if (me > him)
				e = 1000 + me-him;
		  else
				e = -1000 + me-him;
	 }

	 return e;
}


/***************************************
 * Apply a given move to the board
 ***************************************/
int far do_move(board_t board, int from_x, int from_y, int to_x, int to_y)
{
	int player, d, nx, ny;
	 
	player = board[from_x][from_y];
	board[to_x][to_y] = player;

	if (ABS(from_x-to_x) == 2 || ABS(from_y-to_y) == 2)
		board[from_x][from_y] = EMPTY;
	 
	for (d=0; d<8; d++)
	{
		if(brain==0)
		{
			nx = to_x+aix[d];
			ny = to_y+aiy[d];
		}
		else
		{
			nx = to_x+bix[d];
			ny = to_y+biy[d];
		}

		if (ON_BOARD(nx, ny) && board[nx][ny] == OTHER(player))
			board[nx][ny] = player;
	}
	return;
}
