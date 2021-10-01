// INCLUDES
#include "inout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "json.h"

// DEFINES
#define READPORTS_PERIOD_MS 1
#define TOGGLED_PERIOD_MS   1000
#define FIRESTORE_PERIOD_MS 1000

// PROTOTYPES
void toggle_led(void *pvParameter);
void leer_entradas(void *pvParameter);
void firestore_task(void *pvParameter);
void idle(void *pvParameter);
