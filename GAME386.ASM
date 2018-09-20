LOCALS
.MODEL large
.386

; ****************************************************************************
; EQUates
; ****************************************************************************

VIDEO       equ         0A000h                  ; Start of video memory

; ****************************************************************************
; Data Section
; ****************************************************************************
.data

; Where we're going to draw into (defaults to video memory)
_Buffer                 dw  VIDEO

RowOff                  dw      0,  320,  640,  960, 1280, 1600, 1920, 2240
								dw   2560, 2880, 3200, 3520, 3840, 4160, 4480, 4800
								dw   5120, 5440, 5760, 6080, 6400, 6720, 7040, 7360
								dw   7680, 8000, 8320, 8640, 8960, 9280, 9600, 9920
								dw  10240,10560,10880,11200,11520,11840,12160,12480
								dw  12800,13120,13440,13760,14080,14400,14720,15040
								dw  15360,15680,16000,16320,16640,16960,17280,17600
								dw  17920,18240,18560,18880,19200,19520,19840,20160
								dw  20480,20800,21120,21440,21760,22080,22400,22720
								dw  23040,23360,23680,24000,24320,24640,24960,25280
								dw  25600,25920,26240,26560,26880,27200,27520,27840
								dw  28160,28480,28800,29120,29440,29760,30080,30400
								dw  30720,31040,31360,31680,32000,32320,32640,32960
								dw  33280,33600,33920,34240,34560,34880,35200,35520
								dw  35840,36160,36480,36800,37120,37440,37760,38080
								dw  38400,38720,39040,39360,39680,40000,40320,40640
								dw  40960,41280,41600,41920,42240,42560,42880,43200
								dw  43520,43840,44160,44480,44800,45120,45440,45760
								dw  46080,46400,46720,47040,47360,47680,48000,48320
								dw  48640,48960,49280,49600,49920,50240,50560,50880
								dw  51200,51520,51840,52160,52480,52800,53120,53440
								dw  53760,54080,54400,54720,55040,55360,55680,56000
								dw  56320,56640,56960,57280,57600,57920,58240,58560
								dw  58880,59200,59520,59840,60160,60480,60800,61120
								dw  61440,61760,62080,62400,62720,63040,63360,63680

; ****************************************************************************
; Macros
; ****************************************************************************

					  VSYNC macro

					  mov  dx,03DAh
@@1:
					  in   al,dx
					  test al,8
					  jnz  @@1
@@2:
					  in   al,dx
					  test al,8
					  jz   @@2

					  endm

; ****************************************************************************
; Code Section
; ****************************************************************************

.code

;*********************************************************************
; int getvideotype( int * );
; returns the adapter type (1=VGA, 0=other) and number of displayed rows
;*********************************************************************
	  public C getvideotype
getvideotype   PROC far
		push bp
		mov  bp,sp
		push si
		push di

		mov     ax,40h
		mov     es,ax                   ; ES -> video BIOS data area
		mov     al,es:[84h]             ; ROWS
		xor     ah,ah
		inc     ax
		mov     [bx],ax                 ; number of displayed rows
; the following function is supported only by the VGA
		mov     ax,1200h
		mov     bl,36h
		int     10h                     ; enable screen refresh
		cmp     al,12h
		xor     ax,ax                   ; assume not VGA
		jne     return                  ; jump if not supported
		mov     ax,500h
		int     10h                     ; select display page 0
; Select the alternate print-screen routine that works properly if the number
; of displayed rows is not 25.
		mov     ah,12h
		mov     bl,20h
		int     10h                     ; select alternate print-screen
		mov     ax,1                    ; is VGA
return:    

		pop di
		pop si
		pop bp
		ret
getvideotype    ENDP


;===========================================================================
;       Detects 386 or better CPU
;  C-Callable as:   detect386();
;===========================================================================
public C detect386
detect386    PROC far
		push   bp
		mov    bp, sp
		xor    ax, ax
		push   ax
		popf
		pushf
		pop    ax
		and    ax, 0f000h
		cmp    ax, 0f000h
		je     no_386
		mov    ax, 07000h
		push   ax
		popf
		pushf
		pop    ax
		and    ax, 07000h
		je     no_386
		mov    ax, 1
		jmp    exit
no_386:
		xor    ax, ax
exit:
		pop    bp
		ret
detect386    ENDP


;===========================================================================
;       Detects if mouse hardware/software is installed
;  C-Callable as:   mouse_reset();
;===========================================================================
public C mouse_reset
mouse_reset    PROC far
		push   bp
		mov    bp, sp
		push si
		push di

		mov    ax,0000h
		int    33h
		cmp    ax, 0FFFFh
		jne    no_mouse
		mov    ax, 1
		jmp    exit_reset
no_mouse:
		xor    ax, ax
exit_reset:
		 
		pop di
		pop si
		pop    bp
		ret
mouse_reset    ENDP

;===========================================================================
;       Hides Mouse Cursor
;  C-Callable as:   hide_mouse();
;  Multiple calls of hide_mouse() will require multiple calls of show_mouse()
;  to bring the mouse cursor back.
;===========================================================================
public C hide_mouse
hide_mouse     PROC far
		push   bp
		mov    bp, sp
		push si
		push di
		 
		mov    ax,0002h     ;subfunction 2h to hide mouse
		int    33h          ;mouse interrupt
		 
		pop di
		pop si
		pop    bp
		ret
hide_mouse     ENDP

;===========================================================================
;       Shows Mouse Cursor
;  C-Callable as:   show_mouse();
;  Multiple calls of show_mouse() will require multiple calls of hide_mouse()
;  to hide the mouse cursor;
;===========================================================================
public C show_mouse
show_mouse     PROC far
		push   bp
		mov    bp, sp
		push si
		push di
		 
		mov    ax,0001h     ;subfunction 1h to show mouse
		int    33h          ;mouse interrupt
		 
		pop di
		pop si
		pop    bp
		ret
show_mouse     ENDP

;===========================================================================
;       Set Mouse Position
;  C-Callable as:   set_mouse_pos( x, y );
;===========================================================================
public C set_mouse_pos
set_mouse_pos     PROC far
ARG   xpos:WORD,ypos:WORD
		push   bp
		mov    bp, sp
		push si
		push di
		
		mov    ax,0004h     ;subfunction 4h set mouse position
		mov    cx,xpos
		mov    dx,ypos
		int    33h          ;mouse interrupt
			
		pop di
		pop si
		pop    bp
		ret
set_mouse_pos     ENDP


;===========================================================================
;       Set Mouse Range
;  C-Callable as:   set_mouse_range( x1, y1, x2, y2 );
;===========================================================================
public C set_mouse_range
set_mouse_range     PROC far
ARG   x1:WORD,y1:WORD,x2:WORD,y2:WORD
		push   bp
		mov    bp, sp
		push si
		push di

		mov    ax,0007h     ;subfunction 8h horizontal range set
		mov    cx,x1
		mov    dx,x2
		int    33h          ;mouse interrupt
		mov    ax,0008h     ;subfunction 8h vertical range set
		mov    cx,y1
		mov    dx,y2
		int    33h          ;mouse interrupt
		 
		pop di
		pop si
		pop    bp
		ret
set_mouse_range     ENDP

;===========================================================================
; setvgamode(int mode)
;   Set VGA adapter to BIOS mode MODE
;===========================================================================
public C SetVgaMode

SetVgaMode PROC far
ARG      mode:WORD
		push     bp              ; Save BP
		mov      bp,sp           ; Set up stack pointer
		push si
		push di

		mov      ax,mode         ; AX = video mode
		mov      ah,0            ; AH = function number
		int      10h             ; Call video BIOS
		mov      _Buffer, VIDEO  ; VIDEO RAM &address -> _Buffer
		
		pop di
		pop si
		pop      bp              ; Restore BP
		ret
SetVgaMode ENDP


;===========================================================================
; setpal(char far *color_regs, int firstreg,int numregs)
;   Set VGA color registers, beginning with FIRSTREG and
;    continuing for NUMREGS to the color values in COLOR_REGS
;===========================================================================
public C SetPal
SetPal PROC far
ARG   regs:DWORD
		push  bp              ; Save BP
		mov   bp,sp           ; Set up stack pointer
		push si
		push di

		VSYNC

		les   dx,regs         ; Point ES:SX at palette registers
		mov   ah,10h          ; Specify BIOS function 10h
		mov   al,12h          ; ...subfunction 12h
		mov   bx,0            ; Start with first register
		mov   cx,100h         ; Set all 256 (100h) registers
		int   10h             ; Call video BIOS

		pop di
		pop si
		pop   bp              ; Restore BP
		ret
SetPal ENDP



; ============================================================================
; void Clear( int c )
; Clear the current "screen" to a given color.
; ============================================================================
public C Clear
Clear        proc far
ARG colr:DWORD
		push bp
		mov  bp,sp
		push si
		push di

		VSYNC

		les dx, colr            ; put color in dx
		mov  es,_Buffer       ; move buffer to es
		xor  di,di            ; di=0
		mov  al,dl            ; copy dl to al
		mov  ah,dl            ; copy dl to ah
		shl  eax,16           ; shift left eax by 4 bits
		mov  al,dl            ; copy dl to al
		mov  ah,dl            ; copy dl to ah
		mov  cx,16000         ; move 16000 to cx
		rep  stosd            ; store DWORD 4 bytes 16000 times

		pop di
		pop si
		pop  bp
		ret
Clear        endp


; ============================================================================
; void PutBlock( int x, int y, int w, int h, char far* buf )
; Draw a bitmap image into the buffer at location x,y.  Every pixel of the
; image is drawn, so no transparent areas are possible.  Very quick.  Good
; for drawing tiles in a tile based game!
; ============================================================================
public C PutBlock
PutBlock     proc far
ARG   x:WORD,y:WORD,w:WORD,h:WORD,buf:DWORD
		push bp
		mov  bp,sp
		push ds
		push si
		push di

		mov  es,_Buffer          ; put current buffer in es
		mov bx,y                ; copy Y into bx
		shl  bx,1                ; shiftleft 1
		mov  di,[RowOff+bx]      ; store Y Index in di
		add di,x                ; add X in di
		lds si,buf
		mov bx,w                ; copy W into bx
		mov dx,h                ; copy H into dx
		mov  bp,320              ; copy 320 into dp
		sub  bp,bx               ; subtract bx with bp
		mov  ax,bx               ; copy bx into ax
		and  ax,3                ; and 3 with ax
		shr  bx,2                ; shiftright bx by 2
PLoop:
		mov  cx,bx               ; copy bx into cx
		rep  movsd               ; move DWORD 4 bytes cx times
		mov  cx,ax               ; copy ax into cx
		rep  movsb               ; move BYTE cx times
		add  di,bp               ; add bp to di
		dec  dx                  ; dec dx by 1
		jnz  PLoop               ; goto PLoop if not zero

		pop  di
		pop  si
		pop  ds
		pop  bp
		ret
PutBlock     endp


; ============================================================================
; void PutSprite( int x, int y, int w, int h, char far* buf )
; Like Put() except that any pixels of color 0 will be transparent.  Not
; nearly as fast as Put(), but you'll need it for certain sprites, etc.
; ============================================================================
public C PutSprite
PutSprite    proc far
ARG x:WORD,y:WORD,w:WORD,h:WORD,buf:DWORD
		push bp
		mov  bp,sp
		push ds
		push si
		push di

		mov  es,_Buffer
		mov bx, y
		shl  bx,1
		mov  di,[RowOff+bx]
		add di, x
		lds si, buf
		mov bx, w
		mov dx, h
		mov  bp,320
		sub  bp,bx
PSLoop1:
		mov  cx,bx
PSLoop2:
		lodsb
		or   al,al
		je   PSSkip
		mov  [es:di],al
PSSkip:
		inc  di
		dec  cx
		jne  PSLoop2
		add  di,bp
		dec  dx
		jnz  PSLoop1

		pop di
		pop si
		pop  ds
		pop  bp
		ret
PutSprite    endp
end
