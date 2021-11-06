/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: lcd_i2c.h                                                                 // 
//  Descripción: Archivo de cabecera para las tareas                                   //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES
#include "inout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc.h"
#include "lcd_i2c.h"
#include "json.h"
#include "firestore.h"
#include "filesystem.h"
#include "wifi.h"
#include <string.h>

// DEFINES
#define READPORTS_PERIOD_MS     1
#define TOGGLED_PERIOD_MS       1000
#define FIRESTORE_PERIOD_MS     1000
#define MAX_PAGINAS             3
#define STATE_INIT              1
#define STATE_CONN              2
#define STATE_RUN_CONN          3
#define STATE_RUN_NOT_CONN      4
#define RUNNING                 1
#define ON                      1
#define OFF                     0

// PROTOTYPES
void toggle_led(void *pvParameter);
void motor_sonda(void *pvParameter);
void leer_entradas(void *pvParameter);
void leer_adc_ph(void *pvParameter);
void leer_adc_ec(void *pvParameter);
void navegar_menu(void *pvParameter);
void control_lcd(void *pvParameter);
void firestore_task(void *pvParameter);
void state_machine(void *pvParameter);

// EXTERNS
//Estructura de entradas antirrebote
extern antirrebote_t entradas_antirrebote[CANTIDAD_ANTIRREBOTE];
//Datos de ADC
extern esp_adc_cal_characteristics_t *adc_chars;
extern const adc_channel_t channel;
extern const adc_bits_width_t width;
extern const adc_atten_t atten;
extern const adc_unit_t unit;
//wifi
extern char WIFI_SSID[MAX_LENGTH_SSID]; 
extern char WIFI_PSWD[MAX_LENGTH_PSWD];


