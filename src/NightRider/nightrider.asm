        .org 0h
vectors:
reset:	
	jump start
	.align 4
interrupt:
	jump interruptHandler
	.align 4
exception:
	jump exceptionHandler
	.align 4
svc_vector:
	nop
	nop

start:
        // set up a stack

        load r0, (StackEnd & 0x0000ffff)
        load r1, (StackEnd & 0xffff0000) >> 16
        ldspr s8, r0

	// Configure interrupt controller

	load r0, 0xb000
	load r1, 0x0001
	out  r1, r0

	load r0, 0xb001
	load r1, 0x0001
	out  r1, r0

	// enable interrupts

	load r0, 1
	load r1, 0
	ldspr s0, r0
	
        // Print banner

        load r0, (Banner & 0x0000ffff)
        load r1, (Banner & 0xffff0000) >> 16
        ldspr s9, r0

        //
        //  r0 : word from banner
        //  r1 : address in banner
        //

        load r1, 0
banner_loop:
        ldw r0, [s9, r1]
        cmp r0, 0
        jumpz done_banner_loop
        call OutByte
        add r1, 2
        jump banner_loop


done_banner_loop:
        
	// 	Do night rider routine

	//
	//	Port: 0x1000 - 0x1fff
	//
  	//	Register     |       Function	
	//      -----------------------------------------------------
	//      0x1000       |       green LEDs
	//      0x1001       |       red LEDs
	//      0x1002       |       Switches
	//      0x1003       |       Hex 0
	//      0x1004       |       Hex 1
	//      0x1005       |       Hex 2
	//      0x1006       |       Hex 3

	//
	//	r4: green leds (8 bits)
	//	r5: red leds (10 bits)
	//	r6: direction
	//	r7: count for hex display

	load r4,1
	load r5,0
	load r6,0
	load r7,0

nightRiderLoop:
	
	test r4, 0xffff
	jumpz doRed
doGreen:
	
	test r6,1
	jumpnz doGreenForwards

	sl0 r4
	cmp r4,0x100
	jumpnz doneRedGreen

	load r4, 0
	load r5, 1

	jump doneRedGreen
doGreenForwards:
	
	sr0 r4
	cmp r4, 1
	jumpnz doneRedGreen

	xor r6,0xffff // flip direction

	jump doneRedGreen
doRed:
	test r6,1
	jumpnz doRedForwards

	sl0 r5
	cmp r5,0x200
	jumpnz doneRedGreen

	xor r6,0xffff

	jump doneRedGreen
doRedForwards:
	
	sr0 r5
	cmp r5, 0
	jumpnz doneRedGreen

	load r5,0
	load r4,0x80 
	jump doneRedGreen

doneRedGreen:

	add r7,1

	load r0,0x1000
	out  r4,r0
	load r0,0x1001
	out  r5,r0

	load r0, r7
	load r1, 0x1003
	call OutHex
	bsr  r0, 4
	add  r1, 1
	call OutHex
	bsr  r0, 4
	add  r1, 1
	call OutHex
	bsr  r0, 4
	add  r1, 1
	call OutHex

	call Wait

	jump nightRiderLoop

done_night_rider:
	// we should never get here
	sleep

/*====================== UART Routines ====================*/

OutByte:

        stw r1, [s8, 0]
        decw s8
	stw r2, [s8, 0]
	decw s8
	stw r3, [s8, 0]
	decw s8

	// preserve link register
	stspr r2, s1
	
        call WaitTxNotFull

        load r1, 0
        out r0, r1

	// restore link register
	ldspr s1, r2

	incw s8
	ldw r3, [s8, 0]
	incw s8
	ldw r2, [s8, 0]
        incw s8
        ldw r1, [s8, 0]

        ret

WaitTxNotFull:

        stw r0, [s8, 0]
        decw s8
        stw r1, [s8, 0]
        decw s8

        load r0, 3
WaitLoop:
        in   r1, r0
        test r1, 1  // TX_FIFO_FULL
        jumpnz WaitLoop

	// disable counter

	load r0,0
	load r1,0xc000
	out  r0,r1

        incw s8
        ldw  r1, [s8, 0]
        incw s8
        ldw  r0, [s8, 0]

        ret

Wait:
        stw r0, [s8, 0]
        decw s8
        stw r1, [s8, 0]
        decw s8

	// Load counter load register

	load r0, 0xffff
	load r1, 0xc003
	out  r0, r1

	// clear status

	load r0, 1
	load r1, 0xc001
	out  r0, r1

	// set counter enable, prescale and load
	load r0, 0x5b
	load r1, 0xc000
	out  r0, r1

	sleep

	incw s8
        ldw  r1, [s8, 0]
        incw s8
        ldw  r0, [s8, 0]

        ret

OutHex:
        stw r0, [s8, 0]
        decw s8
        stw r1, [s8, 0]
        decw s8
        stw r2, [s8, 0]
        decw s8
        stw r3, [s8, 0]
        decw s8
	stw r4, [s8, 0]
        decw s8
        stw r5, [s8, 0]
        decw s8

	// store spr9
	stspr r2, s9

	load r4, HexTable & 0xffff
	load r5, HexTable >> 16

	ldspr s9, r4

	sl0 r0
	and r0,0x1e

	ldw r0, [s9, r0]
	out r0, r1

	// restore spr9
	ldspr s9, r2
	
	incw s8
        ldw  r5, [s8, 0]
	incw s8
        ldw  r4, [s8, 0]
	incw s8
        ldw  r3, [s8, 0]
	incw s8
        ldw  r2, [s8, 0]
        incw s8
        ldw  r1, [s8, 0]
        incw s8
        ldw  r0, [s8, 0]

        ret

interruptHandler:
	stw r0, [s8, 0]
        decw s8
        stw r1, [s8, 0]
        decw s8

	// Acknowledge all interrupts
	load r0, 0xb002
	load r1, 0xffff
	out  r1, r0

	// Acknowledge timer interrupt
	load r0, 0
	load r1, 0xc000
	out  r0, r1

	incw s8
        ldw  r1, [s8, 0]
        incw s8
        ldw  r0, [s8, 0]

	reti

exceptionHandler:
	rete

/*====================== Data section =====================*/

        .align 100h

data:

Banner:
        dw "NanoBrain SoC Night Rider Demo (c) 2017\r\n"
	dw 0
HexTable:
	dw 0xc0
	dw 0xf9
	dw 0xa4
	dw 0xb0
	dw 0x99
	dw 0x92
	dw 0x82
	dw 0xf8
	dw 0x80
	dw 0x90
	dw 0x88
	dw 0x83
	dw 0xc6
	dw 0xa1
	dw 0x84
	dw 0x8e

        .align 100h
StackStart:
        .times 100h dw 0x0
StackEnd:

        .end
