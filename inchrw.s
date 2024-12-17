.include	"equdefs.inc"
.global inbyte

.section .bss
.even

.text
.even

********************
** inbyte: 
** 入力: fd(long, 4bytes): チャンネルの指定
** 出力: char型1文字データ(%d0)
*********************
inbyte:
	movem.l	%d1-%d3/%a0, -(%sp)
	move.l	%sp, %a0
	add.l	#20, %a0
	sub.l	#4, %sp
	bra	loop
loop:
	move.l	#SYSCALL_NUM_GETSTRING, %d0	
	move.l	(%a0), %d1
	move.l	%sp, %d2
	move.l	#1, %d3	
	trap	#0

	cmpi.l	#1, %d0
	bne	loop
	
	move.b	(%sp), %d0
	add.l	#4, %sp
	
	movem.l	(%sp)+, %d1-%d3/%a0
	rts
	
