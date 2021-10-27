/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //                          
//                                                                                     // 
//  Archivo: inout.c                                                                   // 
//  Descripción: Contiene todas las funciones asociadas al manejo de GPIO.             //   
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "inout.h"

volatile antirrebote_t entradas_antirrebote[CANTIDAD_ANTIRREBOTE];

//Función utilizada para inicializar las GPIO
void gpio_init(void)
{
    //Inicialización del sensor de nivel
    gpio_pad_select_gpio(GPIO_SENSOR_NIVEL);
    gpio_set_direction(GPIO_SENSOR_NIVEL,GPIO_MODE_INPUT);

    //Inicialización de pulsadores
    gpio_pad_select_gpio(GPIO_PULSADOR_DERECHO);
    gpio_set_direction(GPIO_PULSADOR_DERECHO,GPIO_MODE_INPUT);

    gpio_pad_select_gpio(GPIO_PULSADOR_IZQUIERDO);
    gpio_set_direction(GPIO_PULSADOR_IZQUIERDO,GPIO_MODE_INPUT);

    gpio_pad_select_gpio(GPIO_PULSADOR_ARRIBA);
    gpio_set_direction(GPIO_PULSADOR_ARRIBA,GPIO_MODE_INPUT);

    gpio_pad_select_gpio(GPIO_PULSADOR_ABAJO);
    gpio_set_direction(GPIO_PULSADOR_ABAJO,GPIO_MODE_INPUT);

    //Inicialización de LED de prueba
    gpio_pad_select_gpio(GPIO_TEST_LED);
    gpio_set_direction(GPIO_TEST_LED,GPIO_MODE_OUTPUT);

    //Inicialización de LED de prueba
    gpio_pad_select_gpio(19);
    gpio_set_direction(19,GPIO_MODE_OUTPUT);

    //Inicializa las estructuras antirrebote
    init_antirrebote();
}

//Inicialización de las entradas antirrebote
void init_antirrebote(void)
{
    uint32_t i;
    for(i=0;i<CANTIDAD_ANTIRREBOTE;i++)
    {
        entradas_antirrebote[i].contador=0;
        entradas_antirrebote[i].estado_actual=0;      
        entradas_antirrebote[i].estado_anterior=0;   
    }
}
