/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);   //extern tells us that the function can be found in other parts of the program
extern void tick(int*);               // tick is found in timetemplate.S for example.
extern void delay(int);
extern int nextprime( int );

extern void enable_interrupt(void);

int prime = 1234567;    //startingvalue for prime calculation

int mytime = 0x5957;    //00:59:57
int timeoutcount = 0;   //Number of timerinterrupts

char textstring[] = "text, more text, and even more text!";

void set_displays(int display_number, int value);  //functionprototype 

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) {

  volatile int *timer_status = (volatile int *) 0x04000020; // creates a pointer to the timers stautsregistry on address 0x04000020
  *timer_status = 0;                                        // reset the timeout flag (T0) to 0 

  timeoutcount++; //count one more timer interrupt

  if (timeoutcount == 10){ // Update the clock once every 10 interrupts (1 second)
    timeoutcount = 0;      // Reset the counter for the next second
    
    // seconds
    set_displays(0, mytime & 0xF);  // (ones digit) saves the 4 LSB 
    set_displays(1, (mytime >> 4) & 0xF); // (tens digit) moves tendigits (tiotal) 4 bits to the right and saves them with 0xF

    // minutes
    set_displays(2, (mytime >> 8) & 0xF);
    set_displays(3, (mytime >> 12) & 0xF);

    // Hours
    set_displays(4, (mytime >> 16) & 0xF);
    set_displays(5, (mytime >> 20) & 0xF);

    tick(&mytime);  //tick updates the time by one second, (&mytime) means "the address to mytime" so that the function can change the variable
  }
}

/* Add your code here for initializing interrupts. */
void labinit(void){

  volatile int *timer_status = (volatile int *) 0x04000020;   //timer status register (includes the timeoutFlag)
  volatile int *timer_control = (volatile int *) 0x04000024;  //timer control register (used to start, stop and allow interrupts from the timer)
  volatile int *timer_periodl = (volatile int *) 0x04000028;  //lower 16 bits of the timer period 
  volatile int *timer_periodh = (volatile int *) 0x0400002C;  //upper 16 bits of the timer period

  //sets the timer period to 100ms: 3 000 000 clock cycles at 30MHz minus 1 (minus 1 since 0 counts as a cycle)
  *timer_periodl = 0xC6BF;
  *timer_periodh = 0x002D;

  *timer_status = 0; // resets timeoutFlag to 0, for safety
  *timer_control = 0x7; //enable timer interrupts ITO, continuous mode CONT, and start the timer START (0x7 = 0111)

  enable_interrupt(); // calls function in boot.S, that allows the processor to recieve interrupts

}

void set_leds(int led_mask){

  volatile int *leds = (volatile int *) 0x04000000; //point to the memory mapped LED register
  *leds = led_mask & 0x3FF; //use only the lowest 10 bits to control the LEDs, 1 turns on 0 turns off. 0011 1111 1111
                            // set_leds(7) => 00 0000 0111, lights up 3 LEDs representing binary 7

}

void set_displays(int display_number, int value){

  // seven-segment bit patterns for the digits 0-9
  // 0 turns on, 1 turns off
  // digits[2] => 0x24 = 0 010 0100
  int digits[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 
    0x12, 0x02, 0x78, 0x00, 0x10 
  };
  
  volatile int *display = (volatile int *)(0x04000050 + display_number * 0x10); //point to the selected seven-segment display register
  *display = digits[value]; //write the segement pattern for the requested digit
  //display 0 is at 0x04000050, the next 0x04000060 ...
}

int get_sw(void){

  volatile int *switches = (volatile int *) 0x04000010; //point to the memory mapped switch register
  return *switches & 0x3FF; //read the 10 switches 0011 1111 1111, LSB is SW1

}

int get_btn(void){

  volatile int *button = (volatile int *) 0x040000d0; //point to the memory mapped button register
  return *button & 0x1; //return the LSB, button state. 1 is pushed down and 0 is normal state

}


/* Your code goes into main as well as any needed functions. */
int main(void){

  labinit () ; //initialize the timer and enable interrupts

  while (1) {
    print ("Prime: "); 
    prime = nextprime (prime); //Find and store the next larger prime number
    print_dec (prime); //print the prime number in decimal
    print ("\n");
  }
}