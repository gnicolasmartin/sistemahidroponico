/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: gpio.h                                                                    // 
//  Descripción: Archivo de cabecera para el manejo de GPIOs                           //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES
#include "driver/gpio.h"

// DEFINES
#define GPIO_SENSOR_NIVEL       18
#define GPIO_TEST_LED           2  // LED BUILT IN
#define CANTIDAD_ANTIRREBOTE    5
// Pulsadores del teclado
#define GPIO_PULSADOR_IZQUIERDO 25
#define GPIO_PULSADOR_DERECHO   26
#define GPIO_PULSADOR_ARRIBA    27
#define GPIO_PULSADOR_ABAJO     33

// PROTOTYPES
void init_antirrebote(void);
void gpio_init(void);

// STRUCTS
typedef struct antirrebote_t{
    uint32_t estado_actual;
    uint32_t estado_anterior;
    uint32_t contador;
    uint32_t level;
}antirrebote_t;
