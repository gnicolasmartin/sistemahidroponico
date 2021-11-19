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
    gpio_pad_select_gpio(SENSOR_NIVEL_SEC);
    gpio_set_direction(SENSOR_NIVEL_SEC,GPIO_MODE_INPUT);

    //Inicialización de pulsadores
    gpio_pad_select_gpio(GPIO_PULSADOR_DERECHO);
    gpio_set_direction(GPIO_PULSADOR_DERECHO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_PULSADOR_DERECHO, GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(GPIO_PULSADOR_IZQUIERDO);
    gpio_set_direction(GPIO_PULSADOR_IZQUIERDO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_PULSADOR_IZQUIERDO, GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(GPIO_PULSADOR_ARRIBA);
    gpio_set_direction(GPIO_PULSADOR_ARRIBA,GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_PULSADOR_ARRIBA, GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(GPIO_PULSADOR_ABAJO);
    gpio_set_direction(GPIO_PULSADOR_ABAJO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_PULSADOR_ABAJO, GPIO_PULLUP_ONLY);

    //Inicialización de LED de prueba
    gpio_pad_select_gpio(GPIO_TEST_LED);
    gpio_set_direction(GPIO_TEST_LED,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_COOL_1);
    gpio_set_direction(GPIO_COOL_1,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_COOL_2);
    gpio_set_direction(GPIO_COOL_2,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_BOMBA_PRINCIPAL);
    gpio_set_direction(GPIO_BOMBA_PRINCIPAL,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_CALEFACTOR);
    gpio_set_direction(GPIO_CALEFACTOR,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_DOSIF_1);
    gpio_set_direction(GPIO_DOSIF_1,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_DOSIF_2);
    gpio_set_direction(GPIO_DOSIF_2,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_DOSIF_3);
    gpio_set_direction(GPIO_DOSIF_3,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_BRAZO_SONDAS);                 
    gpio_set_direction(GPIO_BRAZO_SONDAS,GPIO_MODE_OUTPUT);  

    gpio_pad_select_gpio(DIR_BRAZO_SONDAS);                 
    gpio_set_direction(DIR_BRAZO_SONDAS,GPIO_MODE_OUTPUT);  

    gpio_pad_select_gpio(GPIO_ALIMENTACION_AUX);                 
    gpio_set_direction(GPIO_ALIMENTACION_AUX,GPIO_MODE_OUTPUT); 

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
        entradas_antirrebote[i].estado_actual=1;      
        entradas_antirrebote[i].estado_anterior=1;
        entradas_antirrebote[i].level=1;     
    }
}

// Control del NEMA17 (Motor para movimiento de sondas)
void motor_sonda(int dir)
{
    uint8_t i = 0;  
    // 2 i -> 1 paso -> 1.8 grados
    // 100 i -> 50 pasos -> 90 grados
    
    gpio_set_level(DIR_BRAZO_SONDAS, dir);

    while(i < DEGREE_90_NEMA17)
    {
        if( i % 2 == 0 )
        {
            gpio_set_level(GPIO_BRAZO_SONDAS, 0);
            // printf("CAMBIO DE ESTADO 0\n");
        }
        else
        {
            gpio_set_level(GPIO_BRAZO_SONDAS, 1);
            // printf("CAMBIO DE ESTADO 1\n");
        }
        
        usleep(50000); // dormimos 50ms (velocidad de giro)
    
        i++;
    }
}

void regular_ph(void)
{
    // Activar el GPIO de la bomba de acido acidulante
}

void regular_ec(void)
{
    // Activar el GPIO de la bomba de solucion A y B
}