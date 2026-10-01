# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# PUSH / POP macros.                                        #
#                                                           #
# RARS and the GNU assembler (used for the DTEK-V board)    #
# use slightly different macro syntax. The file ships with  #
# the RARS-style block active, because Assignments 1-4 are  #
# done in RARS. When you port to the board in Assignment 5, #
# comment out the RARS block below and uncomment the DTEK   #
# block beneath it.                                         #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi	sp,sp,-4
#	sw	%reg,0(sp)
#.end_macro
#
#.macro	POP (%reg)
#	lw	%reg,0(sp)
#	addi	sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
.macro	PUSH reg
	addi sp,sp,-4
	sw \reg,0(sp)
.endm

.macro	POP reg
	lw	\reg,0(sp)
	addi	sp,sp,4
.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x0000
timstr:	.asciz 	"text more text lots of text\0"
	.text
	# delay is declared global so the auto-grader's test harness (which
	# calls delay directly from a small labmain.S) can resolve the symbol.
	.globl timetemplate, tick, time2string, delay, display_string

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 2		# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:
	lw	t0, 0(a0)
	addi	t0, t0, 1

	andi	t1, t0, 0xf
	sltiu	t2, t1, 0xa
	bnez	t2, tiend
	addi	t0, t0, 0x6

	andi	t1, t0, 0xf0
	sltiu	t2, t1, 0x60
	bnez	t2, tiend
	li		t0, 0

tiend:
	sw	t0, 0(a0)
	jr	ra

#########################################################
# Place for your functions: time2string, hexasc, delay. #
#########################################################

time2string:
        PUSH ra
        PUSH s0
        PUSH s1

        mv s0, a0        #Move/copy
        mv s1, a1

#FIRST NUMBER #1
        srli a0, s1, 12         #Shift Right Logical Immediate
        andi a0, a0, 0xF
        jal hexasc
        sb a0, 0(s0)

#SECOND NUMBER #2
    	srli a0, s1, 8         #Shift Right Logical Immediate
        andi a0, a0, 0xF
        jal hexasc
        sb a0, 1(s0)

#KOLON
    	li a0, 0x3A
    	sb a0, 2(s0)

#THIRD NUMBER #3
   	srli a0, s1, 4         #Shift Right Logical Immediate
        andi a0, a0, 0xF
        jal hexasc
        sb a0, 3(s0)

#FORTH NUMBER #4
        andi a0, s1, 0xF
        jal hexasc
        sb a0, 4(s0)

#NULL 
    	sb zero, 5(s0)

    	POP s1
        POP s0
        POP ra
    	jr ra

hexasc:
	andi a0, a0, 0x0F	#Keep only the LSB. AND if 1 and 1 = 1. 0F = 0000 0000 0000 1111
	
	li t0, 9		#Save 9 into t0
	bgt a0, t0, jump	#Is a0 > t0? If yes, go to jump. If no, keep going (bgt = branch if greater than)

#case1
	addi a0, a0, 0x30	#Adds 0x30 to convert into ASCII
	jr ra			#Returns from hexasc

#case2	
jump:
	addi a0, a0, 0x37	#Adds 0x37 because 10=0x0A => 0x0A + 0x37 = 0x41 => 'A'
	andi a0, a0, 0x7F	#Only keep the 7 least significant bits 
	jr ra

delay:
	while:
	ble a0, zero, done	# om a <= 0 avsluta loopen. while(ms > 0)
	li t0, 0 		# int i = 0;
	li t1, 4711
	addi a0, a0, -1 	# ms = ms - 1
		
	for:	
	bge t0, t1, end 
	addi t0, t0, 1
	j for
	
	end:
	j while
	
	done:
	jr ra


	
