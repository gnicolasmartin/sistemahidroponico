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

#include "main.h"

esp_vfs_spiffs_conf_t conf = 
{
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
};

uint8_t init_ok = 0;
bool CROP_RUNNING = false; // TEST: Volver a False
bool SMOKE_TEST = false;
int  TEST_STATE = OFF;
char tipo_planta[50];

bool IRRIGATION_ON= false;
bool MIX_ON= false;

void app_main()
{   
    /************ Initialization ************/
    init_rom();
    fs_init(&conf);
    wifi_init();    // primero inicializar el FS para poder levantar el archivo de configuración del wifi

    gpio_init();    
    adc_init();
    lcd_init();
    dht11_init();

    /************** Variables **************/
    // Timers declaration
    int timer_pump, timer_habitat, timer_display, timer_regulate_water, timer_light, timer_fs_state_check;

    // Timers initialization
    // Quizas no conviene inicializalos en CERO para no tener que esperar 24hs a que corra... ¿Y si empiezan en su valor maximo?¿Correrian todas las tareas juntas?
    timer_pump= timer_habitat= timer_display= timer_regulate_water= timer_light= 0;
    timer_fs_state_check= 0;//STATE_CHECK_DELAY;
    // timer_light= LIGHT_TIME_OFF - 10;


    /*********** Task Declaration **********/
    // Start running
    xTaskCreate(&leer_botones, "leer_botones", 10240, NULL, 1, &task_handler_input);
    xTaskCreate(&navegar_menu, "navegar_menu", 10240, NULL, 2, &task_handler_menu);
    
    // Start suspended
    xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handler_lcd);
    vTaskSuspend(task_handler_lcd);
    xTaskCreate(&regulate_water, "regulate_water", 4096, NULL, 2, &task_handler_regulate_water);
    vTaskSuspend(task_handler_regulate_water);
    xTaskCreate(&measure_habitat, "measure_habitat", 4096, NULL, 1, &task_handler_measure_habitat);
    vTaskSuspend(task_handler_measure_habitat);

    // EN DUDA SI QUEDAN O NO
    // xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, NULL);
    // xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, &task_handler_firestore);
    // vTaskSuspend(task_handler_firestore);

    
    /************** Main loop - IDLE **************/  
    while(!CROP_RUNNING)
    {
        timer_fs_state_check++;     

        // Se activa desde el menú de navegación
        if(SMOKE_TEST)
        {            
            switch (TEST_STATE)
            {
                // -> TESTING LIGHTS
                case LIGHTS:
                    //printf("SMOKE TEST ACTIVATED!\n");
                    // vTaskResume(task_handler_lcd);
                    // gpio_set_level(GPIO_LIGHT, ON);
                    // gpio_set_level(GPIO_COOLERS_LIGHT, ON);
                    // sleep(25);
                    // gpio_set_level(GPIO_LIGHT, OFF);
                    // gpio_set_level(GPIO_COOLERS_LIGHT, OFF);
                    sleep(10);
                    TEST_STATE= COOLERS;
                    
                    vTaskResume(task_handler_lcd);
                    // PH_MAX= 90; 
                    // PH_MIN= 60; 
                    // EC_MAX= 2300; 
                    // EC_MIN= 1700;
                    // ESTO TIENE QUE IR! LO COMENTO PARA EL VIDEO
                    // vTaskResume(task_handler_regulate_water);
                    break;

                // -> TESTING REGULATE_WATER
                case REGULATE_WATER:      
                    // ------------------------------
                    // ESTO NO TIENE QUE IR! LO DEJO PARA EL VIDEO
                    sleep(10);
                    TEST_STATE= DHT11;
                    vTaskResume(task_handler_lcd);
                    // vTaskResume(task_handler_measure_habitat);
                    // ------------------------------

                    // ESTO TIENE QUE IR! LO COMENTO PARA EL VIDEO
                    // if(eTaskGetState(task_handler_regulate_water) == eSuspended)
                    // {
                    //     TEST_STATE= DHT11;
                    //     vTaskResume(task_handler_lcd);
                    //     vTaskResume(task_handler_measure_habitat);
                    // }
                    // break;

                 // -> TESTING DHT11
                case DHT11:
                    // vTaskResume(task_handler_measure_habitat);
                    // if(eTaskGetState(task_handler_measure_habitat) == eSuspended)
                    // {
                    //     TEST_STATE= COOLERS;
                    //     vTaskResume(task_handler_lcd);
                    // }
                    sleep(10);
                    TEST_STATE= LIGHTS;
                    vTaskResume(task_handler_lcd);
                    break;

                // -> TESTING COOLERS
                case COOLERS:
                    vTaskResume(task_handler_lcd);
                    // gpio_set_level(GPIO_COOLERS_HABITAT, ON);
                    // sleep(25);
                    // gpio_set_level(GPIO_COOLERS_HABITAT, OFF);
                    sleep(10);
                    TEST_STATE= OFF;
                    break;
                
                default:
                    vTaskResume(task_handler_lcd);
                    SMOKE_TEST= false;
                    TEST_STATE= OFF;
                    printf("SMOKE TEST FINISHED!\n");
                    break;
            }
        }
        else
        {
            // Cuando se cumple el tiempo seteado en la constante, llama a la función que realiza el chequeo del estado en firestore
            if(WIFI_IS_CONNECTED && timer_fs_state_check > STATE_CHECK_DELAY)
            {
                printf("VALIDAMOS EL ESTADO IDLE\n");
                timer_fs_state_check = 0;
                if(fs_check_state(SYSTEM_ID, tipo_planta) > 0)
                {
                    printf("Cultivando %s...\n",tipo_planta);
                    fs_check_limits(tipo_planta);
                    fs_stats_actualization(SYSTEM_ID);
                    CROP_RUNNING = true;
                }
            }
        }

        sleep(1);
    }

    /************** Main loop - RUNNING **************/    
    while(CROP_RUNNING)
    {
        // Increments timers value
        timer_pump++;
        timer_light++;
        timer_habitat++;
        timer_display++;
        timer_regulate_water++;

        /** CONTROL TASK: DISPLAY **/
        if(timer_display > DISPLAY_INACTIVITY)    // APAGADO
        {
            // Quizas la tarea leer_botones puede reiniciar el timer_display cada vez que presionan un boton...

            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_display == DISPLAY_INACTIVITY)
                printf("Apagamos display\n");   // ¿VCC DEL LCD = 0V?
        }

        /** CONTROL TASK: IRRIGATION **/
        if(timer_pump < PUMP_TIME_OFF)    // APAGADO
        {
            IRRIGATION_ON= false;

            if(!MIX_ON)
            {
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
            }
            
            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_pump == 1)
                printf("Apagamos riego\n"); 
        }
        else if(timer_pump < PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
        {
            IRRIGATION_ON= true;
            
            gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
            
            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_pump == PUMP_TIME_OFF)
                printf("Prendemos riego\n");
        }
        else
        {
            timer_pump= 0;
        }

        /** CONTROL TASK: LIGHT **/
        if(timer_light < LIGHT_TIME_OFF)    // APAGADO
        {
            gpio_set_level(GPIO_LIGHT, OFF);
            gpio_set_level(GPIO_COOLERS_LIGHT, OFF);
            
            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_light == 1)
                printf("Apagamos luz\n"); 
        }
        else if(timer_light < LIGHT_TIME_OFF + LIGHT_TIME_ON) // ENCENDIDO
        {
            gpio_set_level(GPIO_LIGHT, ON);
            gpio_set_level(GPIO_COOLERS_LIGHT, ON);

            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_light == LIGHT_TIME_OFF)
                printf("Prendemos luz\n");
        }
        else
        {
            timer_light= 0;
        }

        /** CONTROL TASK: WATER MEASURE **/
        if(timer_regulate_water > REGULATE_WATER_TIME_OFF)  // TIME ON
        {                  
            vTaskResume(task_handler_regulate_water);

            if(eTaskGetState(task_handler_regulate_water) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
            {
                printf("SE AUTOSUSPENDIO LA TAREA\n");
                timer_regulate_water= 0;                
            }
        }

        /** CONTROL TASK: HABITAT MEASURE **/
        if(timer_habitat > HABITAT_TIME_OFF)  // TIME ON
        {                  
            vTaskResume(task_handler_measure_habitat);

            if(eTaskGetState(task_handler_measure_habitat) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
            {
                printf("SE AUTOSUSPENDIO LA TAREA\n");
                timer_habitat= 0;                
            }
        }
    
        sleep(1); // Base de tiempo 1 segundo
    } 

    exit(EXIT_FAILURE);
}
