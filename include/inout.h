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
#define GPIO_DOSIF_SOLUCION_A   16
#define GPIO_DOSIF_SOLUCION_B   17
#define GPIO_DOSIF_ACIDULANTE   5   // DRIVER AL LADO DEL NEMA17
// CALEFACTOR            
#define GPIO_CALEFACTOR         13
// ALIMENTACION SONDAS            
#define GPIO_ALIMENTACION_AUX   23
// MOTOR SONDAS            
#define DIR_BRAZO_SONDAS        15  // DRIVER DE LA IZQUIERDA
#define GPIO_BRAZO_SONDAS       18
#define DEGREE_90_NEMA17        100
#define DEGREE_180_UP            1
#define DEGREE_180_DOWN          0
#define PERIOD_uSEG_DOSIF       10425 // 1 ml cada 6 min
#define PERIOD_SEG_DOSIF        0.010425
// DHT11            
#define GPIO_SENSOR_TEMP        4

#define ON                      1
#define OFF                     0

// PROTOTYPES
void init_antirrebote(void);
void gpio_init(void);
void dht11_init(void);
uint8_t dht11_check_response(void);
void motor_sonda(int dir);
void motor_dosificador(int dosificador);
uint8_t dht11_read(void);
int get_dht11_data(int datos);

// STRUCTS
typedef struct antirrebote_t{
    uint32_t estado_actual;
    uint32_t estado_anterior;
    uint32_t contador;
    uint32_t level;
}antirrebote_t;
