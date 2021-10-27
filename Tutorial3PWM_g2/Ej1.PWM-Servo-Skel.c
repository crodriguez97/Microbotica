/* ************************************************************************** */
/* UNIVERSIDAD DE MALAGA               DEPARTAMENTO DE TECNOLOGIA ELECTRONICA */
/* http://www.uma.es                                    http://www.dte.uma.es */
/* ========================================================================== */
/* PROGRAMA :  PWM-Servo                                                      */
/* VERSION  : 1.0                                                             */
/* TARGET   : Kit  TIVA Launchpad IDE CCSv7                                   */
/* DESCRIPCION : Este programas genera dos salidas PWM a través de los        */
/* terminales PF2 y PF3 usando el Timer adecuado en modo PWM, o un modulo PWM */
/*  Al pulsar los botones de la placa, deberña aumentar/reducir el ciclo de   */
/*  trabajo, provocando un aumento/reducción de la velocidad e incluso cambio */
/*  sentido                                                                   */
/* RECURSOS :                                                                 */
/* AUTOR    : Ignacio Herrero Reder (iherrero@uma.es)                         */
/* FECHA    : 08/10/17                                                        */
/* COMENTARIOS  : 1 tabulador = 8 espacios                                    */
                
/* **************************************************************************	*/
#include <stdint.h>
#include <stdbool.h>
// Librerias que se incluyen tipicamente para configuracion de perifericos y pinout
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h" 
// Libreria de control del sistema
#include "driverlib/sysctl.h"
// Incluir librerias de periférico y otros que se vaya a usar para control PWM y gestión
// de botones  (TODO)

/* **************************************************************************	*/
/* (ver cabecera en el código del programa)                                    	*/                   
/* **************************************************************************	*/
#include <stdint.h>
#include <stdbool.h>
// Librerias que se incluyen tipicamente para configuracion de perifericos y pinout
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h" 
// Libreria de control del sistema
#include "driverlib/sysctl.h"
// Incluir librerias de periférico y otros que se vaya a usar para control PWM y gestión
// de botones  (TODO)
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "drivers/buttons.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"


#define PERIOD_PWM 100000	// TODO: Ciclos de reloj para conseguir una señal periódica de 50Hz (según reloj de periférico usado)
#define COUNT_1MS 5000// TODO: Ciclos para amplitud de pulso de 1ms (max velocidad en un sentido)
#define STOPCOUNT 7600  // TODO: Ciclos para amplitud de pulso de parada (1.52ms)
#define COUNT_2MS  10000 // TODO: Ciclos para amplitud de pulso de 2ms (max velocidad en el otro sentido)
#define NUM_STEPS 50    // Pasos para cambiar entre el pulso de 2ms al de 1ms
#define CYCLE_INCREMENTS (abs(COUNT_1MS-COUNT_2MS))/NUM_STEPS  // Variacion de amplitud tras pulsacion

uint32_t valor = STOPCOUNT; // Valor actual de los motores
uint32_t valor2 = STOPCOUNT;
bool down = 0;
bool up = 1;
bool down2 = 1;
bool up2 = 0;

int main(void){
    //uint32_t ui32Period;
    // Elegir reloj adecuado para los valores de ciclos sean de tamaño soportable
    SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //20MHz

    ButtonsInit();

    //Configuracion Modulo PWM
    SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // habilita pwm
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // habilita gpio f

    GPIOPinConfigure(GPIO_PF2_M1PWM6); // configuracion de puerto como pwm
    GPIOPinConfigure(GPIO_PF3_M1PWM7); // configuracion de puerto como pwm

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);

    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PERIOD_PWM);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, valor);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, valor2);

    PWMGenEnable(PWM1_BASE, PWM_GEN_3);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);

    //Configuracion BOTONES
    //CONFIGURACION PUERTOS DE GPIO BOTONES INTERRUPCIONES FNM/ACL
        GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
        GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0, GPIO_FALLING_EDGE);
        GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);
        GPIOIntEnable(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);
        IntEnable(INT_GPIOF);
        IntMasterEnable();


  // Codigo principal, (poner en bucle infinito o bajo consumo)
    while(1){};
}

// Rutinas de interrupción de pulsadores
void RutinaISR(void){
    uint32_t status;

    status= GPIOIntStatus(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);

    if (status & (GPIO_PIN_4)){         // Boton Izquierdo: modifica  ciclo de trabajo en CYCLE_INCREMENTS para el servo conectado a PF2, subiendo o bajando

        if(down & valor-CYCLE_INCREMENTS > COUNT_1MS){
            valor -= CYCLE_INCREMENTS;
        }else{
           down = 0;
           up = 1;
        }

        if(up & (valor+CYCLE_INCREMENTS < COUNT_2MS)){
            valor += CYCLE_INCREMENTS;
        }else{
            down = 1;
            up = 0;
        }

        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, valor);
    }

    if (status & GPIO_PIN_0){        //Boton Derecho: modifica  ciclo de trabajo en CYCLE_INCREMENTS para el servo conectado a PF3, subiendo o bjando

        if(down2 & (valor2-CYCLE_INCREMENTS > COUNT_1MS)){
            valor2 -= CYCLE_INCREMENTS;
        }else{
            down2 = 0;
            up2 = 1;
        }

        if(up2 & (valor2+CYCLE_INCREMENTS < COUNT_2MS)){
            valor2 += CYCLE_INCREMENTS;
        }else{
            down2 = 1;
            up2 = 0;
        }

        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, valor2);
    }

    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
};




