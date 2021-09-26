
// This is an example program for music programming.
// You will run this program without modification. 

// Authors: Min He
// Date: August 26, 2018

// Header files 
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SwitchLed.h"

// 2. Declarations Section

// define music note data structure 
struct Note {
  unsigned long tone_index;
  unsigned char delay;
};
typedef const struct Note NTyp;

// Constants
#define TONE_DURATION 2 // each tone uses the same duration 
#define NUM_VALs  64  // Assume 3-bit DAC is used, that will give 16 values for one period.

#define RELEASED 0
#define PRESSED 1

// initial values for piano major tones.
// Assume SysTick clock frequency is 16MHz.
const unsigned long tonetab[] =
// C, D, E, F, G, A, B
// 1, 2, 3, 4, 5, 6, 7
{30534 * 2,27211 * 2,24242 * 2,22923 * 2,20408 * 2,18182 * 2,16194 * 2, // C4 Major notes
15289 * 2,13621 * 2,12135 * 2,11454 * 2,10204 * 2,9091 * 2,8099 * 2, // C5 Major notes
7645 * 2,6810 * 2,6067 * 2,5727 * 2,5102 * 2,4545 * 2,4050 * 2, // C6 Major notes
3822 * 2, 3405 * 2, 3034 * 2, 2863 * 2, 2551 * 2, 2273 * 2, 2025 * 2}; // C7 Major notes

// index definition for tones used in happy birthday.
#define C4 0
#define D4 1
#define E4 2
#define F4 3
#define G4 4
#define A4 5
#define B4 6
#define C5 0+7
#define D5 1+7
#define E5 2+7
#define F5 3+7
#define G5 4+7
#define A5 5+7
#define B5 6+7
#define C6 0+2*7
#define D6 1+2*7
#define E6 2+2*7
#define F6 3+2*7
#define G6 4+2*7
#define A6 5+2*7
#define B6 6+2*7
#define C7 0+3*7
#define D7 1+3*7
#define E7 2+3*7
#define F7 3+3*7
#define G7 4+3*7
#define A7 5+3*7
#define B7 6+3*7
#define PAUSE 255

// note table for Happy Birthday
// doe ray mi fa so la ti 
// C   D   E  F  G  A  B
NTyp mysong[][100] =
{
 
// score table for Happy Birthday
{G4,2,G4,2,A4,4,G4,4,C5,4,B4,4,
   PAUSE,4,  G4,2,G4,2,A4,4,G4,4,D5,4,C5,4,
   PAUSE,4,  G4,2,G4,2,G5,4,E5,4,C5,4,B4,4,A4,8, 
	 PAUSE,4,  F5,2,F5,2, E5,4,C5,4,D5,4,C5,8,0,0},

// score table for Mary Had A Little Lamb
{E4, 4, D4, 4, C4, 4, D4, 4, E4, 4, E4, 4, E4, 8, 
 D4, 4, D4, 4, D4, 8, E4, 4, G4, 4, G4, 8,
 E4, 4, D4, 4, C4, 4, D4, 4, E4, 4, E4, 4, E4, 8, 
 D4, 4, D4, 4, E4, 4, D4, 4, C4, 8, 0, 0 },

// score table for Twinkle Twinkle Little Stars
{C4,4,C4,4,G4,4,G4,4,A4,4,A4,4,G4,8,F4,4,F4,4,E4,4,E4,4,D4,4,D4,4,C4,8, 
 G4,4,G4,4,F4,4,F4,4,E4,4,E4,4,D4,8,G4,4,G4,4,F4,4,F4,4,E4,4,E4,4,D4,8, 
 C4,4,C4,4,G4,4,G4,4,A4,4,A4,4,G4,8,F4,4,F4,4,E4,4,E4,4,D4,4,D4,4,C4,8,0,0},

};

//   Function Prototypes
void Delay(void);
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);
void Switch_Init(void);
void Button_Init(void);
void Extra_Button_Init(void);
void play_a_song(NTyp notetab[]);

//Global Variable
int songNumber = 0;
int ManuelorAuto = 0; //1 is auto
unsigned char current_s;
int octave_shift = 0;

// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){
  DisableInterrupts();    
  DAC_Init();       // Port B is DAC
	Switch_Init();		// Port F Buttons
	Button_Init();	//Port D Buttons
	Extra_Button_Init();	//Port A Buttons
	EnableInterrupts();
	current_s=RELEASED;
	
  while(1){
		while(ManuelorAuto){
			play_a_song(mysong[songNumber]); //while ManuelorAuto is 1, the mode is AUTOPLAY
		}
  }
}

void play_a_song(NTyp notetab[])
{
	unsigned char i=0, j;
	int currentSong = songNumber;
	
	while (notetab[i].delay && ManuelorAuto &&(currentSong == songNumber)) {
		if (notetab[i].tone_index == PAUSE) 
			Sound_stop(); // silence tone, turn off SysTick timer
		else {
			Sound_Init((tonetab[notetab[i].tone_index + octave_shift])/NUM_VALs);
		}
		
		// tempo control: play current note for specified duration
		for (j=0;j<notetab[i].delay;j++) 
			Delay();
		
		Sound_stop();
		i++;  // move to the next note
	}
	
	// pause after each play
	for (j=0;j<15;j++) 
		Delay();
}


// Subroutine to wait 0.1 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void){
	unsigned long volatile time;
  time = 727240*20/91;  // 0.1sec
  while(time){
		time--;
  }
}

void Switch_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x11;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000F000F; //  configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;  //     disable analog functionality on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4,0

  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R |= 0x11;    //      PF4,PF0 is both edges
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00400000; // (g) bits:23-21 for PORTF, set priority to 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

void Button_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000008; // (a) activate clock for port D
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTD_CR_R |= 0x0F;         // allow changes to PD3-0
  GPIO_PORTD_DIR_R &= ~0x0F;    // (c) make PD3-0 in
  GPIO_PORTD_AFSEL_R &= ~0x0F;  //     disable alt funct on PD3-0
  GPIO_PORTD_DEN_R |= 0x0F;     //     enable digital I/O on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; //  configure PD3-0 as GPIO
  GPIO_PORTD_AMSEL_R &= ~0x0F;  //     disable analog functionality on PD3-0
  GPIO_PORTD_IS_R &= ~0x0F;     // (d) PD3-0 is edge-sensitive
  GPIO_PORTD_IBE_R |= 0x0F;    //     PD3-0 is both edges
  GPIO_PORTD_ICR_R = 0x0F;      // (e) clear flags 3-0
  GPIO_PORTD_IM_R |= 0x0F;      // (f) arm interrupt on PD3-0
  NVIC_PRI0_R = (NVIC_PRI0_R&0x0FFFFFFF)|0xA0000000; // (g) bits:31-29 for PORTD, set priority to 5
  NVIC_EN0_R |= 0x00000008;      // (h) enable interrupt 3 in NVIC
	
}

void Extra_Button_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000001; // (a) activate clock for port A
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTA_CR_R |= 0x1C;         // allow changes to PA4-2
  GPIO_PORTA_DIR_R &= ~0x1C;    // (c) make PA4-2 in
  GPIO_PORTA_AFSEL_R &= ~0x1C;  //     disable alt funct on PA4-2
  GPIO_PORTA_DEN_R |= 0x1C;     //     enable digital I/O on PA4-2
  GPIO_PORTA_PCTL_R &= ~0x000FFF00; //  configure PA4-2 as GPIO
  GPIO_PORTA_AMSEL_R &= ~0x1C;  //     disable analog functionality on PA4-2
  GPIO_PORTA_IS_R &= ~0x1C;     // (d) PA4-2 is edge-sensitive
  GPIO_PORTA_IBE_R |= 0x1C;    //     PA4-2 is both edges
  GPIO_PORTA_ICR_R = 0x1C;      // (e) clear flags 4-2
  GPIO_PORTA_IM_R |= 0x1C;      // (f) arm interrupt on PA4-2
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF0F)|0x000000C0; // (g) bits:7-5 for PORTD, set priority to 6
  NVIC_EN0_R |= 0x00000001;      // (h) enable interrupt 1 in NVIC
}

void GPIOPortF_Handler(void){ // called on touch of either SW1 or SW2
	 unsigned long In;
	
  if(GPIO_PORTF_RIS_R & 0x01){  // SW2 touched
		In = GPIO_PORTF_DATA_R&0x01; // read PF0:sw2 into In
		GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag0
		
		if((In == 0x00)&&(current_s==RELEASED)){ // zero means SW2 is pressed
			current_s=PRESSED;
		}
		
		if ((In == 0x01)&&(current_s==PRESSED)){
			current_s=RELEASED;
			if(ManuelorAuto) {
				songNumber++;
				if(songNumber == 3) songNumber = 0;
			} else {
				octave_shift += 7;
				if(octave_shift == 21) octave_shift = 0;
			}
		}
	}
	
  if(GPIO_PORTF_RIS_R & 0x10){  // SW1 touch
		In = GPIO_PORTF_DATA_R&0x10; // read PF4:sw1 into In
		GPIO_PORTF_ICR_R = 0x10; // acknowledge flag4
		
		if((In == 0x00)&& (current_s==RELEASED)){ // zero means SW1 is pressed
			current_s=PRESSED;
		}
		if ((In == 0x10)&& (current_s==PRESSED)){	 
			current_s=RELEASED;
			if(ManuelorAuto == 1) {
				ManuelorAuto = 0;
			}
			else {
				ManuelorAuto = 1;
			}
		}
	}
}

void GPIOPortD_Handler(void){

	unsigned long In = GPIO_PORTD_DATA_R & 0x0F;
	
  if(GPIO_PORTD_RIS_R & 0x01){  
		GPIO_PORTD_ICR_R = 0x01;  
		
		if((In == 0x01)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
			current_s=PRESSED;
			Sound_Init(tonetab[C4 + octave_shift]/NUM_VALs);
		}
	}
	
  if(GPIO_PORTD_RIS_R & 0x02){  
		GPIO_PORTD_ICR_R = 0x02;  
		
		if((In == 0x02)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
			current_s=PRESSED;
			Sound_Init(tonetab[D4 + octave_shift]/NUM_VALs);
		}
	}
	
	if(GPIO_PORTD_RIS_R & 0x04){  
		GPIO_PORTD_ICR_R = 0x04;  
		
		if((In == 0x04)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
			current_s=PRESSED;
			Sound_Init(tonetab[E4 + octave_shift]/NUM_VALs);
		}
	}
	
	if(GPIO_PORTD_RIS_R & 0x08){  
		GPIO_PORTD_ICR_R = 0x08;  
		if((In == 0x08)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
			current_s=PRESSED;
			Sound_Init(tonetab[F4 + octave_shift]/NUM_VALs);
		}
	}
	
	if ((In == 0x00)&& (current_s==PRESSED)){
			current_s=RELEASED;
			Sound_stop();	
	}
}

void GPIOPortA_Handler(void){

  	unsigned long In = GPIO_PORTA_DATA_R & 0x1C;
	
		if(GPIO_PORTA_RIS_R & 0x04){  
			GPIO_PORTA_ICR_R = 0x04;  
			if((In == 0x04)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
				current_s=PRESSED;
				Sound_Init(tonetab[G4 + octave_shift]/NUM_VALs);
			}
		}
		
		if(GPIO_PORTA_RIS_R & 0x08){  
			GPIO_PORTA_ICR_R = 0x08;  
			if((In == 0x08)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
				current_s=PRESSED;
				Sound_Init(tonetab[A4 + octave_shift]/NUM_VALs);
			}
		}
		
		if(GPIO_PORTA_RIS_R & 0x10){  
			GPIO_PORTA_ICR_R = 0x10;  	
			if((In == 0x10)&& (current_s==RELEASED) && (ManuelorAuto == 0)){
				current_s=PRESSED;
				Sound_Init(tonetab[B4 + octave_shift]/NUM_VALs);
			}

		}
		
		if ((In == 0x00)&& (current_s==PRESSED)){
				current_s=RELEASED;
				Sound_stop();			
		}
}


