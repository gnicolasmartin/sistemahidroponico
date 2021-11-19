/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: adc.c                                                                     // 
//  Descripción: Contiene todas las funciones asociadas al manejo del adc              //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "adc.h"

esp_adc_cal_characteristics_t *adc_chars_ec, *adc_chars_ph;
const adc_channel_t channel_ec = ADC_CHANNEL_7, channel_ph = ADC_CHANNEL_6;
const adc_bits_width_t width = ADC_WIDTH_BIT_12;
const adc_atten_t atten_ec = ADC_ATTEN_DB_11, atten_ph = ADC_ATTEN_DB_11;
const adc_unit_t unit_ec = ADC_UNIT_1, unit_ph = ADC_UNIT_1;

void adc_init(void)
{
    //Configura el ADC
    if (unit_ec == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel_ec, atten_ec);
        adc1_config_channel_atten(channel_ph, atten_ph);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel_ec, atten_ec);
    }

    //Caracteriza el adc
    adc_chars_ec = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    adc_chars_ph = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type_ec = esp_adc_cal_characterize(unit_ec, atten_ec, width, DEFAULT_VREF, adc_chars_ec);
    print_char_val_type(val_type_ec);
    esp_adc_cal_value_t val_type_ph = esp_adc_cal_characterize(unit_ph, atten_ph, width, DEFAULT_VREF, adc_chars_ph);
    print_char_val_type(val_type_ph);

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
