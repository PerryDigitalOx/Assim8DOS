.AUTODEPEND

#     *Translator Definitions*
CC = bcc +GAME.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BC\LIB
INCLUDEPATH = C:\BC\INCLUDE


#     *Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#     *List Macros*


EXE_dependencies =  \
 game.obj \
 game386.obj

#     *Explicit Rules*
game.exe: game.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/d/P-/L$(LIBPATH) @&&|
c0l.obj+
game.obj+
game386.obj
game
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#     *Individual File Dependencies*
game.obj: game.cfg game.c 

game386.obj: game.cfg game386.asm 
	$(TASM) /MX /ZI /O GAME386.ASM,GAME386.OBJ

#     *Compiler Configuration File*
game.cfg: game.mak
  copy &&|
-R
-ml
-3
-g255
-v
-G
-O
-Og
-Oe
-Om
-Ov
-Ol
-Ob
-Op
-Oi
-Z
-k-
-vi-
-I$(INCLUDEPATH)
-L$(LIBPATH)
-P-.C
| game.cfg

