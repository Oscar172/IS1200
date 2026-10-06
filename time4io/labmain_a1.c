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
char textstring[] = "text, more text, and even more text!";

void update_time(void)
{
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
void labinit(void)
{}

void set_leds(int led_mask){

  volatile int *leds = (volatile int *) 0x04000000; // manages the 10 LEDs, 1 lights up
  *leds = led_mask & 0x3FF; // 0011 1111 1111 10 leds 

}

void set_displays(int display_number, int value){
// 0 lights up, digits[2] => 0x24 = 0 010 0100
  int digits[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 
    0x12, 0x02, 0x78, 0x00, 0x10 
  };
  
  volatile int *display = (volatile int *)(0x04000050 + display_number * 0x10);
  *display = digits[value];

}

int get_sw(void){

  volatile int *switches = (volatile int *) 0x04000010;  // *switches reads hardwareregister
  return *switches & 0x3FF; // & 0x3FF saves only the 10 lowest bits

}

int get_btn(void){

  volatile int *button = (volatile int *) 0x040000d0;
  return *button & 0x1; // returns 1 if pressed

}


/* Your code goes into main as well as any needed functions. */
int main()
{
  labinit();

  int leds = 0;
  set_leds(leds);

  while (leds < 15) {  
    delay(2400);
    leds++;
    set_leds(leds);
  }

  // Enter a forever loop
  while (1) {
  
    set_displays(0, mytime & 0xF);
    set_displays(1, (mytime >> 4) & 0xF);

    set_displays(2, (mytime >> 8) & 0xF);
    set_displays(3, (mytime >> 12) & 0xF);

    set_displays(4, (mytime >> 16) & 0xF);
    set_displays(5, (mytime >> 20) & 0xF);

    if (get_sw() & 0x80) { // switch if on --> break (SW8)
        break;
    }

    if (get_btn()) {

      int switches = get_sw();

      int value = switches & 0x3F; // the value from the switches gets stored in value, 
      int select = (switches >> 8) & 0x3;

      int tens = value / 10; // removes the one digit and leaves the ten didigt.          39/10 = 3
      int ones = value % 10; // the remainder after divisin by 10 leaves the one digit,   39 % 10 = 9 

      if (select == 1) { // Update the seconds when the switches are 01
          mytime = (mytime & 0xFFFF00) | (tens << 4) | ones; // clear old seconds (lowest 8 bits), move tens digit into bits 4-7, place one digits in bits 0-3 
      }
/*
      0x00123456
      0x00123400
      0x00000030
      0x00000009
      after OR:
      0x00123439
*/
      if (select == 2) { // Update the minutes when the switches are 10
          mytime = (mytime & 0xFF00FF) | (tens << 12) | (ones << 8); // clear old minutes (bits 8-15), move tens bits (12-15), move ones bits (8-11)
      }

      if (select == 3) { // Update the hours when the switches are 11
          mytime = (mytime & 0x00FFFF) | (tens << 20) | (ones << 16); // clear hours (bits 16-23), move tens (20-23), move ones (16-19)
      }
    }

    time2string(textstring, mytime);
    display_string(textstring);
    delay(2300);
    update_time(); //tick(&mytime);
  }
  return 0;
}