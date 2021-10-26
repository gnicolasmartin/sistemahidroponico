#include <stdio.h>

#include "esp_system.h"
#include "nvs_flash.h"
/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: lcd_i2c.h                                                                 // 
//  Descripción: Archivo de cabecera para el main                                      //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES
#include "tasks.h"
#include "wifi.h"
#include "filesystem.h"

// PROTOTYPES
extern TaskHandle_t task_handler_1;
extern TaskHandle_t task_handler_2;
extern TaskHandle_t task_handler_3;
extern TaskHandle_t task_handler_4;
extern TaskHandle_t task_handler_5;



