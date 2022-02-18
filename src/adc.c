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
const adc_channel_t channel_ec = ADC_CHANNEL_0, channel_ph = ADC_CHANNEL_3;
const adc_bits_width_t width = ADC_WIDTH_BIT_12;
const adc_atten_t atten_ec = ADC_ATTEN_DB_11, atten_ph = ADC_ATTEN_DB_11;
const adc_unit_t unit_ec = ADC_UNIT_1, unit_ph = ADC_UNIT_1;

extern uint32_t ph;
extern uint32_t ec;
uint32_t PH_MIN;
uint32_t PH_MAX;
uint32_t EC_MIN;
uint32_t EC_MAX;

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

void medir_ph(void)
{
    uint32_t adc_reading = 0;
    
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit_ph == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel_ph);
        } else {
            int raw;
            adc2_get_raw((adc2_channel_t)channel_ph, width, &raw);
            adc_reading += raw;
        }
        usleep(10000);
    }
    
    adc_reading /= NO_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    float voltage_ph = ((float) esp_adc_cal_raw_to_voltage(adc_reading, adc_chars_ph))/1000;
    ph = (uint32_t) -6.718671*voltage_ph + 21.78108;

    printf("Raw: %d\tVoltage: %fV\t PH: %d\n", adc_reading, voltage_ph, ph);
}

void medir_ec(void)
{
    uint32_t adc_reading = 0;
    float aux;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit_ec == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel_ec);
        } else {
            int raw;
            adc2_get_raw((adc2_channel_t)channel_ec, width, &raw);
            adc_reading += raw;
        }
    }
    
    adc_reading /= NO_OF_SAMPLES;

    aux = 2044959000 + (520.3848 - 2044959000)/(1 + pow((adc_reading/13859.89),6.790912));

    //Convert adc_reading to voltage in mV
    float voltage_ec = ((float)esp_adc_cal_raw_to_voltage(adc_reading, adc_chars_ec));

    ec = (uint32_t) aux;   
    
    printf("Raw: %d\tVoltage: %fmV\t EC: %dppm\n", adc_reading, voltage_ec, ec);
}

int analizar_ph(void)
{
    if(ph > PH_MIN && ph < PH_MAX)
    {
        return REGULATED;
    }
    
    return DESREGULATED;
}

int analizar_ec(void)
{
    if(ec > EC_MIN && ec < EC_MAX)
    {
        return REGULATED;
    }
    
    return DESREGULATED;
}