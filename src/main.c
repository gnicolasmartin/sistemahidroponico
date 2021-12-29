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

bool IRRIGATION_ON= false;
bool MIX_ON= false;

void app_main()
{   
    /************ Initialization ************/
    nvs_flash_init();
    gpio_init();
    adc_init();
    lcd_init();
    fs_init(&conf); 
    wifi_init();    // primero inicializar el FS para poder levantar el archivo de configuración del wifi
    // wifi_wait();

    /************** Variables **************/
    // Timers declaration
    int timer_pump, timer_ph, timer_ec, timer_humidity, timer_temperature, timer_display, timer_regulate_water;

    // Timers initialization
    // Quizas no conviene inicializalos en CERO para no tener que esperar 24hs a que corra... ¿Y si empiezan en su valor maximo?¿Correrian todas las tareas juntas?
    timer_pump= timer_ph= timer_ec= timer_humidity= timer_temperature= timer_display= timer_regulate_water= 0;


    /*********** Task Declaration **********/
    // Start running
    xTaskCreate(&leer_botones, "leer_botones", 1024, NULL, 1, &task_handler_input);
    xTaskCreate(&navegar_menu, "navegar_menu", 10240, NULL, 1, &task_handler_menu);
    xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handler_lcd);
    // Start suspended
    xTaskCreate(&regular_agua, "regular_agua", 4096, NULL, 2, &task_handler_regulate_water);
    vTaskSuspend(task_handler_regulate_water);
    // xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, &task_handler_firestore);
    // vTaskSuspend(task_handler_firestore);

    // EN DUDA SI QUEDAN O NO
    //xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, NULL);
    
    /************** Main loop **************/
    while(RUNNING)
    {
        // Increments timers value
        timer_pump++;
        timer_ph++;
        timer_ec++;
        timer_humidity++;
        timer_temperature++;
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

    //     /** CONTROL TASK: HUMIDITY **/
    //     if(timer_humidity <= HUMIDITY_TIME_OFF)    // TIME OFF
    //     {
    //         timer_humidity++;        
    //     }
    //     else  // TIME ON
    //     {
    //         timer_humidity++;
    //         vTaskResume(task_handler_regulate_water);

    //         if(eTaskGetState (task_handler_regulate_water) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
    //         {
    //             timer_humidity= 0;                
    //         }
    //     }

    //     /** CONTROL TASK: TEMPERATURE **/
    //     if(timer_temperature <= TEMPERATURE_TIME_OFF)    // TIME OFF
    //     {
    //         timer_temperature++;        
    //     }
    //     else  // TIME ON
    //     {
    //         timer_temperature++;
    //         vTaskResume(task_handler_regulate_water);

    //         if(eTaskGetState (task_handler_regulate_water) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
    //         {
    //             timer_temperature= 0;                
    //         }
    //     }
        sleep(1); // Base de tiempo 1 segundo
    } 

    exit(EXIT_FAILURE);
}
