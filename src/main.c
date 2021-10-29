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

void app_main()
{
    //Se llama a las funciones de inicialización
    nvs_flash_init();
    gpio_init();
    //adc_init();
    lcd_init();
    // wifi_init();
    // wifi_wait();
    fs_init(&conf);

    lcd_send_string("Bienvenido\\a", LCD_ROW_1);
	lcd_send_string("Ingresar al menú->", LCD_ROW_2);

    //Se crean las tareas a despachar por el scheduler
    //xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, &task_handler_1);
    //xTaskCreate(&leer_entradas, "leer_entradas", 1024, NULL, 1, &task_handler_2);
    //xTaskCreate(&leer_adc_ec, "leer_adc_ec", 4096, NULL, 2, &task_handler_3);
    //xTaskCreate(&navegar_menu, "navegar_menu", 10240, NULL, 2, &task_handler_4);
    //xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handler_5);    
    xTaskCreate(&toggle_pin,"toggle_pin", 4096, NULL, 1, NULL);
    //xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, NULL);
    
    // All done, unmount partition and disable SPIFFS
    // esp_vfs_spiffs_unregister(conf.partition_label);
    // printf("SPIFFS unmounted\n");
    
    //vTaskStartScheduler();
}
