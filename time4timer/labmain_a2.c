/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

int mytime = 0x5957;
int timeoutcount = 0;
char textstring[] = "text, more text, and even more text!";

void update_time(void){
  
  //Reads minutes and hours from mytime.
  int minutes = ((mytime >> 12) & 0xF) * 10 + ((mytime >> 8) & 0xF);

  int hours = ((mytime >> 20) & 0xF) * 10 + ((mytime >> 16) & 0xF);

  tick(&mytime); //tick(&mytime);

  //Tick resets mytime when seconds > 59.
  if (mytime == 0) {
    minutes++;

    if (minutes == 60) {
        minutes = 0;
        hours = (hours + 1) % 100;
    }

      // Store hours and minutes; seconds are now 00.
      mytime = ((hours / 10) << 20) | ((hours % 10) << 16) | ((minutes / 10) << 12) | ((minutes % 10) << 8);
  }
}

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
void labinit(void){

  volatile int *timer_status = (volatile int *) 0x04000020;
  volatile int *timer_control = (volatile int *) 0x04000024;
  volatile int *timer_periodl = (volatile int *) 0x04000028;
  volatile int *timer_periodh = (volatile int *) 0x0400002C;

  *timer_periodl = 0xC6BF;
  *timer_periodh = 0x002D;

  *timer_status = 0;
  *timer_control = 0x6;

}

void set_leds(int led_mask){

  volatile int *leds = (volatile int *) 0x04000000; //leds är en pekare till ett heltal, volatile kollar om värdet förändras.
  *leds = led_mask & 0x3FF; // 0011 1111 1111 10 leds 

}

void set_displays(int display_number, int value){

  int digits[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 
    0x12, 0x02, 0x78, 0x00, 0x10 
  };
  
  volatile int *display = (volatile int *)(0x04000050 + display_number * 0x10);
  *display = digits[value];

}

int get_sw(void){

  volatile int *switches = (volatile int *) 0x04000010; 
  return *switches & 0x3FF;

}

int get_btn(void){

  volatile int *button = (volatile int *) 0x040000d0;
  return *button & 0x1;

}


/* Your code goes into main as well as any needed functions. */
int main()
{
  labinit();

  volatile int *timer_status = (volatile int *) 0x04000020;

  int leds = 0;
  int led_timeoutcount = 0;

  set_leds(leds);

  while (leds < 15) {  

    if (*timer_status & 0x1){
      *timer_status = 0;
      led_timeoutcount++;

      if (led_timeoutcount == 10){
        leds++;
        set_leds(leds);
        led_timeoutcount = 0;
      }
    }
  }

  // Enter a forever loop
  while (1) {
  
    if (get_sw() & 0x80) { //
        break;
    }

    if (get_btn()) {

      int switches = get_sw();

      int value = switches & 0x3F;
      int select = (switches >> 8) & 0x3;

      int tens = value / 10;
      int ones = value % 10;

      if (select == 1) {
          mytime = (mytime & 0xFFFF00) | (tens << 4) | ones;
      }

      if (select == 2) {
          mytime = (mytime & 0xFF00FF) | (tens << 12) | (ones << 8);
      }
  
      if (select == 3) {
          mytime = (mytime & 0x00FFFF) | (tens << 20) | (ones << 16);
      }
    }

    if(*timer_status & 0x1){

      *timer_status = 0;
      timeoutcount++;

      if (timeoutcount == 10){

        set_displays(0, mytime & 0xF);
        set_displays(1, (mytime >> 4) & 0xF);

        set_displays(2, (mytime >> 8) & 0xF);
        set_displays(3, (mytime >> 12) & 0xF);

        set_displays(4, (mytime >> 16) & 0xF);
        set_displays(5, (mytime >> 20) & 0xF);

        time2string(textstring, mytime);
        display_string(textstring);

        update_time(); //tick(&mytime);

        timeoutcount = 0;
      }
    }
  }
  return 0;
}