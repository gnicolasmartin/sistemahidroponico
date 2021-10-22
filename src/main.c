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
#include "esp_partition.h"
#include "esp_spiffs.h"

extern TaskHandle_t task_handle_1, task_handle_2, task_handle_3, task_handle_4, task_handle_5;

void app_main()
{
    //Se llama a las funciones de inicialización
    nvs_flash_init();
    init_gpio();
    adc_init();
    lcd_Init();

    lcd_SendString("Bienvenido\\a", LCD_ROW_1);
	lcd_SendString("Ingresar al menú->", LCD_ROW_2);

    //Se crean las tareas a despachar por el scheduler
    xTaskCreate(&toggle_led, "toggle_led", 1024, NULL, 1, &task_handle_1);
    xTaskCreate(&leer_entradas, "leer_entradas", 1024, NULL, 1, &task_handle_2);
    xTaskCreate(&leer_adc_ec, "leer_adc_ec", 4096, NULL, 2, &task_handle_3);
    xTaskCreate(&navegar_menu, "navegar_menu", 4096, NULL, 2, &task_handle_4);
    xTaskCreate(&control_lcd, "control_lcd", 4096, NULL, 2, &task_handle_5);    
    xTaskCreate(&toggle_pin,"toggle_pin", 4096, NULL, 1, NULL);
    //xTaskCreate(&tarea2, "tarea2", 1024, NULL, 2, NULL);
    //xTaskCreate(&tarea3, "tarea3", 1024, NULL, 3, NULL);
    //xTaskCreate(&tarea4, "tarea4", 1024, NULL, 4, NULL);

    //vTaskStartScheduler();
}
