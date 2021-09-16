#include "o3.h"
#include "gpio.h"
#include "systick.h"
#include <stdio.h>

typedef struct {
	volatile word CTRL;
	volatile word MODEL;
	volatile word MODEH;
	volatile word DOUT;
	volatile word DOUTSET;
	volatile word DOUTCLR;
	volatile word DOUTTGL;
	volatile word DIN ;
	volatile word PINLOCKN;
} gpio_port_map_t;

typedef struct {
	volatile gpio_port_map_t port[6];
	volatile word unused_space [10];
	volatile word EXTIPSELL;
	volatile word EXTIPSELH;
	volatile word EXTIRISE;
	volatile word EXTIFALL;
	volatile word IEN;
	volatile word IF;
	volatile word IFS;
	volatile word IFC;
	volatile word ROUTE;
	volatile word INSENSE;
	volatile word LOCK;
	volatile word CTRL;
	volatile word CMD;
	volatile word EM4WUEN;
	volatile word EM4WUPOL;
	volatile word EM4WUCAUSE;
} gpio_map_t;

typedef struct {
	volatile word CTRL;
	volatile word LOAD;
	volatile word VAL;
	volatile word CALIB;
} systick_t;

void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
	    if (i >= 10) {
		    i -= 10;
		    timestamp[offset]++;
		
	    } else {
		    timestamp[offset+1] = '0' + i;
		    i=0;
	    }
    }
}

void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}


//Define states
#define S_STATE 0
#define M_STATE 1
#define H_STATE 2
#define CTDOWN_STATE 3
#define AL_STATE 4

//variables
int seconds, minutes, hours, state;
volatile gpio_map_t* GPIO_map;

//Toggles light
void set_LED(int on) {
    if(on == 1)
    	GPIO_map->port[GPIO_PORT_E].DOUTSET = 0b0100;
    else
    	GPIO_map->port[GPIO_PORT_E].DOUTCLR = 0b0100;
}

//PB1
void GPIO_EVEN_IRQHandler(void) {
    switch (state) {
    	case H_STATE:
    		if(hours + minutes + seconds == 0) {
    			state = AL_STATE;
    			set_LED(1);
    		}
    		else
    			state++;
    		break;
    	case AL_STATE:
    		state = S_STATE;
    		set_LED(0);
    		break;
    	case CTDOWN_STATE:
    		break;
    	default:
    		state++;
    };

    GPIO_map->IFC = 1<<10;
}

//PB0
void GPIO_ODD_IRQHandler(void) {
    switch (state) {
		case S_STATE:
			seconds++;
			break;
		case M_STATE:
			minutes++;
			break;
		case H_STATE:
			hours++;
			break;
    };

    GPIO_map->IFC = 1<<9; //PB0 pin
}


void SysTick_Handler() {
	if (state == CTDOWN_STATE) {
		if (seconds <= 0) {
			if (minutes <= 0) {
				if (hours <= 0) {
					state = AL_STATE;
					set_LED(1);
				}
				else{
				hours--;
				minutes = 60;
				}
			}
			else{
			minutes--;
			seconds = 60;
			}
		}
		seconds--;
	}
}

void set_4bit_flag(volatile word *w, int i, word flag) {
	*w &= ~(0b1111 << (i * 4));
	*w |= flag << (i*4);
}

void init_io() {
    GPIO_map = (gpio_map_t*) GPIO_BASE;
    #define LED_PIN 2
    #define PB0_PIN 9
    #define PB1_PIN 10

    //Set input mode
    set_4bit_flag(&GPIO_map->port[GPIO_PORT_E].MODEL, LED_PIN, GPIO_MODE_OUTPUT);
	set_4bit_flag(&GPIO_map->port[GPIO_PORT_B].MODEH, PB0_PIN-8, GPIO_MODE_INPUT);
	set_4bit_flag(&GPIO_map->port[GPIO_PORT_B].MODEH, PB1_PIN-8, GPIO_MODE_INPUT);

    //Set selection for button pin
    set_4bit_flag(&GPIO_map->EXTIPSELH, PB0_PIN-8, 0b0001);
	set_4bit_flag(&GPIO_map->EXTIPSELH, PB1_PIN-8, 0b0001);

    //Set fall
	GPIO_map->EXTIFALL |= 1 << PB0_PIN;
	GPIO_map->EXTIFALL |= 1 << PB1_PIN;

    //Set interrupt
	GPIO_map->IEN |= 1 << PB0_PIN;
	GPIO_map->IEN |= 1 << PB1_PIN;

    //Start system clock
    volatile systick_t* sys_tick;
    sys_tick = (systick_t*) SYSTICK_BASE;
    sys_tick->CTRL = 0b0111;
    sys_tick->LOAD = FREQUENCY;
}

void update_time_display() {
	char str[7];
    time_to_string(str, hours, minutes, seconds);
    lcd_write(str);
}


int main(void) {

    init();
    init_io();

	//Init
	seconds = 0;
	minutes = 0;
	hours = 0;
	state = S_STATE;

	while (1){
	update_time_display();
	}

	return 0;
}

