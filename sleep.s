.global sleep

.section .bss
.even

.text
.even

********************
** sleep: 数秒待機
*********************
sleep:
	movem.l	%d1, -(%sp)
sleep_loop:
	addi.l	#1, %d1
	cmp.l	#500000, %d1
	bne	sleep_loop	
	movem.l (%sp)+, %d1
	rts

