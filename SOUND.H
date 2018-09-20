										 // More definitions than you can
										 // shake a stick at.
										 // Pointer type definition for samples.
										 // This definition should be far or
										 // huge.  If you use near, you will have
										 // to change the code to use malloc()
										 // instead of farmalloc()
#define SAMPLE signed char far

#define MAXSOUNDS 3

#define WORD unsigned int
#define DWORD unsigned long int
#define BYTE unsigned char
#define FALSE 0
#define TRUE 1

#define DSP_RESET 6
#define DSP_READ 0x0a
#define DSP_WRITE 0x0c
#define DSP_WSTATUS 0x0c
#define DSP_RSTATUS 0x0e

#define FM_DETECT 1
#define DSP_DETECT 2

#define DSP_DMA_8BIT 0x14
#define DSP_DMA_ADC 0x24
#define DSP_SAMPLE_RATE 0x40

#define DSP_SILENCE 0x80
#define DSP_SPKR_ON 0xd1
#define DSP_SPKR_OFF 0xd3
#define DSP_GET_VERS 0xe1

#define DSP_INVOKE_INTR 0xf2

#define FM_BOTH_OFF  0x168

typedef enum {bit8=0,bit4=1,bit26=2,bit2=3,high} playmode;
typedef enum {none,nodsp,adlib,sb20,sbpro} cardtype;
typedef enum {fmerr=0,nodsperr,irqerr,dmacherr,nomem} SBERROR;

void SetRate(WORD rate);
int SB_Setup(void);
											  // These functions Are not really needed
											  // outside of sound.c


/* timerx prototypes */

void timer_on(void);
WORD timer_off(void);
WORD to_micro(WORD clk);
void clkdelay(WORD clicks);
void mdelay(WORD delay);
WORD mcalc(WORD micro);
void _saveregs msdelay(WORD delay);


/*  Stuff added by Eric Jorgensen */
													// function prototypes
void playsound(SAMPLE *data,DWORD length);
void Go_Varmint(void);
void Dropdead_Varmint(void);

														// Globals
														// The variables athat are
														// commented out should
														// probably stay that way since
														// They are only used internally.
extern char *errname[];
//extern DWORD vclock;
extern SBERROR sberr;
extern int sounds_in_queue;
extern WORD dma_bufferlen;
