#include "driver/gpio.h"

#define GPIO_SENSOR_NIVEL 27
#define GPIO_TEST_LED 2
#define GPIO_PULSADOR_IZQUIERDO 13
#define GPIO_PULSADOR_DERECHO 12
#define CANTIDAD_ANTIRREBOTE 3

void init_antirrebote(void);
void init_gpio(void);

typedef struct antirrebote_t{
    uint32_t estado_actual;
    uint32_t estado_anterior;
    uint32_t contador;
    uint32_t level;
}antirrebote_t;
