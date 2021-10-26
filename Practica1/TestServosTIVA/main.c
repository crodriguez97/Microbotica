/* **************************************************************************
    Testea la calibración de los servos

    Genera dos ondas PWM a 50Hz (20ms) por los pines PF2 y PF3.
    La PWM por PF3 tiene un ciclo de trabajo fijo de 5ms
    La PWM por PF2 se inicia con un ciclo de trabajo de 1.5ms (servo parado o prácticamente parado)
    Al pulsar los botones el ciclo aumenta/disminuye en 60us
    Los márgenes teóricos del servo están entre 1ms y 2ms.

    Si nada mas empezar el servo esta prácticamente parado (o se para con 2-3 pulsaciones de uno u otro botón), se
    considera calibrado. Si no, se tendrá que abrir y calibrar mediante el potenciómetro interno.

  **************************************************************************   */
#include <stdint.h>
#include <stdbool.h>
// Librerias que s incluyen tipicamente para configuracion de perifericos y pinout
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
// Libreria de control del sistema
#include "driverlib/sysctl.h"
// Incluir librerias del periférico que se vaya a usar para control PWM
#include "driverlib/gpio.h" // NHR: Libreria GPIO
#include "driverlib/pwm.h"  // NHR: Libreria PWM
#include "drivers/buttons.h" // NHR
#include "driverlib/interrupt.h" // NHR
#include "inc/hw_ints.h" // NHR
#include "main.h"




int main(void){

    //Set the clock
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // Reloj del sistema a 4MHz (PLL-200MHz/50=40MHz)
    SysCtlClockSet(SYSCTL_SYSDIV_50 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |SYSCTL_OSC_MAIN); //configuro reloj del sistema
    // Configura pulsadores placa TIVA (int. por flanco de bajada)
    ButtonsInit();
    GPIOIntTypeSet(GPIO_PORTF_BASE, ALL_BUTTONS,GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTF_BASE,ALL_BUTTONS);
    IntEnable(INT_GPIOF);
    // Configuracion  ondas PWM: frecuencia 20Hz, anchura inicial= valor STOPCOUNT, 1520us para salida por PF0, y 1000uS para salida por PF4.
    configServos();
    // Codigo principal, (poner en bucle infinito o bajo consumo)
    while(1);
}
// Rutinas de interrupción de pulsadores

void GPIOFIntHandler(void)
{
    int32_t i32Status = GPIOIntStatus(GPIO_PORTF_BASE,ALL_BUTTONS);
    // Boton Izquierdo: reduce ciclo de trabajo en CYCLE_INCREMENTS para el servo conectado a PF4, hasta llegar a MINCOUNT
    if(((i32Status & LEFT_BUTTON) == LEFT_BUTTON)){
        ui32DutyCycle1+=cycle_changes;
        cambiaCiclo(ui32DutyCycle1);
    }
    // Boton Derecho: aumenta ciclo de trabajo en CYCLE_INCREMENTS para el servo conectado a PF4, hasta llegar a MAXCOUNT
    if(((i32Status & RIGHT_BUTTON) == RIGHT_BUTTON)){
        ui32DutyCycle1-=cycle_changes;
        cambiaCiclo(ui32DutyCycle1);
    }
    GPIOIntClear(GPIO_PORTF_BASE,ALL_BUTTONS);  //limpiamos flags
}

