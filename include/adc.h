/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: adc.h                                                                     // 
//  Descripción: Archivo de cabecera para el manejo del adc                            //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES
#include "driver/adc.h"
#include "esp_adc_cal.h"

// DEFINES
#define DEFAULT_VREF    1100
#define NO_OF_SAMPLES   64 

// PROTOTYPES
void print_char_val_type(esp_adc_cal_value_t);
void adc_init(void);