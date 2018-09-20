#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>
#include <ctype.h>

#include "sound.h"

#define WORD unsigned int
#define BYTE unsigned char

static cardtype CheckHard(void);
static int test_int(void);
static int scan_int(void);
int DSP_Reset(void);
BYTE DSP_Read(void);
void DSP_Write(BYTE output);
WORD DSP_GetVersion(void);
void SB_SetVect(void);
void SB_RemoveVect(void);
int get_sb_env(void);
int CardCheck(void);
cardtype WhichCard(void);
BYTE int2vect(BYTE intnr);
void enable_int(BYTE nr);
void disable_int(BYTE nr);
void InitT2(void);
void measure(void);
void dma_set(BYTE far *sound_address,WORD len,BYTE channel);
WORD polldma(BYTE channel);

static void far interrupt (*orgint)(void) = NULL;
static void far interrupt (*orgirqint)(void) = NULL;

static WORD ioaddr[6]={0x220,0x240,0x210,0x230,0x250,0x260};
static BYTE intrx[5]={7,5,2,3,10};

WORD io_addr,intnr,dma_ch;
static WORD mue3,mue23,dsp_vers,wh_card;

volatile BYTE tst_cnt;

static WORD dma_adr[8]= {0x00,0x02,0x04,0x06,0xc0,0xc4,0xc8,0xcc};
static WORD dma_len[8]= {0x01,0x03,0x05,0x07,0xc2,0xc6,0xca,0xce};
static WORD dma_page[8]={0x87,0x83,0x81,0x82,0x8f,0x8b,0x89,0x8a};
WORD dma_bufferlen = 200;
BYTE far *dma_buffer = NULL;
SBERROR sberr = 0;
char *errname[] = {
  "No DSP2",
  "No DSP",
  "No open IRQ",
  "No open DMA channel",
  "No DMA buffer memory"};

static WORD timer_val,timer_hold,timer_diff,mue999;
//static WORD timadd,timsum;
int sounds_in_queue = 0;
int sample_rate = 11000;

SAMPLE *sounddata[MAXSOUNDS];
DWORD soundpos[MAXSOUNDS];


/**************************************************************************
  void Go_Varmint(void)
  void Dropdead_Varmint(void)

  DESCRIPTION:  Starts/stops the interrupt routine for Varmint's audio tools

**************************************************************************/
void Go_Varmint(void)
{
  SB_SetVect();                         // Install the looper
  DSP_Write(0x48);                       // Set DSP for 8bit DMA
  DSP_Write((dma_bufferlen) & 0xff);  // Write length low byte
  DSP_Write((dma_bufferlen) >> 8);    // Write length high byte

  dma_set(dma_buffer,dma_bufferlen,dma_ch);
  DSP_Write(0x1C);                       // Set DSP for Autoinit 8bit DMA

  DSP_Write(DSP_INVOKE_INTR);           // Ignition!
}

void Dropdead_Varmint(void)
{
  DSP_Write(0xD0);                      // Halt DMA
  SB_RemoveVect();
}


/*  ---------------  DSP  Stuff ------------ */


/**************************************************************************
  int DSP_Reset()

  DESCRIPTION: Resets the DSP

**************************************************************************/
int DSP_Reset(void)
{
  int i;

  asm mov dx,io_addr;                   // load address for dsp reset
  asm add dx,DSP_RESET;
  asm mov al,1
  asm out dx,al;                        // Send a 1 to the DSP

  mdelay(mcalc(mue3));                  // Wait three micro seconds

  asm mov dx,io_addr;                   // load address again
  asm add dx,DSP_RESET;
  asm mov al,0
  asm out dx,al;                        // send a 0 to the DSP

  for(i=0;i<50;i++)                     // DSP should send back an 0xaa
  {
	 mdelay(mcalc(mue3));
	 if(DSP_Read()==0xaa) return(TRUE);
  }

  return(FALSE);
}


/**************************************************************************
  BYTE DSP_Read()

  DESCRIPTION:  reads a byte from the dsp


**************************************************************************/
BYTE DSP_Read(void)
{
  asm mov dx,io_addr;               // load address to read status byte
  asm add dx,DSP_RSTATUS;
loop:
  asm in al,dx;                     // read until high bit is set
  asm test al,0x80;
  asm jz loop

  asm mov dx,io_addr;               // read byte from output address
  asm add dx,DSP_READ;
  asm in al,dx;
  return(_AL);
}


/**************************************************************************
  void DSP_Write(BYTE output)

  DESCRIPTION: Writes a byte to the DSP

**************************************************************************/
void DSP_Write(BYTE output)
{
  asm  mov dx,io_addr;               // dx holds the port address
  asm  add dx,DSP_WSTATUS;
loop:                                // wait for bit 7 to clear
  asm    in al,dx;
  asm    test al,0x80;
  asm    jnz loop
												 // write our bute!
  asm  mov dx,io_addr;
  asm  add dx,DSP_WRITE;
  asm  mov al,output
  asm  out dx,al;

}

/**************************************************************************
  int get_sb_env()

  DESCRIPTION:  Get sound blaster information from the environment
					 variable "BLASTER"

**************************************************************************/
int get_sb_env(void)
{
	FILE *setupfile;
	char setupstring[20];
	char *str;
	int i;

	if ((setupfile = fopen("SETUP.CFG", "rt")) == NULL)
		return(FALSE);
		
	fgets(setupstring, 20, setupfile);
	str=&setupstring[0];
	
	fclose(setupfile);
														// Convert string to upper case
	for(i = 0 ; i < strlen(str); i++) *(str+i) = toupper(*(str+i));
														// pick apart variable for info.
														// Io address
	for(i = 0; *(str+i) != 0 && *(str + i) != 'A'; i++);
	if(*(str+i) == 0) return(FALSE);
	sscanf(str+i+1,"%x",&io_addr);
	if(io_addr<0x210 || io_addr>0x260) return (FALSE);

														// IRQ port number (?)
	for(i = 0; *(str+i) != 0 && *(str + i) != 'I'; i++);
	if(*(str+i) == 0) return(FALSE);
	sscanf(str+i+1,"%d",&intnr);
	if(intnr < 2  || intnr > 10) return (FALSE);

														// Dma channel number
	for(i = 0; *(str+i) != 0 && *(str + i) != 'D'; i++);
	if(*(str+i) == 0) return(FALSE);
	sscanf(str+i+1,"%d",&dma_ch);
	if(dma_ch > 7) return(FALSE);           // only 0-7 allowed

	printf("\nSound Blaster @ addr: %x  irq: %d  dma: %d ",io_addr,intnr,dma_ch);

	return(TRUE);
}



/**************************************************************************
  WORD DSP_GetVersion()

  DESCRIPTION:  Get the version number of the DSP

**************************************************************************/
WORD DSP_GetVersion(void)
{
  DSP_Write(DSP_GET_VERS);
  return((WORD)DSP_Read()*256+DSP_Read());
}


/*  ---------------  Misc.  Stuff ------------ */



/**************************************************************************
  int CardCheck()

  DESCRIPTION:  Check for both FM chip and DSP

**************************************************************************/
int CardCheck(void)
{
  int ret=0;

//  if(FM_Detect()) ret|=FM_DETECT;
  if(DSP_Reset()) ret|=DSP_DETECT;
  return(ret);
}


/**************************************************************************
  static void far interrupt testn_int()

  DESCRIPTION:  Function stored as an interrupt to test various interrrupt
					 vectors by test_int()

**************************************************************************/
static void far interrupt testn_int(void)
{
  tst_cnt++;

  asm mov dx,io_addr;
  asm add dx,DSP_RSTATUS;
  asm in al,dx;         /* Ack DSP interrupt */

  asm mov al,0x20;
  asm out 0x20,al; /* set EOI */
  asm mov bx,intnr
  asm cmp bx,7
  asm jbe end
  asm out 0xa0,al; /* set EOI */

end:
}



/**************************************************************************
  static int test_int()

  DESCRIPTION:  This function is used by scan_int() to test interrupt
					 stuff.  It installs a test interrupt in the
					 requested spot (intnr) then sees if the DSP can
					 use it.

**************************************************************************/
static int test_int(void)
{
  int i;
  BYTE int1,int2;

  orgint=getvect(int2vect(intnr));

  asm in al,0x21;   /* save org master intr settings */
  asm mov int1,al;

  asm in al,0xa1;   /* save org slave intr settings */
  asm mov int2,al;

  asm mov al,0xfe;
  asm cli;
  asm out 0x21,al;  /* disable ALL intr (except timer) */
  asm mov al,0xff
  asm out 0xa1,al;
  asm sti;

  tst_cnt=0;                   // reset our test interrupt counter.

  enable_int(intnr);           // put in our test interrupt
  setvect(int2vect(intnr),testn_int);

  DSP_Write(DSP_INVOKE_INTR);  /* still magic -- make DSP interrupt? */

  for(i=0;i<30000;i++) if(tst_cnt) break;

  asm cli;
  asm mov al,int1;
  asm out 0x21,al;  /* restore org master intr */

  asm mov al,int2;  /* restore org slave intr */
  asm out 0xa1,al;
  asm sti;
  setvect(int2vect(intnr),orgint);

  if(i==30000) return(FALSE);
  else return(TRUE);
}


/**************************************************************************
  static int scan_int()

  DESCRIPTION: This makes sure that the interrupt number picked by the
					 IRQ specification is a good choice.

**************************************************************************/
static int scan_int(void)
{
  int i;

  if(test_int()) return(intnr);   // Original choice good?

  for(i=0;i<5;i++)                // Try our five best guesses
  {
	 intnr=intrx[i];
	 if(test_int()) return(i);
  }
  return(0);
}


/**************************************************************************
  static cardtype CheckHard()

  DESCRIPTION:  Checks hardware for DSP and FM chip

**************************************************************************/
static cardtype CheckHard(void)
{
  int ret;

  ret=DSP_Reset();
  if(ret)
  {
	 if(!scan_int()) {              // Scan IRQ's
		sberr= irqerr;
		return(none);
	 }

/*  fm_addr=io_addr+FM_BOTH_OFF;
	 if(!FM_Detect()) {
		sberr = fmerr;
		return(none);                 // no fm? -> damaged!
	 }

											  // SBPro  checking here.  Not too critical
	 fm_both=fm_addr;
	 fm_addr=io_addr+FM_RIGHT_OFF;
	 fm_right=fm_addr;
	 ret3=FM_Detect();
	 fm_addr=fm_both;

	 if(ret3)
	 {
		wh_card=sbpro;
		fm_left=io_addr+FM_LEFT_OFF;
	 }
	 else
		wh_card=sb20;  */
	 wh_card = sb20;
	 return(wh_card);
  }
  sberr = nodsperr;
  return(none);
}


/**************************************************************************
  cardtype WhichCard()

  DESCRIPTION:  Calls various functions to make sure you've
					 got a Sound Blaster

**************************************************************************/
cardtype WhichCard(void)
{
  cardtype cret;
  int i;

  if(get_sb_env()) cret=CheckHard();    // grab environment variable
  if(cret!=nodsp) return(cret);         // If dsp is there, then go home

  intnr=7;
  for(i=0;i<6;i++)                      // scan around for a better io address
  {
	 io_addr=ioaddr[i];

	 cret=CheckHard();
	 if(cret!=nodsp) return(cret);
  }
  return(none);                         // Uh oh.
}


/**************************************************************************
  int SB_Setup()

  DESCRIPTION: Sets up the sound blaster for action.  This is the only
					function a programmer should really use.  Most of the
					nitty gritty is handled internally.

**************************************************************************/
int SB_Setup(void)
{
  int i;

  InitT2();                      /* init Timer 2 */
  measure();                     /* time loop factor */
  mue3=mcalc(3) ;               /* calc val for 3 micro sec delay */
  mue23=mcalc(23) ;             /* calc val for 23 micro sec delay */

  WhichCard();
  if(wh_card==none) return(FALSE);

  if(wh_card==sb20 || wh_card==sbpro)
  {                             // Get DSP ready
	 dsp_vers=DSP_GetVersion();
	 DSP_Write(DSP_SPKR_ON);
  }

										  // Allocate space for Mixing buffer
  dma_buffer = (BYTE far *)farmalloc(dma_bufferlen+5);
  if(!dma_buffer) {
	 sberr = nomem;
	 return(FALSE);
  }
										  // Clear the buffer
  for(i = 0; i < dma_bufferlen+4; i++) {
	 *(dma_buffer+i) = 0;
  }
  SetRate(11000);               // Set the sample rate

	return(TRUE);
}


/**************************************************************************
  DWORD far2long(char far *adr)

  DESCRIPTION: This is used by dma_set to convert a regular far address
					to a 20 bit flat address.

**************************************************************************/
DWORD far2long(char far *adr)
{
  return(((DWORD)FP_SEG(adr)<<4)+FP_OFF(adr));
}


/**************************************************************************
  void SetRate(WORD rate)

  DESCRIPTION:  Sets the sample rate (specified in hz)

**************************************************************************/
void SetRate(WORD rate)
{
  DWORD val;

  if(rate<4000) return;               // Calculate number for the sound card
  val=256-1000000L/rate;
  DSP_Write(DSP_SAMPLE_RATE);
  DSP_Write((BYTE)val);

  sample_rate = rate;                 // FYI
}

/**************************************************************************
  WORD dma_set(DWORD adrl,WORD len,int channel)

  DESCRIPTION:  This programs the DMA controller to start a single pass
					 output transfer.

					 (Draeden of VLA has provided some good information for
					 DMA programming in his INTRO to DMA document)

**************************************************************************/
void dma_set(BYTE far *sound_address,WORD len,BYTE channel)
{
  WORD adr;
  DWORD adrl;
  BYTE page;

  adrl = far2long(sound_address); // convert address to 20 bit format
  adr=(WORD)adrl;                 // extract page address
  page=(BYTE)(adrl>>16);          // extract page number

											 // PREPARE DMA.
											 //   (Channels 0-3 have different command
											 //    ports than 4-7.)

  if(channel < 4) {               // channels 0-3?
	 asm {
											 // SET THE CHANNEL MASK BIT
		mov  al,04                   // set 3rd bit
		add al,channel
		out 0x0a,al                 // write the channel

		mov al,0
		out 0x0c,al                  // Clear the byte pointer

		mov al,0x58                  // Read mode  (was 48)
		add al,channel
		out 0x0b,al                  // set the mode
	 }
  }
  else {                          // channels 4-7?
	 asm {
											 // SET THE CHANNEL MASK BIT
		mov al,channel              // (no need to set third bit)
		out 0xd4,al                 // write the channel

		mov al,0
		out 0xd8,al                  // Clear the byte pointer

		mov al,0x58                  // Read mode
		add al,channel
		out 0xd6,al                  // set the mode
	 }
  }

											 // OK.  Now the transfer info
											 // WRITE THE ADDRESS OF THE DATA
  _DX = dma_adr[channel];         // Set the address port
  asm {
	 mov ax,adr
	 out dx,al                     // Write address low byte
	 mov al,ah
	 out dx,al                     // write address high byte
  }

  _DX = dma_page[channel];        // Set the page port
  asm {
	 mov al,page
	 out dx,al                      // Write the page byte
  }
											 // WRITE THE LENGTH OF THE DATA
  _DX = dma_len[channel];          // Set the length port
  asm {
	 mov ax,len
	 out dx,al                     // Write length low byte
	 mov al,ah
	 out dx,al                     // write length high byte
  }

											 // WRITE THE PAGE LOCATION OF THE DATA
											 // CLEAR THE CHANNEL MASK BIT
  if(channel < 4) {
	 asm {
		mov al,channel              // (mask bit already clear)
		out 0x0a,al                 // write the channel
	 }
  }
  else {
	 asm {
		mov al,channel
		and al,0x03                  // Clear the mask bit
		out 0x0a,al                 // write the channel
	 }
  }
}


/**************************************************************************
  static void far interrupt sb_int()

  DESCRIPTION:  This is the sound Blaster interrupt that is to be
					 called at the end of DMA transfer.  This software is
					 set up for a continual DMA transfer, so all it does
					 is have the DMA transfer start all over again.

**************************************************************************/
static void far interrupt sb_int(void)
{
  int d,i,j;
  static int dma_pos=0;

											  // SAMPLE MIXER
											  // Write data to the dma buffer
  for(dma_pos = 0; dma_pos < dma_bufferlen+1; dma_pos++) {
	 d = 128;                       // reset data holder

	 for(i = 0; i < sounds_in_queue; i++) {
		d += *(sounddata[i]++);       // accumulate data
		soundpos[i]--;                // move to next byte
		if(!soundpos[i]) {           // Sound done?
											  // Clear the slot and scoot sounds left
		  for(j = i; j < sounds_in_queue;j++) {
			 sounddata[j] = sounddata[j+1];
			 soundpos[j] = soundpos[j+1];
		  }
		  sounds_in_queue--;
		}
	 }
	 if(d>255) d = 255;             // clip the output byte
	 if(d<0) d = 0;

	 *(dma_buffer + dma_pos) = d;   // Write data to the buffer
  }
  *(dma_buffer) = d;               //  This prevents the popping sound
											  // That can happen when the buffer is
											  // suddenly reset

											  // acknowledge DSP interrupt
  asm mov dx,io_addr;
  asm add dx,DSP_RSTATUS;
  asm in al,dx;
											  // reset interrupt
  asm mov al,0x20;
  asm out 0x20,al; /* set EOI */
  asm mov bx,intnr
  asm cmp bx,7
  asm jbe end
  asm out 0xa0,al; /* set EOI */
end:
											  // Varmint's system clock
}




/**************************************************************************
  void SB_SetVect()

  DESCRIPTION:  Installs the DMA interrupt vector.  This makes it so that
					 sb_int() is called whenever a DMA transfer is finished

**************************************************************************/
void SB_SetVect(void)
{
  orgirqint=getvect(int2vect(intnr));
  setvect(int2vect(intnr),sb_int);     /* set vector to our routine */
  enable_int(intnr);                   /* enable sb interrupt */
}


/**************************************************************************
  void SB_RemoveVect()

  DESCRIPTION:  Removes the DMA interrupt vector

**************************************************************************/
void SB_RemoveVect(void)
{
  disable_int(intnr);                     /* disable sb interrupt */
  setvect(int2vect(intnr),orgirqint);      /* restore org intr vector */
}


/* --------------------------------------------------- */
/* timerX routines are following
	These routines are for highly accurate time measurements
*/

/**************************************************************************
  void InitT2()

  DESCRIPTION: Initializes speaker timer for timing operations.

**************************************************************************/
void InitT2(void)
{
  asm in al,0x61   /* no signal on speaker! */
  asm and al,0xfd
  asm or  al,1
  asm out 0x61,al

  asm mov al,0xb4  /* program timer 2 with modus 2 */
  asm out 0x43,al  /* and counter value of 0 (2^16)*/
  asm mov al,0
  asm out 0x42,al
  asm out 0x42,al
}


/**************************************************************************
  void timer_on()

  DESCRIPTION: Turns on timer counter for a time measurement

**************************************************************************/
void timer_on(void)
{
  asm mov al,0x80;  /* latch timer 2 */
  asm out 0x43,al;  /* save value in timer_hold */
  asm in al,0x42;
  asm mov bl,al;
  asm in al,0x42;
  asm mov bh,al;
  asm mov timer_hold,bx;
}


/**************************************************************************
  WORD timer_off()

  DESCRIPTION: Turns off time and reports clicks elapsed.  Note that this
					timer is so quick that it is wraps after only 56
					milliseconds.  If you want to timer longer stuff, I suggest
					using the global variable vclock.  It's tick frequency is
					sample_rate / dma_bufferlen.

**************************************************************************/
WORD timer_off(void)
{
  asm mov al,0x80; /* latch timer 2 */
  asm out 0x43,al;
  asm in al,0x42;
  asm mov ah,al;
  asm in al,0x42;
  asm xchg ah,al;
  asm mov bx,timer_hold;
  asm sub ax,bx;
  asm neg ax;
  asm mov timer_diff,ax; /* calc timer_hold - ax to timer_diff */
  return(_AX);
}


/**************************************************************************
  WORD to_micro(WORD clk)

  DESCRIPTION: Converts clock ticks number to microsecs

**************************************************************************/
WORD to_micro(WORD clk)
{
	 return(clk*838/1000);
}


/**************************************************************************
  void clkdelay(WORD clicks)

  DESCRIPTION: Wait specified number of clock ticks

**************************************************************************/
void clkdelay(WORD clicks)
{
  asm mov al,0x80;  /* latch timer 2 */
  asm out 0x43,al;  /* save value in bx */
  asm in al,0x42;
  asm mov bl,al;
  asm in al,0x42;
  asm mov bh,al;
loop:
  asm mov al,0x80;
  asm out 0x43,al;
  asm in al,0x42;
  asm mov ah,al;
  asm in al,0x42;
  asm xchg ah,al;
  asm sub ax,bx;
  asm neg ax;
  asm cmp ax,clicks;  /* leave routine after click CLK's */
  asm jle loop;
}


/**************************************************************************
  void measure()

  DESCRIPTION: measures a standard delay loop for other delay functions

**************************************************************************/
void measure(void)
{
  timer_on();

  asm cli
  asm mov cx,10000  /* internal test loop */
loop1:
  asm loop loop1

  timer_off();
  asm sti
  timer_val=timer_diff;

  mue999=mcalc(999); /* calc for msdelay */
}


/**************************************************************************
  void mdelay(WORD delay)

  DESCRIPTION: Very tiny delay

**************************************************************************/
void mdelay(WORD delay)
{
  asm mov cx,delay
 loop1:
  asm loop loop1
}


/**************************************************************************
  void _saveregs msdelay(WORD delay)

  DESCRIPTION:  Millisec delay.  When using this library, you should use this
					 delay for millisecond delays instead of the delay
					 functions that comes with turbo C.

**************************************************************************/
void _saveregs msdelay(WORD delay)
{
  WORD i;

  for(i=0;i<delay;i++) mdelay(mue999); /* only 999 cause of fnctn call */
}


/**************************************************************************
  WORD mcalc(WORD micro)

  DESCRIPTION:  Calculates number of ticks to send to mdelay for a specified
  number of microseconds.

**************************************************************************/
WORD mcalc(WORD micro)
{
  return(WORD)((long)micro*10000L/timer_val*1000/838);
}




/********************************
TIMER 0 STUFF
*********************************/
/* --------------------------------

CAUTION: These routines can cause a lot of headaches while debugging.
If you set your own interrupt and then stop the program before you call
Remove_Timer0(), you'd better reboot your computer, because very
unpredictable things will happen if Install_Timer0() is called again.
My suggestion is to get your interrupt working and then comment out the
Timer0 routines until the rest of the program is written and debugged.
												  - Eric  */

/**************************************************************************
  BYTE int2vect(BYTE intnr)

  DESCRIPTION:  This function converts a PIC irq number to a true
					 interrupt vector number.  For PC's with a 286 or greater,
					 irq's 0-7  refer to interrupts 0x08 - 0x0F and
					 irq's 8-15 refer to interrupts 0x70 - 0x77.

**************************************************************************/
BYTE int2vect(BYTE intnr)
{
  if(intnr>7) return(intnr + 0x68);
  else return(intnr+8);
}


/**************************************************************************
  void enable_int(BYTE nr)

  DESCRIPTION:  Enables an IRQ interrupt using the Programmable
					 interrupt controller (PIC)

**************************************************************************/
void enable_int(BYTE nr)
{
  if(nr>7)                     /* use 2nd intr controller? */
  {
	 asm in al,0xa1;           // Read the PIC status from 0xa1
	 asm mov cl,nr;            // load the interrupt number in the counter
	 asm sub cl,8              // subract 8 to get the bit location right
	 asm mov bl,1;             // load a 1 to bl
										/* calc correct mask */
	 asm shl bl,cl;            // Bitshift left bl by cl
	 asm not bl;               // calculate the compliment
	 asm and al,bl;            // leave a hole
	 asm out 0xa1,al;          // write the result to the port
  }
  else
  {
	 asm in al,0x21;
	 asm mov cl,nr;
	 asm mov bl,1;
	 asm shl bl,cl; /* calc correct mask */
	 asm not bl;
	 asm and al,bl;
	 asm out 0x21,al;
  }
}


/**************************************************************************
  void disable_int(BYTE nr)

  DESCRIPTION:  Disables an IRQ interrupt using the Programmable
					 interrupt controller.

**************************************************************************/
void disable_int(BYTE nr)
{
  if(nr>7)
  {
	 asm in al,0xa1;    /* use 2nd intr controller? */
	 asm mov cl,nr;
	 asm sub cl,8
	 asm mov bl,1;
	 asm shl bl,cl;    /* calc correct mask */
	 asm or al,bl;
	 asm out 0xa1,al;
  }
  else
  {
	 asm in al,0x21;
	 asm mov cl,nr;
	 asm mov bl,1;
	 asm shl bl,cl;    /* calc correct mask */
	 asm or al,bl;
	 asm out 0x21,al;
  }
}

/**************************************************************************
  void playsound(SAMPLE *data,length)

  DESCRIPTION: Adds a sound to the play list.  If the playlist is full,
					all the sounds are scooted over and the new sound is  added
					as the last item;

**************************************************************************/
void playsound(SAMPLE *data,DWORD length)
{
  int i;

  if(sounds_in_queue >= MAXSOUNDS)
  {
	 for(i= 0; i <sounds_in_queue; i++)
	 {
		sounddata[i] = sounddata[i+1];
		soundpos[i] = soundpos[i+1];
	 }
	 sounds_in_queue--;
  }
  sounddata[sounds_in_queue] = data;
  soundpos[sounds_in_queue] = length-1;

  sounds_in_queue++;
}

