/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: lcd_i2c.h                                                                 // 
//  Descripción: Archivo de cabecera para el manejo de LCD I2C                         //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// CONFIG DEFINES
#ifndef __LCD_I2C_H
#define __LCD_I2C_H

// INCLUDES
#include <stdio.h>
#include "driver/i2c.h"

// PROTOTYPES
void lcd_init (void);
void lcd_send_string (char *, int);
void lcd_send_command (uint8_t);
void lcd_send_data (uint8_t);
void i2c_init (void);
int lcd_SendInternal(uint8_t, uint8_t);
int lcd_WriteData(uint8_t, uint8_t *, int);

// DEFINES
#define I2C_MASTER_NUM             I2C_NUM_1        /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ         1000             /*!< I2C master clock frequency */
#define ACK_CHECK_EN               0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS              0x0              /*!< I2C master will not check ack from slave */
#define LCD_ADDR    (0x27)
#define LCD_DELAY_MS 5
#define LCD_DELAY_US 10000
// LCD instructions: https://mil.ufl.edu/3744/docs/lcdmanual/commands.html
#define LCD_CLEAR               0x01        // Replace all characters with ASCII 'space'
#define LCD_HOME                0x02        // Return cursor to first position on first line
#define LCD_DISPLAY_ON          0x0C        // Display on, cursor not displayed, don't blink character
//#define LCD_FUNCTION_SET_4BIT   0x30        // 4-bit data, 1-line display, 5x10 font
#define LCD_FUNCTION_SET_4BIT   0x08        // 4-bit data, 2-line display, 5x8 font
// Pin mappings
#define LCD_RS_CMD      (0 << 0)            // P0 -> RS
#define LCD_RS_DATA     (1 << 0)            // P0 -> RS
#define LCD_RW          (1 << 1)            // P1 -> RW
#define LCD_ENABLE      (1 << 2)            // P2 -> E
#define LCD_BACKLIGHT   (1 << 3)            // P3 -> Backlight
#define LCD_D4          (1 << 4)            // P4 -> D4
#define LCD_D5          (1 << 5)            // P5 -> D5
#define LCD_D6          (1 << 6)            // P6 -> D6
#define LCD_D7          (1 << 7)            // P7 -> D7
#define LCD_ROWS    2
#define LCD_COLS    16
#define LCD_ROW_1   0x80
#define LCD_ROW_2   0xC0
// Pins
#define I2C_SDA	    21
#define I2C_SCL	    22


#endif  /* __LCD_I2C_H */