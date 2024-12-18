################################################################
### Software Jikken -- Makefile for C Jikken theme 3
### Kaito Shigyo <shigyo.kaito.437`>
### $Id: Makefile.3,v 1.2 2001/12/19 06:00:09 nom Exp $
################################################################

PROGRAM		= fs

# mon.o must be first so that ld places mon.o $400.

OBJECTS		= crt0.o mon.o csys68k.o inchrw.o mtk_asm.o \
			mtk_c.o outchr.o file.o main.o

SRCS		= crt0.s mon.s csys68k.c inchrw.s mtk_asm.s \
			mtk_c.c outchr.s file.c main.c

ASMGLIS		= crt0.glis mon.glis csys68k.glis inchrw.glis mtk_asm.glis \
			mtk_c.glis outchr.glis file.glis main.glis

include $(LIB_JIKKEN)/make.conf
