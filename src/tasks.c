/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //                          
//                                                                                     // 
//  Archivo: tasks.c                                                                   // 
//  Descripción: Contiene todas las implementaciones de las tareas conmutadas por      // 
//  el sistema operativo.                                                              //     
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "tasks.h"

//Estructura de entradas antirrebote
extern antirrebote_t entradas_antirrebote[CANTIDAD_ANTIRREBOTE];

//Datos de ADC
extern esp_adc_cal_characteristics_t *adc_chars;
extern const adc_channel_t channel;
extern const adc_bits_width_t width;
extern const adc_atten_t atten;
extern const adc_unit_t unit;
uint8_t estado_motor=0;
TaskHandle_t task_handle_1, task_handle_2, task_handle_3, task_handle_4, task_handle_5;

//Variables para el manejo de menúes
uint8_t pagina_menu=0;
uint8_t opt_menu=0;

//Vectores para configuraciones de WiFi y variables
char SSID[16]="                ";
char PWD[16]="                ";
uint8_t cursor=0;
char caracter='0';
char SSID_seteado[16];
char PWD_seteado[16];

//Tarea que lee las entradas implementando antirrebote por Software
void leer_entradas(void *pvParameter)
{
    uint32_t i;
    while(1)
    {
        //Barre cada una de las entradas que se accionan con dispositivos de contacto
        for(i=0;i<CANTIDAD_ANTIRREBOTE;i++)
        {
            //Switch - Case que selecciona la entrada a leer
            switch(i)
            {
                case 0:
                    entradas_antirrebote[i].estado_actual = gpio_get_level(GPIO_SENSOR_NIVEL);
                    break;
                case 1:
                    entradas_antirrebote[i].estado_actual = gpio_get_level(GPIO_PULSADOR_DERECHO);
                    break;
                case 2:
                    entradas_antirrebote[i].estado_actual = gpio_get_level(GPIO_PULSADOR_IZQUIERDO);
                    break;
                case 3:
                    entradas_antirrebote[i].estado_actual = gpio_get_level(GPIO_PULSADOR_ARRIBA);
                    break;
                case 4:
                    entradas_antirrebote[i].estado_actual = gpio_get_level(GPIO_PULSADOR_ABAJO);
                    break;
                default:
                    break;
            }

            //Si el estado actual leído, coincide con el anterior, se incrementa un contador.
            //Al llegar el mismo a 20, se asigna el estado definitivo
            if(entradas_antirrebote[i].estado_actual == entradas_antirrebote[i].estado_anterior)
            {
                entradas_antirrebote[i].contador++;
                if(entradas_antirrebote[i].contador >= 20)
                {
                    entradas_antirrebote[i].contador=0;
                    entradas_antirrebote[i].estado_anterior=entradas_antirrebote[i].estado_actual;
                    entradas_antirrebote[i].level=entradas_antirrebote[i].estado_actual;                  
                }
            }
            else
            {
                    entradas_antirrebote[i].estado_anterior = entradas_antirrebote[i].estado_actual;
                    entradas_antirrebote[i].contador=0;
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

//Dummy para probar GPIO
void toggle_led(void *pvParameter)
{
    while(1)
    {
        if(entradas_antirrebote[0].level)
            gpio_set_level(GPIO_TEST_LED,0);
        else
            gpio_set_level(GPIO_TEST_LED,1);
        printf("Cambiando valor de led\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void toggle_pin(void *pvParameter)
{
    while(1)
    {
        if(estado_motor == 0)
        {
            estado_motor = 1;
            gpio_set_level(19,0);
        }
        else
        {
            estado_motor = 0;    
            gpio_set_level(19,1);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void leer_adc_ph(void *pvParameter)
{
    while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        uint32_t ph_value = 23.3 - 0.008*adc_reading;
        printf("Raw: %d\tVoltage: %dmV\t PH: %d\n", adc_reading, voltage, ph_value);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void leer_adc_ec(void *pvParameter)
{
    while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        printf("Raw: %d\tVoltage: %dmV\t\n", adc_reading, voltage);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

//Tarea utilizada para navegar en los menúes del display
void navegar_menu(void *pvParameter)
{
    int i;
    while (1) {
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "DERECHA"
        if(entradas_antirrebote[1].level)
        {
            //Si el programa está posicionado en el primer menú de opciones
            //-->Si está seleccionada la opción "Configuración WiFi"
            if(pagina_menu==1 && opt_menu==0)
            {  
                //-->Ingresa a la Configuración de WiFi
                opt_menu=0;
                pagina_menu=2;
                vTaskResume(task_handle_5);
            }
            //-->Si está seleccionada la opción "Ver Status"
            else if(pagina_menu==1 && opt_menu==1)
            {
                //-->Ingresa a ver la cantidad de días de cosecha y la cantidad restante estimada
                opt_menu=0;
                pagina_menu=3;
                vTaskResume(task_handle_5);
            }
            //Si el programa está posicionado en el menú de WiFi
            //-->Si está seleccionada la opción "Configuración de SSID"            
            else if(pagina_menu==2 && opt_menu==0)
            {
                //Posiciona el cursor en el primer elemento del vector de seteo e ingresa a la configuración de SSID
                opt_menu=0;
                cursor=0;
                caracter=SSID[0];
                pagina_menu=4;
                vTaskResume(task_handle_5);
            }       
            //Si el programa está posicionado en el menú de WiFi
            //-->Si está seleccionada la opción "Configuración de PWD"             
            else if(pagina_menu==2 && opt_menu==1)
            {
                //Posiciona el cursor en el primer elemento del vector de seteo e ingresa a la configuración de SSID
                opt_menu=0;
                opt_menu=0;
                cursor=0;
                caracter=PWD[0];
                pagina_menu=5;
                vTaskResume(task_handle_5);
            }   
            //Si se encuentra en la pantalla principal, accede al primer menú           
            else if(pagina_menu==0)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handle_5);
            }
            //Si se encuentra en la configuración de SSID o PWD mueve el cursor
            else if(pagina_menu==4 || pagina_menu==5)
            {
                cursor++;
                caracter='0';
                if(cursor>15)
                {
                    cursor=0;
                }
                vTaskResume(task_handle_5); 
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "IZQUIERDA"
        if(entradas_antirrebote[2].level)
        {
            //Navega desde el primer menú hacia la pantalla principal
            if(pagina_menu==1)
            {
                opt_menu=0;
                pagina_menu=0;
                vTaskResume(task_handle_5);
            }
            //Navega desde la configuración WiFi hacia el primer menú
            if(pagina_menu==2)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handle_5);
            }
            //Navega desde "Status" hacia el primer menú
            if(pagina_menu==3)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handle_5);
            }
            //Navega desde la configuración de SSID hacia la configuración WiFi
            if(pagina_menu==4)
            {
                opt_menu=0;
                pagina_menu=2;
                //Busca el final del SSID seteado y almacena el valor en char * que se usará para la conexión
                for(i=0;i<16;i++)
                {
                    if(SSID[i]==' ')
                    {
                        SSID[i]='\0';
                    }
                }
                strcpy(SSID_seteado,SSID);
                printf("Se seteó el SSID: %s\n",SSID_seteado);                
                vTaskResume(task_handle_5);
            }
            //Navega desde la configuración de PWD hacia la configuración WiFi
            if(pagina_menu==5)
            {
                opt_menu=0;
                pagina_menu=2;
                //Busca el final del PWD seteado y almacena el valor en char * que se usará para la conexión
                for(i=0;i<16;i++)
                {
                    if(PWD[i]==' ')
                    {
                        PWD[i]='\0';
                    }
                }
                strcpy(PWD_seteado,PWD);
                printf("Se seteó el PWD: %s\n",PWD_seteado);                
                vTaskResume(task_handle_5);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ARRIBA"
        if(entradas_antirrebote[3].level)
        {
            if(pagina_menu==1 || pagina_menu==2)
            {
                //En menúes de dos opciones selecciona la opción superior
                if(opt_menu==0)
                {
                    opt_menu=1; 
                    vTaskResume(task_handle_5);   
                }
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter--;
                if(caracter=='0')
                    caracter='y';
                vTaskResume(task_handle_5);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ABAJO"
        if(entradas_antirrebote[4].level)
        {
            if(pagina_menu==1 || pagina_menu==2)
            {
                //En menúes de dos opciones selecciona la opción inferior
                if(opt_menu==1)
                {
                    opt_menu=0;
                    vTaskResume(task_handle_5);
                }    
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter++;
                if(caracter=='y')
                    caracter='0';
                vTaskResume(task_handle_5);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

//Tarea dedicada a refrescar las opciones de menú según la máquina de estado
void control_lcd(void *pvParameter)
{
    while (1) {
        if(pagina_menu==0)
        {
            lcd_SendCommand(0x01);
            if(opt_menu==0)
            {
                lcd_SendString("Bienvenido", LCD_ROW_1);
	            lcd_SendString(">Ingresar al menu", LCD_ROW_2);
                vTaskSuspend(task_handle_5);
            }
        }
        if(pagina_menu==1)
        {
            lcd_SendCommand(0x01);
            if(opt_menu==0)
            {
                lcd_SendString(">Configurar WiFi", LCD_ROW_1);
	            lcd_SendString(" Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handle_5);
            } 
            if(opt_menu==1)
            {
                lcd_SendString(" Configurar WiFi", LCD_ROW_1);
	            lcd_SendString(">Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handle_5);
            } 
        }
        if(pagina_menu==2)
        {
            lcd_SendCommand(0x01);
            if(opt_menu==0)
            {
                lcd_SendString(">Configurar SSID", LCD_ROW_1);
	            lcd_SendString(" Configurar PWD", LCD_ROW_2);
                vTaskSuspend(task_handle_5);
            }
            if(opt_menu==1)
            {
                lcd_SendString(" Configurar SSID", LCD_ROW_1);
	            lcd_SendString(">Configurar PWD", LCD_ROW_2);
                vTaskSuspend(task_handle_5);
            } 
        }
        if(pagina_menu==3)
        {
            lcd_SendCommand(0x01);    
            lcd_SendString("Dias de cosecha: ", LCD_ROW_1);
	        lcd_SendString("Días restantes: ", LCD_ROW_2);     
            vTaskSuspend(task_handle_5);       
        }
        if(pagina_menu==4)
        {
            lcd_SendCommand(0x01);
            SSID[cursor]=caracter;            
            lcd_SendString("Introducir SSID:", LCD_ROW_1);
	        lcd_SendString(SSID, LCD_ROW_2);
            vTaskSuspend(task_handle_5);            
        }
        if(pagina_menu==5)
        {
            lcd_SendCommand(0x01);
            PWD[cursor]=caracter;
            lcd_SendString("Introducir PWD:", LCD_ROW_1);
	        lcd_SendString(PWD, LCD_ROW_2);
            vTaskSuspend(task_handle_5);            
        }
    }
}