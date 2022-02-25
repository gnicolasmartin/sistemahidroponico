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
#include "times.h"
#include <unistd.h>
#include "esp32/rom/ets_sys.h"

// DEFINES
#define SENSOR_NIVEL_PRI        26
#define SENSOR_NIVEL_SEC        27
#define GPIO_TEST_LED           2  // LED BUILT IN
#define CANTIDAD_ANTIRREBOTE    5
// Pulsadores del teclado
#define GPIO_PULSADOR_IZQUIERDO 25//35 // SETEAR EN 27 PARA HACER PRUEBAS EN PROTO (PULL UP INTENRO)
#define GPIO_PULSADOR_DERECHO   35//25
#define GPIO_PULSADOR_ARRIBA    32
#define GPIO_PULSADOR_ABAJO     33
// COOLERS
#define GPIO_COOLERS_LIGHT      15
#define GPIO_COOLERS_HABITAT    16
// BOMBAS
#define GPIO_BOMBA_PRINCIPAL    23
#define GPIO_DOSIF_SOLUCION_A   13
#define GPIO_DOSIF_SOLUCION_B   17
#define GPIO_DOSIF_ACIDULANTE   12  
// CALEFACTOR            
#define GPIO_LIGHT              19
// ALIMENTACION SONDAS            
// #define GPIO_ALIMENTACION_AUX   23
// MOTOR SONDAS            
#define DIR_BRAZO_SONDAS        18  // DRIVER DE LA IZQUIERDA
#define GPIO_BRAZO_SONDAS       5
#define DEGREE_90_NEMA17        100
#define DEGREE_180_UP           1
#define DEGREE_180_DOWN         0
#define PERIOD_uSEG_DOSIF       2606 // 1 ml cada 6 min
// DHT11            
#define GPIO_SENSOR_TEMP        14

#define ON                      1
#define OFF                     0

// PROTOTYPES
void init_antirrebote(void);
void gpio_init(void);
void dht11_init(void);
uint8_t dht11_start(void);
uint8_t dht11_check_response(void);
void motor_sonda(int dir);
void dispenser_ph();
void dispenser_ec();
uint8_t dht11_read(void);

// STRUCTS
typedef struct antirrebote_t{
    uint32_t estado_actual;
    uint32_t estado_anterior;
    uint32_t contador;
    uint32_t level;
}antirrebote_t;
