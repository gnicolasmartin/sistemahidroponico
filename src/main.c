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
    //Se llama a las funciones de inicialización
    nvs_flash_init();
    gpio_init();
    //adc_init();
    lcd_init();
    fs_init(&conf); 
    wifi_init();    // primero inicializar el FS para poder levantar el archivo de configuración del wifi
    //wifi_wait();

    init_ok = 1;
    
    //Se crean las tareas a despachar por el scheduler
    //xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, NULL);
    xTaskCreate(&state_machine,"state_machine", 10240, NULL, 3, NULL); 
    xTaskCreate(&leer_entradas, "leer_entradas", 1024, NULL, 1, &task_handler_input);
    //xTaskCreate(&leer_adc_ec, "leer_adc_ec", 4096, NULL, 2, &task_handler_adc);
    xTaskCreate(&navegar_menu, "navegar_menu", 10240, NULL, 1, &task_handler_menu);
    xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handler_lcd);    
    xTaskCreate(&motor_sonda,"motor_sonda", 4096, NULL, 2, &task_handler_motor);
    //xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, &task_handler_firestore);

    vTaskSuspend(task_handler_input);
    vTaskSuspend(task_handler_menu);
    vTaskSuspend(task_handler_motor);    

    
    // All done, unmount partition and disable SPIFFS
    // esp_vfs_spiffs_unregister(conf.partition_label);
    // printf("SPIFFS unmounted\n");
    
    //vTaskStartScheduler();
}
