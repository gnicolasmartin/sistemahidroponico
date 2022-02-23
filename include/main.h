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
#include "times.h"
#include <esp_task_wdt.h>

// PROTOTYPES
extern TaskHandle_t task_handler_firestore;
extern TaskHandle_t task_handler_regulate_water;
extern TaskHandle_t task_handler_measure_habitat;
extern TaskHandle_t task_handler_input;
extern TaskHandle_t task_handler_menu;
extern TaskHandle_t task_handler_lcd;
extern TaskHandle_t task_handler_motor;
extern uint32_t PH_MAX, PH_MIN, EC_MAX, EC_MIN;
extern bool WIFI_IS_CONNECTED;




