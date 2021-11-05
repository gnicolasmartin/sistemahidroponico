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
#define SENSOR_NIVEL_PRI        36
#define SENSOR_NIVEL_SEC        39
#define GPIO_TEST_LED           2  // LED BUILT IN
#define CANTIDAD_ANTIRREBOTE    5
// Pulsadores del teclado
#define GPIO_PULSADOR_IZQUIERDO 25
#define GPIO_PULSADOR_DERECHO   26
#define GPIO_PULSADOR_ARRIBA    27
#define GPIO_PULSADOR_ABAJO     33
// COOLERS
#define GPIO_COOL_1             14
#define GPIO_COOL_2             12
// BOMBAS
#define GPIO_BOMBA_PRINCIPAL    19
#define GPIO_DOSIF_1            16
#define GPIO_DOSIF_2            17
#define GPIO_DOSIF_3            5
// CALEFACTOR            
#define GPIO_CALEFACTOR         13
// ALIMENTACION SONDAS            
#define GPIO_ALIMENTACION_AUX   23
// MOTOR SONDAS            
#define GPIO_BRAZO_SONDAS       18
// DHT11            
#define GPIO_SENSOR_TEMP        4


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
