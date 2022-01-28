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

    gpio_pad_select_gpio(GPIO_DOSIF_SOLUCION_A);
    gpio_set_direction(GPIO_DOSIF_SOLUCION_A,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_DOSIF_SOLUCION_B);
    gpio_set_direction(GPIO_DOSIF_SOLUCION_B,GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_DOSIF_ACIDULANTE);
    gpio_set_direction(GPIO_DOSIF_ACIDULANTE,GPIO_MODE_OUTPUT);

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

void dht11_init(void)
{
    gpio_pad_select_gpio(GPIO_SENSOR_TEMP);                 
    gpio_set_direction(GPIO_SENSOR_TEMP,GPIO_MODE_OUTPUT);

    gpio_set_level(GPIO_SENSOR_TEMP, 1);
    usleep(4000);

    gpio_set_level(GPIO_SENSOR_TEMP, 0);
    usleep(18000);

    gpio_set_level(GPIO_SENSOR_TEMP, 1);
    usleep(30);

    gpio_pad_select_gpio(GPIO_SENSOR_TEMP);                 
    gpio_set_direction(GPIO_SENSOR_TEMP,GPIO_MODE_INPUT);

    if(dht11_check_response())
        printf("DHT11 INIT OK\n");
    else
        printf("DHT11 INIT FAILED\n");
/*
    gpio_pad_select_gpio(GPIO_SENSOR_TEMP);      
    gpio_set_direction(GPIO_SENSOR_TEMP, GPIO_MODE_OUTPUT);
    //gpio_set_level(GPIO_SENSOR_TEMP,1);
    //ets_delay_us(4000);

    gpio_set_level(GPIO_SENSOR_TEMP,0);
    //vTaskDelay(36 / portTICK_RATE_MS);
    ets_delay_us(22000);
    gpio_set_level(GPIO_SENSOR_TEMP,1);
    ets_delay_us(43);
    gpio_pad_select_gpio(GPIO_SENSOR_TEMP);      
    gpio_set_direction(GPIO_SENSOR_TEMP, GPIO_MODE_INPUT);*/
}


uint8_t dht11_check_response(void)
{
    uint8_t i, Flag_response = 0;
    usleep(40);

    if(!gpio_get_level(GPIO_SENSOR_TEMP))
    {
        usleep(80);
        if(gpio_get_level(GPIO_SENSOR_TEMP))
        {
            Flag_response = 1;
        }
        else
            return 0;
    }


    for(i=0;i<200;i++)
    {
        if(!gpio_get_level(GPIO_SENSOR_TEMP))
        {
            Flag_response = 1;
            break;
        }
        else
        {
            Flag_response = 0;
        }
        usleep(1);
    }

    return Flag_response;
}

uint8_t dht11_read(void)
{
    uint8_t i,j, error, reading=0;
    for(j=0;j<8;j++)
    {
        error=1;
        for(i=0;i<120;i++)
        {
            usleep(1);
            if(gpio_get_level(GPIO_SENSOR_TEMP))
            {
                error=0;
                break;
            }
        }
        if(error==1)
        {
            printf("ERROR EN LECTURA\n");
            return 0;
        }

        usleep(40);
        if(!gpio_get_level(GPIO_SENSOR_TEMP))
        {
            reading &= ~(1<<(7-j));
        }
        else
            reading |= (1<<(7-j));
        
        error=1;
        for(i=0;i<120;i++)
        {
            usleep(1);
            if(!gpio_get_level(GPIO_SENSOR_TEMP))
            {
                error=0;
                break;
            }
        }
        if(error==1)
        {
            printf("LECTURA NO OK\n");
            return 0;
        }
    }
    return reading;
}

// Control del NEMA17 (Motor para movimiento de sondas)
void motor_sonda(int dir)
{
    int i = 0;  
    // 2 i -> 1 paso -> 1.8 grados
    // 100 i -> 50 pasos -> 90 grados
    gpio_set_level(DIR_BRAZO_SONDAS, dir);

    while(i < 2.66*DEGREE_90_NEMA17)
    {
        if( i % 2 == 0 )
        {
            gpio_set_level(GPIO_BRAZO_SONDAS, OFF);
            // printf("CAMBIO DE ESTADO 0\n");
        }
        else
        {
            gpio_set_level(GPIO_BRAZO_SONDAS, ON);
            // printf("CAMBIO DE ESTADO 1\n");
        }
        
        usleep(50000); // dormimos 50ms (velocidad de giro)
    
        i++;
    }
}

// Control de los dosificadores
void motor_dosificador(int dosificador)
{
    int i = 0;  
    // 2 i -> 1 paso -> 1.8 grados
    // 100 i -> 50 pasos -> 90 grados
    while(i < TIME_uSEG_FOR_1ML)
    {
        if( i % 2 == 0 )
        {
            gpio_set_level(dosificador, OFF);
            // printf("CAMBIO DE ESTADO 0\n");
        }
        else
        {
            gpio_set_level(dosificador, ON);
            // printf("CAMBIO DE ESTADO 1\n");
        }
        
        usleep(PERIOD_uSEG_DOSIF); // dormimos 10,425ms (velocidad de giro)
    
        i+= PERIOD_uSEG_DOSIF;
    }
}