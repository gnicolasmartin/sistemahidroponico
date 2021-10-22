#include "adc_utils.h"

esp_adc_cal_characteristics_t *adc_chars;
const adc_channel_t channel = ADC_CHANNEL_7;
const adc_bits_width_t width = ADC_WIDTH_BIT_12;
const adc_atten_t atten = ADC_ATTEN_DB_11;
const adc_unit_t unit = ADC_UNIT_1;

void adc_init(void)
{
    //Configura el ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Caracteriza el adc
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}
