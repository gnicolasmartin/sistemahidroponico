#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100
#define NO_OF_SAMPLES   64 

void print_char_val_type(esp_adc_cal_value_t);
void adc_init(void);