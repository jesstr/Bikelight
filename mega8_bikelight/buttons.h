#ifndef _BUTTONS_H_
#define _BUTTONS_H_ 1

#include <avr/io.h>

/* Buttons registers defines */
#define BUTTON_1_DDR		DDRD
#define BUTTON_1_PORT		PORTD
#define BUTTON_1_PIN_REG	PIND
#define BUTTON_1_PIN		PD2

/* */
void Buttons_Init(void);

#endif /* _BUTTONS_H_ */