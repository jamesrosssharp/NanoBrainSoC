        .org 0h

start:
        // set up a stack

        load r0, (StackEnd & 0x0000ffff)
        load r1, (StackEnd & 0xffff0000) >> 16
        ldspr s8, r0

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
        add r1, 1
        jump banner_loop


done_banner_loop:
        sleep

/*====================== UART Routines ====================*/

OutByte:

        stw r1, [s8, 0]
        decw s8

        call WaitTxNotFull

        load r1, 0
        out r0, r1

        incw s8
        ldw r1, [s8, 0]

        ret

WaitTxNotFull:

        stw r0, [s8, 0]
        decw s8
        stw r1, [s8, 0]
        decw s8

        load r0, 3
        in   r1, r0
WaitLoop:
        test r1, 1  // TX_FIFO_FULL
        jumpnz WaitLoop

        incw s8
        ldw  r1, [s8, 0]
        incw s8
        ldw  r0, [s8, 0]

        ret

/*====================== Data section =====================*/

        .align 100h

data:

Banner:
        dw "NanoBrain SoC Bootloader (c) 2017\r\n"


        .align 100h
StackStart:
        .times 100h dw 0x0
StackEnd:

        .end
