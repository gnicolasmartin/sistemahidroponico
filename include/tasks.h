#include "inout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc_utils.h"
#include "lcd_i2c.h"
#include <string.h>

#define MAX_PAGINAS 3

void toggle_led(void *pvParameter);
void toggle_pin(void *pvParameter);
void leer_entradas(void *pvParameter);
void leer_adc_ph(void *pvParameter);
void leer_adc_ec(void *pvParameter);
void navegar_menu(void *pvParameter);
void control_lcd(void *pvParameter);
