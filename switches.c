void labinit(void)
{

    volatile int *timer_status  = (volatile int *) 0x04000020;
    volatile int *timer_control = (volatile int *) 0x04000024;
    volatile int *timer_periodl = (volatile int *) 0x04000028;
    volatile int *timer_periodh = (volatile int *) 0x0400002C;

    volatile int *switch_mask = (volatile int *) 0x04000018;
    volatile int *switch_edge = (volatile int *) 0x0400001C;


    *timer_periodl = 0xC6BF;
    *timer_periodh = 0x002D;

    *timer_status = 0;
    *timer_control = 0x7;


    *switch_edge = 0x1;   
    *switch_mask = 0x1;   

    enable_interrupt();
}
30000 i boot.S
void handle_interrupt(unsigned cause)
{
    if (cause == 16) {

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

    else if (cause == 17) {

        volatile int *switch_edge = (volatile int *) 0x0400001C;

        *switch_edge = 0x1;

        update_time();

        set_displays(0, mytime & 0xF);
        set_displays(1, (mytime >> 4) & 0xF);
        set_displays(2, (mytime >> 8) & 0xF);
        set_displays(3, (mytime >> 12) & 0xF);
        set_displays(4, (mytime >> 16) & 0xF);
        set_displays(5, (mytime >> 20) & 0xF);
    }
}

och li t0, 0x30000 i boot.s
