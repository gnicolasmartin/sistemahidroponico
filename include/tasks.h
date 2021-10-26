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
#include <string.h>

// DEFINES
#define READPORTS_PERIOD_MS 1
#define TOGGLED_PERIOD_MS   1000
#define FIRESTORE_PERIOD_MS 1000
#define MAX_PAGINAS 3

// PROTOTYPES
void toggle_led(void *pvParameter);
void toggle_pin(void *pvParameter);
void leer_entradas(void *pvParameter);
void leer_adc_ph(void *pvParameter);
void leer_adc_ec(void *pvParameter);
void navegar_menu(void *pvParameter);
void control_lcd(void *pvParameter);
void firestore_task(void *pvParameter);

// EXTERNS
//Estructura de entradas antirrebote
extern antirrebote_t entradas_antirrebote[CANTIDAD_ANTIRREBOTE];
//Datos de ADC
extern esp_adc_cal_characteristics_t *adc_chars;
extern const adc_channel_t channel;
extern const adc_bits_width_t width;
extern const adc_atten_t atten;
extern const adc_unit_t unit;

