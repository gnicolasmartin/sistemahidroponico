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

void app_main()
{   
    /************ Initialization ************/
    nvs_flash_init();
    gpio_init();
    adc_init();
    lcd_init();
    fs_init(&conf); 
    wifi_init();    // primero inicializar el FS para poder levantar el archivo de configuración del wifi
    //wifi_wait();

    /************** Variables **************/
    // Timers
    int timer_pump, timer_ph, timer_ec, timer_humidity, timer_temperature, timer_display;
    timer_pump= timer_ph= timer_ec= timer_humidity= timer_temperature= timer_display= 0;
    // Quizas no conviene inicializalos en CERO para no tener que esperar 24hs a que corra... ¿Y si empiezan en su valor maximo?¿Correrian todas las tareas juntas?
    // States
    bool ph_task_on, ec_task_on, humidity_task_on, temperature_task_on;
    ph_task_on= ec_task_on= humidity_task_on= temperature_task_on= false;

    /*********** Task Declaration **********/
    // Start running
    xTaskCreate(&leer_entradas, "leer_entradas", 1024, NULL, 1, &task_handler_input);
    xTaskCreate(&navegar_menu, "navegar_menu", 10240, NULL, 1, &task_handler_menu);
    xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handler_lcd);
    // Start suspended
    // xTaskCreate(&motor_sonda,"motor_sonda", 4096, NULL, 2, &task_handler_motor); // DESCOMENTAR PARA QUE FUNCIONE EL NEMA17
    // vTaskSuspend(task_handler_motor);    // DESCOMENTAR PARA QUE FUNCIONE EL NEMA17
    xTaskCreate(&leer_adc, "leer_adc", 4096, NULL, 2, &task_handler_adc);
    // vTaskSuspend(task_handler_adc);
    // xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, &task_handler_firestore);
    // vTaskSuspend(task_handler_firestore);

    // EN DUDA SI QUEDAN O NO
    //xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, NULL);
    // xTaskCreate(&state_machine,"state_machine", 10240, NULL, 3, NULL); 
    


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

        /** CONTROL TASK: DISPLAY **/
        if(timer_display > DISPLAY_INACTIVITY)    // APAGADO
        {
            // Quizas la tarea leer_entradas puede reiniciar el timer_display cada vez que presionan un boton...

            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_display == DISPLAY_INACTIVITY)
                printf("Apagamos display\n");   // ¿VCC DEL LCD = 0V?
        }

        /** CONTROL TASK: IRRIGATION **/
        if(timer_pump < PUMP_TIME_OFF)    // APAGADO
        {
            // gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);

            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_pump == 1)
                printf("Apagamos riego\n"); 
        }
        else if(timer_pump < PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
        {
            // gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
            
            // ↓ BORRAR LO QUE VENGA APARTIR DE AHORA ↓
            if (timer_pump == PUMP_TIME_OFF)
                printf("Prendemos riego\n");
        }
        else
        {
            timer_pump= 0;
        }

    //     /** CONTROL TASK: PH **/
    //     if(timer_ph <= PH_TIME_OFF)    // TIME OFF
    //     {
    //         timer_ph++;        
    //     }
    //     else  // TIME ON
    //     {
    //         timer_ph++;
    //         vTaskResume(task_handler_adc);

    //         if(eTaskStateeTaskGetState(task_handler_adc) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
    //         {
    //             timer_ph= 0;                
    //         }
    //     }
        
    //     /** CONTROL TASK: EC **/
    //     if(timer_ec <= EC_TIME_OFF)    // TIME OFF
    //     {
    //         timer_ec++;        
    //     }
    //     else  // TIME ON
    //     {
    //         timer_ec++;
    //         vTaskResume(task_handler_adc);

    //         if(eTaskStateeTaskGetState(task_handler_adc) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
    //         {
    //             timer_ec= 0;                
    //         }
    //     }

    //     /** CONTROL TASK: HUMIDITY **/
    //     if(timer_humidity <= HUMIDITY_TIME_OFF)    // TIME OFF
    //     {
    //         timer_humidity++;        
    //     }
    //     else  // TIME ON
    //     {
    //         timer_humidity++;
    //         vTaskResume(task_handler_adc);

    //         if(eTaskStateeTaskGetState(task_handler_adc) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
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
    //         vTaskResume(task_handler_adc);

    //         if(eTaskStateeTaskGetState(task_handler_adc) == eSuspended) // TASK AUTO SUSPEND THEIR SELF WHEN FINISH
    //         {
    //             timer_temperature= 0;                
    //         }
    //     }
        sleep(1); // Base de tiempo 1 segundo
    } 

    exit(EXIT_FAILURE);
}
