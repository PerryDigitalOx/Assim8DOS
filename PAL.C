/*******************************************************************************
 PAL.C
 These routines are used to manipulate a palette.
 Routines to fade in and out, set R,G,B palettes.
********************************************************************************/
#include <mem.h>  // for memset
#include "game386.h"  //needed for SetPal
#include "sound.h"

static char far Pal[768];

/*******************************************************************************
 Sets the active palette to all black.  Useful for drawing a screen and then
 fading in later.
*******************************************************************************/
void far Black_Pal( void )
{
	memset( Pal, 0, 768 );        // sets all of temp Pal = 0  (black)
	SetPal( Pal );                // set palette using temp Pal (black)
}

/*******************************************************************************
 Fades in from black to the specified palette.
*******************************************************************************/
void far FadeIn( char far * palette , int speed )
{
	int i, j, v;

	for( i = 0; i < 32; i++ )
	{
		msdelay( speed );
		for( j = 0; j < 768; j++ )
			Pal[j] =  (i * (unsigned)palette[j]) >> 5;
			SetPal( Pal );
	}
}

/*******************************************************************************
 Fades to black from a specified palette.
*******************************************************************************/
void far FadeOut( char far * palette , int speed )
{
	int i, j, v;

	for( i = 31; i >= 0; i-- )
	{
		msdelay( speed );
		for( j = 0; j < 768; j++ )
			Pal[j] =  (i * (unsigned)palette[j]) >> 5;
		SetPal( Pal );
	}
}

/*******************************************************************************
 Fades to Red and back again from a specified palette.
*******************************************************************************/
void FadeToRed( char far * palette, int speed )
{
	int j, i;

	for( i=31; i>=0; i--)
	{  
		for( j=0; j < 768; j+=3 )
		{
			Pal[j+1] =  (i * (unsigned)palette[j+1]) >> 5;
			Pal[j+2] =  (i * (unsigned)palette[j+2]) >> 5;
		}
		SetPal( Pal );
	}

	for( i=0; i<32; i++)
	{
		msdelay( speed+15 );
		for( j=0; j < 768; j+=3 )
		{
			Pal[j+1] =  (i * (unsigned)palette[j+1]) >> 5;
			Pal[j+2] =  (i * (unsigned)palette[j+2]) >> 5;
		}
		SetPal( Pal );
	}
}

/*
//***************************************************************************
// Changes palette to RED.
//***************************************************************************
void RGB2Red( char * palette )
{
	int j;

	for( j = 0; j < 768; j+=3 )
	{
		Pal[j+1] =  ((unsigned)palette[j]) >> 5;
		Pal[j+2] =  ((unsigned)palette[j]) >> 5;
	}
	SetPal( Pal );
}
//***************************************************************************
// Changes palette to GREEN.
//***************************************************************************
void RGB2Green( char * palette )
{
	int j;

	for( j = 0; j < 768; j+=3 )
	{
		Pal[j] =  ((unsigned)palette[j]) >> 5;
		Pal[j+2] =  ((unsigned)palette[j]) >> 5;
	}

	SetPal( Pal );
}
//***************************************************************************
// Changes palette to Blue.
//***************************************************************************
void RGB2Blue( char * palette )
{
	int j;

	for( j = 0; j < 768; j+=3 )
	{
		Pal[j] =  ((unsigned)palette[j]) >> 5;
		Pal[j+1] =  ((unsigned)palette[j]) >> 5;
	}

	SetPal( Pal );
}


//***************************************************************************
// Fades specified palette darker.
//***************************************************************************
void Fade_Darker( char * palette , int amount , int speed)
{
	int i, j;
	int v = 0;

	if( amount>31 ) v = 31;
	v=31-amount;

	for( i = 31; i >= v; i-- )
	{
		msdelay( speed );
		for( j = 0; j < 768; j++ )
			Pal[j] =  (i * (unsigned)palette[j]) >> 5;
		SetPal( Pal );
	}
}


/*******************************************************************************
 Fades in only a part of the palette.
*******************************************************************************
PartFadeIn( int first, int num, char far* palette )
{
	int i, j, v;

	for( i = 0; i < 32; i++ )
	{
		for( j = 0; j < num * 3; j++ )
			Pal[j] =  (i * (unsigned)palette[j]) >> 5;
		SetPartPal( first, num, Pal );
	}
	return;
}

/*******************************************************************************
 Fades out only a part of the palette.
*******************************************************************************
PartFadeOut( int first, int num, char far* palette )
{
	int i, j, v;

	for( i = 31; i >= 0; i-- )
	{
		for( j = 0; j < num * 3; j++ )
			Pal[j] =  (i * (unsigned)palette[j]) >> 5;
		SetPartPal( first, num, Pal );
	}
	return;
}

/*******************************************************************************
 Cycles 'num' palette entries, starting at the 'first' entry.  Entries are
 cycled to the "left".
*******************************************************************************
PartCycleL( int first, int num, char far* palette )
{
	int i, a, b, c;

	num--;
	a = palette[0];
	b = palette[1];
	c = palette[2];
	for( i = 0; i < num * 3; i++ )
		palette[i] = palette[i + 3];
	palette[i] = a;
	palette[i + 1] = b;
	palette[i + 2] = c;
	SetPartPal( first, num + 1, palette );
	return;
}

/*******************************************************************************
 Cycles 'num' palette entries, starting at the 'first' entry.  Entries are
 cycled to the "right".
*******************************************************************************
PartCycleR( int first, int num, char far* palette )
{
	int i, a, b, c;

	i = (num * 3) - 1;
	a = palette[i - 2];
	b = palette[i - 1];
	c = palette[i];
	for( ; i >= 3; i-- )
		palette[i] = palette[i - 3];
	palette[0] = a;
	palette[1] = b;
	palette[2] = c;
	SetPartPal( first, num, palette );
	return;
}
*/
