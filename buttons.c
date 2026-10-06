void labinit(void)
{
    /* Timer */
    volatile int *timer_status  = (volatile int *) 0x04000020;
    volatile int *timer_control = (volatile int *) 0x04000024;
    volatile int *timer_periodl = (volatile int *) 0x04000028;
    volatile int *timer_periodh = (volatile int *) 0x0400002C;

    /* Buttons */
    volatile int *button_mask = (volatile int *) 0x040000D8;
    volatile int *button_edge = (volatile int *) 0x040000DC;

    /* Timer setup */
    *timer_periodl = 0xC6BF;
    *timer_periodh = 0x002D;

    *timer_status = 0;
    *timer_control = 0x7;

    /* Button #0 setup */
    *button_edge = 0x1;   // clear pending interrupt
    *button_mask = 0x1;   // enable interrupt for button bit 0

    enable_interrupt();
}
50000 i boot.S
void handle_interrupt(unsigned cause)
{
    if (cause == 16) {
        /* TIMER interrupt */
        volatile int *timer_status = (volatile int *) 0x04000020;

        *timer_status = 0;
        timeoutcount++;

        if (timeoutcount == 10) {
            timeoutcount = 0;

            set_displays(0, mytime & 0xF);
            set_displays(1, (mytime >> 4) & 0xF);
            set_displays(2, (mytime >> 8) & 0xF);
            set_displays(3, (mytime >> 12) & 0xF);
            set_displays(4, (mytime >> 16) & 0xF);
            set_displays(5, (mytime >> 20) & 0xF);

            update_time();
        }
    }

    else if (cause == 18) {
        /* BUTTON interrupt */

        volatile int *button_edge = (volatile int *) 0x040000DC;

        *button_edge = 0x1;

        update_time();
        update_time();

        set_displays(0, mytime & 0xF);
        set_displays(1, (mytime >> 4) & 0xF);
        set_displays(2, (mytime >> 8) & 0xF);
        set_displays(3, (mytime >> 12) & 0xF);
        set_displays(4, (mytime >> 16) & 0xF);
        set_displays(5, (mytime >> 20) & 0xF);
    }
}

och li t0, 0x50000 i boot.s
