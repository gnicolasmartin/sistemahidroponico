/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //                          
//                                                                                     // 
//  Archivo: json.c                                                                    // 
//  Descripción: Contiene todas las funciones asociadas al manejo de JSONs.            //   
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "lcd_i2c.h"

/* Function-------------------------------------------------------------------*/
void lcd_init(void)
{
    // Init I2C
    i2c_init();
	lcd_send_command(LCD_FUNCTION_SET_4BIT);
	lcd_send_command(LCD_HOME);
	lcd_send_command(LCD_DISPLAY_ON);
	lcd_send_command(LCD_CLEAR);
}

void lcd_send_string(char *str, int row)
{
    lcd_send_command(row);
	while(*str)
	{
		lcd_send_data((uint8_t)(*str));
		str++;
	}
}

/* Private Functions ---------------------------------------------------------*/
void i2c_init(void)
{
    // Configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);

    // Install Driver
    i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
}

void lcd_send_command(uint8_t cmd)
{
	lcd_SendInternal(cmd, LCD_RS_CMD);
}

void lcd_send_data(uint8_t data)
{
	lcd_SendInternal(data, LCD_RS_DATA);
}

int lcd_SendInternal(uint8_t data, uint8_t mode)
{
	int res;
	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	data_arr[0] = up|mode|LCD_BACKLIGHT|LCD_ENABLE;
	data_arr[1] = (up|mode|LCD_BACKLIGHT)&~LCD_ENABLE;
	data_arr[2] = lo|mode|LCD_BACKLIGHT|LCD_ENABLE;
	data_arr[3] = (lo|mode|LCD_BACKLIGHT)&~LCD_ENABLE;

    res = lcd_WriteData(LCD_ADDR, data_arr, 4);

    vTaskDelay(LCD_DELAY_MS / portTICK_RATE_MS);
	return res;
}

int lcd_WriteData(uint8_t addr, uint8_t data[], int len)
{
    esp_err_t ret;
    i2c_cmd_handle_t hCmd = i2c_cmd_link_create();
    i2c_master_start(hCmd);
    i2c_master_write_byte(hCmd, ( addr << 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(hCmd, data, len, ACK_CHECK_EN);
    i2c_master_stop(hCmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, hCmd, 10/portTICK_RATE_MS);
    i2c_cmd_link_delete(hCmd);

    return ret;
}