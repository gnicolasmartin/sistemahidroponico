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
#include <unistd.h>
#include <math.h>

// DEFINES
#define DEFAULT_VREF             1100
#define NO_OF_SAMPLES            64 
#define SONDA_STABILIZATION_TIME 60    // in seconds
#define WATER_STABILIZATION_TIME 300   // in seconds (5 min)
// State Machine"regular_agua" task
enum{
    INIT,
    MEASURE,
    ANALYZE,
    REGULATE_PH,
    REGULATE_EC,
    MIX_WATER,
    END
};
// Status of measurement
#define REGULATED               0        
#define DESREGULATED            1

// Valores de parametros (ESTO TIENE QUE IR EN UN ARCHIVO DE CONFIGURACION! Y TIENE QUE ACTUALIZARSE LOS VALORES DEFAULT CON LAS BASE DE DATOS)
// Lechuga
// Tomate
// Acelga
// Espinaca
#define SELECTED_PLANT  0 // ESTE VALOR PODRIA SER DINAMICO Y QUE LO LEVANTE DE LA BASE DE DATOS!
#define CANT_PLANTAS    4 // ESTE VALOR PODRIA SER DINAMICO Y QUE LO LEVANTE DE LA BASE DE DATOS!
#define LECHUGA         0
#define TOMATE          1
#define ACELGA          2
#define ESPINACA        3

// PROTOTYPES
void print_char_val_type(esp_adc_cal_value_t);
void adc_init(void);
void medir_ph(void);
void medir_ec(void);
int analizar_ph(void);
int analizar_ec(void);
