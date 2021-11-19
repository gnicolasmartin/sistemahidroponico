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
extern TaskHandle_t task_handler_firestore;
extern TaskHandle_t task_handler_adc;
extern TaskHandle_t task_handler_input;
extern TaskHandle_t task_handler_menu;
extern TaskHandle_t task_handler_lcd;
extern TaskHandle_t task_handler_motor;

// ↓ LEVANTARLOS DEL CONFIG FILE EN UN FUTURO ↓
#define PUMP_TIME_ON            600  // 10min =   600seg
#define PUMP_TIME_OFF           3600  //   1hs =  3600seg
#define PH_TIME_OFF             30  //  24hs = 86400seg
#define EC_TIME_OFF             30  //  24hs = 86400seg
#define HUMIDITY_TIME_OFF       30  //  24hs = 86400seg
#define TEMPERATURE_TIME_OFF    30  //  24hs = 86400seg
#define DISPLAY_INACTIVITY      10  //  5min =   300seg
#define H20_TIME_OFF            10  //  24hs = 86400seg
#define H20_TIME_ON             10



