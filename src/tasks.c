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

uint8_t estado_motor=0;
TaskHandle_t task_handler_1, task_handler_2, task_handler_3, task_handler_4, task_handler_5;

//Variables para el manejo de menúes
uint8_t pagina_menu=0;
uint8_t opt_menu=0;

//Vectores para configuraciones de WiFi y variables
char SSID[16]="                ";
char PWD[16]="                ";
uint8_t cursor=0;
char caracter='0';

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
        {
            gpio_set_level(GPIO_TEST_LED,0);
            printf("Presionado 0\n");
        }
        else if(entradas_antirrebote[1].level)
        {
            gpio_set_level(GPIO_TEST_LED,0);
            printf("Presionado DERECHA\n");
        }
        else if(entradas_antirrebote[2].level)
        {
            gpio_set_level(GPIO_TEST_LED,0);
            printf("Presionado IZQUIERDA\n");
        }
        else if(entradas_antirrebote[3].level)
        {
            gpio_set_level(GPIO_TEST_LED,0);
            printf("Presionado ARRIBA\n");
        }
        else if(entradas_antirrebote[4].level)
        {
            gpio_set_level(GPIO_TEST_LED,0);
            printf("Presionado ABAJO\n");
        }
        else
        {
            gpio_set_level(GPIO_TEST_LED,1);
        }

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
                vTaskResume(task_handler_5);
            }
            //-->Si está seleccionada la opción "Ver Status"
            else if(pagina_menu==1 && opt_menu==1)
            {
                //-->Ingresa a ver la cantidad de días de cosecha y la cantidad restante estimada
                opt_menu=0;
                pagina_menu=3;
                vTaskResume(task_handler_5);
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
                vTaskResume(task_handler_5);
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
                vTaskResume(task_handler_5);
            }   
            //Si se encuentra en la pantalla principal, accede al primer menú           
            else if(pagina_menu==0)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_5);
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
                vTaskResume(task_handler_5); 
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
                vTaskResume(task_handler_5);
            }
            //Navega desde la configuración WiFi hacia el primer menú
            if(pagina_menu==2)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_5);
            }
            //Navega desde "Status" hacia el primer menú
            if(pagina_menu==3)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_5);
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
                //strcpy(SSID_seteado,SSID);
                sprintf(WIFI_SSID, "%s", SSID);
                printf("Se seteó el SSID: %s\n", SSID);   
                save_wifi_config(); 
                vTaskResume(task_handler_5);
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

                sprintf(WIFI_SSID, "%s", PWD);
                printf("Se seteó el PWD: %s\n", PWD);      
                save_wifi_config();        
                vTaskResume(task_handler_5);
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
                    vTaskResume(task_handler_5);   
                }
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter--;
                if(caracter=='0')
                    caracter='y';
                vTaskResume(task_handler_5);
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
                    vTaskResume(task_handler_5);
                }    
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter++;
                if(caracter=='y')
                    caracter='0';
                vTaskResume(task_handler_5);
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
            lcd_send_command(0x01);
            if(opt_menu==0)
            {
                printf("Bienvenido\n");
                printf(">Ingresar al menu\n");
                lcd_send_string("Bienvenido", LCD_ROW_1);
	            lcd_send_string(">Ingresar al menu", LCD_ROW_2);
                vTaskSuspend(task_handler_5);
            }
        }
        if(pagina_menu==1)
        {
            lcd_send_command(0x01);
            if(opt_menu==0)
            {
                printf(">Configurar WiFi\n");
                printf(" Ver Status\n");
                lcd_send_string(">Configurar WiFi", LCD_ROW_1);
	            lcd_send_string(" Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handler_5);
            } 
            if(opt_menu==1)
            {
                printf(" Configurar WiFi\n");
                printf(">Ver Status\n");
                lcd_send_string(" Configurar WiFi", LCD_ROW_1);
	            lcd_send_string(">Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handler_5);
            } 
        }
        if(pagina_menu==2)
        {
            lcd_send_command(0x01);
            if(opt_menu==0)
            {
                printf(">Configurar SSID\n");
                printf(" Configurar PWD\n");
                lcd_send_string(">Configurar SSID", LCD_ROW_1);
	            lcd_send_string(" Configurar PWD", LCD_ROW_2);
                vTaskSuspend(task_handler_5);
            }
            if(opt_menu==1)
            {
                printf(" Configurar SSID\n");
                printf(">Configurar PWD\n");
                lcd_send_string(" Configurar SSID", LCD_ROW_1);
	            lcd_send_string(">Configurar PWD", LCD_ROW_2);
                vTaskSuspend(task_handler_5);
            } 
        }
        if(pagina_menu==3)
        {
            load_wifi_config(); //Prueba borrar
            printf("Dias de cosecha: \n");
            printf("Días restantes: \n");
            lcd_send_command(0x01);    
            lcd_send_string("Dias de cosecha: ", LCD_ROW_1);
	        lcd_send_string("Días restantes: ", LCD_ROW_2);     
            vTaskSuspend(task_handler_5);       
        }
        if(pagina_menu==4)
        {            
            lcd_send_command(0x01);
            SSID[cursor]=caracter;            
            lcd_send_string("Introducir SSID:", LCD_ROW_1);
	        lcd_send_string(SSID, LCD_ROW_2);
            printf("Introducir SSID:\n");
            printf("%s\n", SSID);
            vTaskSuspend(task_handler_5);            
        }
        if(pagina_menu==5)
        {
            lcd_send_command(0x01);
            PWD[cursor]=caracter;
            lcd_send_string("Introducir PWD:", LCD_ROW_1);
	        lcd_send_string(PWD, LCD_ROW_2);
            printf("Introducir PWD:\n");
            printf("%s\n", PWD);
            vTaskSuspend(task_handler_5);            
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// Update values in firestore database 
void firestore_task(void *pvParameter)
{
    static uint32_t u32DocLength;
    static char tcDoc[FIRESTORE_DOC_MAX_SIZE];

    init_firestore();
    
    while(1)
    {
        if(entradas_antirrebote[0].level)   // 3V3 
        {
            // Apagamos el LED
            gpio_set_level(GPIO_TEST_LED,0);
            
            u32DocLength = snprintf(tcDoc, sizeof(tcDoc), " ");

            // Leemos de firestore un documento
            firestore_err_t FIRESTORE_STATUS= firestore_get_document(PLANTS_COLLECTION_ID, PLANT_DOCUMENT_ID, tcDoc, &u32DocLength);

            // Chequeamos si hay error
            if(FIRESTORE_STATUS != FIRESTORE_OK)
            {
                printf("ERROR: Couldn't get document\n");
            }

            // Creamos un archivo JSON para respaldar el documento leido de firestore
            FILE* f = fopen("/spiffs/document.json", "w");    

            // Chequeamos si hay error
            if (f != NULL)                                   
            {
                // Guardamos el JSON en un archivo
                fprintf(f, "%s", tcDoc);
                fputc('\0', f);
                fclose(f); 
            }

            // Actualizamos el valor de un campo
            char valor[100]; 
            itoa(rand(), valor, 10);
            int value = replace_value("/spiffs/document.json", "temperature", valor);

            // Chequeamos si hay error
            if(value == -1)
            {
                printf("ERROR: Couldn´t replace value in JSON file\n");
            }

            // value = search_value("/spiffs/document.json", "temperature", valor);

            // if(value == -1)
            // {
            //      printf("ERROR: Couldn´t search value in JSON file\n");
            // }

            // printf("Leimos la key name y es: %s\n", valor);

            // Abrimos el JSON con el campo actualizado
            f = fopen("/spiffs/document.json", "r");

            // Pasamos el contenido del JSON a una variable
            fread(tcDoc, FIRESTORE_DOC_MAX_SIZE, 1, f);

            // Cerramos archivo JSON de respaldo
            fclose(f);

            // Actualizamos el documento en firestore
            FIRESTORE_STATUS= firestore_update_document(ESP_COLLECTION_ID, ESP_DOCUMENT_ID, tcDoc, &u32DocLength);
            
            // Chequeamos si hay error
            if(FIRESTORE_STATUS != FIRESTORE_OK)
            {
                printf("ERROR: Couldn't update document\n");
            }
            
        }
        else                                // GND
        {
            // Encendemos el LED
            gpio_set_level(GPIO_TEST_LED,1);
        }

        vTaskDelay(FIRESTORE_PERIOD_MS / portTICK_PERIOD_MS);
    }
}