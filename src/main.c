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

void app_main()
{
    //Se llama a las funciones de inicialización
    nvs_flash_init();
    init_gpio();
    init_wifi();
    wifi_wait();

    //Se crean las tareas a despachar por el scheduler
    //xTaskCreate(&toggle_led, "toggle_led", 10240, NULL, 3, NULL);
    xTaskCreate(&leer_entradas,"leer_entradas", 10240, NULL, 3, NULL);
    xTaskCreate(&firestore_task,"firestore", 10240, NULL, 4, NULL);
    //xTaskCreate(&idle, "idle", 1024, NULL, 5, NULL);
    //xTaskCreate(&tarea2, "tarea2", 1024, NULL, 2, NULL);
    //xTaskCreate(&tarea3, "tarea3", 1024, NULL, 3, NULL);
    //xTaskCreate(&tarea4, "tarea4", 1024, NULL, 4, NULL);


    //vTaskStartScheduler();
}
