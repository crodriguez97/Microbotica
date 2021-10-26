/*
 * main.h
 *
 *  Created on: 13 oct. 2020
 *      Author: Nacho
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdbool.h>

#define DIV_RELOJ_PWM   2
#define PERIOD_PWM (SysCtlClockGet()*0.02)/DIV_RELOJ_PWM  // Periodo de 20Hz
#define CYCLECOUNT_25 PERIOD_PWM/4   // Ciclo de trabajo del servo "fijado"
#define STOP_COUNT (SysCtlClockGet()*0.0015393)/DIV_RELOJ_PWM   // 1.5 ms
#define MAX_COUNT (SysCtlClockGet()*0.0018)/DIV_RELOJ_PWM      // 1.8 ms
#define MIN_COUNT  (SysCtlClockGet()*0.0012)/DIV_RELOJ_PWM       // 1.2 ms
#define CYCLE_CHANGES (MAX_COUNT-MIN_COUNT)      // variacion de amplitud tras pulsacion

extern void configServos();
extern void cambiaCiclo(uint32_t ui32DutyCycle1);

uint32_t ui32DutyCycle1, maxCount, minCount, cycle_changes;

#endif /* MAIN_H_ */
