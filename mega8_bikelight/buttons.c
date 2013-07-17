#include "buttons.h"

/* Buttons initialization */
void Buttons_Init(void)
{
	/* Set all PINs as input with internal pull-ups  */ 
	BUTTON_1_DDR&=~(1<<BUTTON_1_PIN);
	BUTTON_1_PORT|=(1<<BUTTON_1_PIN);
}