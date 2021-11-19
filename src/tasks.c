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

uint8_t machine_state = STATE_INIT;
TaskHandle_t task_handler_motor, task_handler_firestore, task_handler_regulate_water, task_handler_input, task_handler_menu, task_handler_lcd;

//Variables para el manejo de menúes
uint8_t pagina_menu=0;
uint8_t opt_menu=0;

//Vectores para configuraciones de WiFi y variables
char SSID[16]="                ";
char PWD[16]="                ";
uint8_t cursor=0;
char caracter='0';
extern uint8_t wifi_connected;
extern uint8_t init_ok;

// Maquina de estados del sistema hidroponico
void state_machine(void *pvParameter)
{
    // uint32_t timer_pump= 0;
    // // uint32_t timer_sensado_on = 0;
    // // uint8_t motor_sonda_status = 0;
    // while (RUNNING) 
    // {
    //     printf("El valor del timer es: %d\n", timer_pump);
    //     /** CONTROL TASK: IRRIGATION **/
    //     if(timer_pump <= PUMP_TIME_OFF)    // APAGADO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
    //         timer_pump++;
    //     }
    //     else if(timer_pump <= PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
    //         timer_pump++;
    //     }
    //     else
    //     {
    //         timer_pump= 0;
    //     }

    //     /** CONTROL TASK: PH **/
    //     if(timer_ph <= PH_TIME_OFF)    // TIME OFF
    //     {
    //         timer_ph++;        
    //     }
    //     else  // TIME ON
    //     {
    //         PH_STATE= RUNNING;
            
    //         if(PH_STATE == RUNNING)
    //         {   
    //             timer_ph++;
    //         }
    //         else    // FINISHED
    //         {
    //             vTaskSuspend();
    //             timer_ph= 0;
    //         }
            
    //     }
        
    //     /** CONTROL TASK: EC **/
    //     if(timer_pump <= PUMP_TIME_OFF)    // APAGADO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
    //         timer_pump++;
    //     }
    //     else if(timer_pump <= PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
    //         timer_pump++;
    //     }
    //     else
    //     {
    //         timer_pump= 0;
    //     }

    //     /** CONTROL TASK: HUMIDITY **/
    //     if(timer_pump <= PUMP_TIME_OFF)    // APAGADO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
    //         timer_pump++;
    //     }
    //     else if(timer_pump <= PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
    //         timer_pump++;
    //     }
    //     else
    //     {
    //         timer_pump= 0;
    //     }

    //     /** CONTROL TASK: TEMPERATURE **/
    //     if(timer_temperature <= PUMP_TIME_OFF)    // APAGADO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
    //         timer_pump++;
    //     }
    //     else if(timer_pump <= PUMP_TIME_OFF + PUMP_TIME_ON) // ENCENDIDO
    //     {
    //         gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
    //         timer_pump++;
    //     }
    //     else
    //     {
    //         timer_pump= 0;
    //     }


        // printf("Funcionando en modo CONEXION\n");
        // //vTaskResume(task_handler_regulate_water);        

        // if(motor_sonda_status == 0)
        //     timer_sensado_on++;
        
        // if(timer_sensado_on == 30)
        // {
        //     timer_sensado_on = 0;
        //     // vTaskResume(task_handler_motor); // DESCOMENTAR PARA QUE FUNCIONE EL NEMA17
        // }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}

//Tarea que lee las entradas implementando antirrebote por Software
void leer_botones(void *pvParameter)
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
                    entradas_antirrebote[i].estado_actual = gpio_get_level(SENSOR_NIVEL_SEC);
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

        vTaskDelay(10 / portTICK_PERIOD_MS);
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

void regular_agua(void *pvParameter)
{
    motor_sonda(DEGREE_90_DOWN);
    gpio_set_level(GPIO_ALIMENTACION_AUX, ON);

    sleep(SONDA_STABILIZATION_TIME);
    static int ESTADO= MEASURE;

    while (1) 
    {
        switch(ESTADO)
        {
            case MEASURE:

                medir_ph();
                medir_ec();

                ESTADO= ANALYZE;

            break;

            case ANALYZE:
                
                if(analizar_ph() == DESREGULATED)
                {
                    ESTADO= REGULATE_PH;
                }
                else if(analizar_ec() == DESREGULATED)
                {
                    ESTADO= REGULATE_EC;
                }
                else
                {
                    ESTADO= REGULATED;
                }

            break;

            case REGULATE_PH:
                
                regular_ph();
                ESTADO= MIX_WATER;

            break;

            case REGULATE_EC:

                regular_ec();
                ESTADO= MIX_WATER;

            break;

            case MIX_WATER:

                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
                sleep(WATER_STABILIZATION_TIME/5);
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
                sleep(WATER_STABILIZATION_TIME/5);
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
                sleep(WATER_STABILIZATION_TIME/5);
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
                sleep(WATER_STABILIZATION_TIME/5);
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);
                sleep(WATER_STABILIZATION_TIME/5);

                gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);

                ESTADO= MEASURE;

            break;

            case REGULATED:

                motor_sonda(DEGREE_90_UP);
                gpio_set_level(GPIO_ALIMENTACION_AUX, OFF);
                vTaskSuspend(NULL); // Tarea "regular agua" se autosuspende

            break;
        }
    }
}

//Tarea utilizada para navegar en los menúes del display
void navegar_menu(void *pvParameter)
{
    int i;
    while (1) {
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "DERECHA"
        if(!entradas_antirrebote[1].level)
        {
            //Si el programa está posicionado en el primer menú de opciones
            //-->Si está seleccionada la opción "Configuración WiFi"
            if(pagina_menu==1 && opt_menu==0)
            {  
                //-->Ingresa a la Configuración de WiFi
                opt_menu=0;
                pagina_menu=2;
                vTaskResume(task_handler_lcd);
            }
            //-->Si está seleccionada la opción "Ver Status"
            else if(pagina_menu==1 && opt_menu==1)
            {
                //-->Ingresa a ver la cantidad de días de cosecha y la cantidad restante estimada
                opt_menu=0;
                pagina_menu=3;
                vTaskResume(task_handler_lcd);
            }
            //Si el programa está posicionado en el menú de WiFi
            //-->Si está seleccionada la opción "Configuración de SSID"            
            else if(pagina_menu==2 && opt_menu==0)
            {
                // escanea las redes que hay disponibles
                wifi_scan();
                // printf("Las redes son %s\n", WIFI_SSIDS[0]);

                //Posiciona el cursor en el primer elemento del vector de seteo e ingresa a la configuración de SSID
                opt_menu=0;
                cursor=0;
                caracter=SSID[0];
                pagina_menu=4;
                vTaskResume(task_handler_lcd);
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
                vTaskResume(task_handler_lcd);
            }   
            //Si se encuentra en la pantalla principal, accede al primer menú           
            else if(pagina_menu==0)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
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
                vTaskResume(task_handler_lcd);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "IZQUIERDA"
        if(!entradas_antirrebote[2].level)
        {
            //Navega desde el primer menú hacia la pantalla principal
            if(pagina_menu==1)
            {
                opt_menu=0;
                pagina_menu=0;
                vTaskResume(task_handler_lcd);
            }
            //Navega desde la configuración WiFi hacia el primer menú
            if(pagina_menu==2)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
            //Navega desde "Status" hacia el primer menú
            if(pagina_menu==3)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
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
                wifi_init();
                vTaskResume(task_handler_lcd);
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
                wifi_init();
                vTaskResume(task_handler_lcd);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ARRIBA"
        if(!entradas_antirrebote[3].level)
        {
            if(pagina_menu==1 || pagina_menu==2)
            {
                //En menúes de dos opciones selecciona la opción superior
                if(opt_menu==0)
                {
                    opt_menu=1; 
                    vTaskResume(task_handler_lcd); 
                }
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter--;
                if(caracter=='0')
                    caracter='y';
                vTaskResume(task_handler_lcd);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ABAJO"
        if(!entradas_antirrebote[4].level)
        {
            if(pagina_menu==1 || pagina_menu==2)
            {
                //En menúes de dos opciones selecciona la opción inferior
                if(opt_menu==1)
                {
                    opt_menu=0;
                    vTaskResume(task_handler_lcd);
                }    
            }
            if(pagina_menu==4 || pagina_menu==5)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter++;
                if(caracter=='y')
                    caracter='0';
                vTaskResume(task_handler_lcd);
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
                vTaskSuspend(task_handler_lcd);
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
                vTaskSuspend(task_handler_lcd);
            } 
            if(opt_menu==1)
            {
                printf(" Configurar WiFi\n");
                printf(">Ver Status\n");
                lcd_send_string(" Configurar WiFi", LCD_ROW_1);
	            lcd_send_string(">Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handler_lcd);
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
                vTaskSuspend(task_handler_lcd);
            }
            if(opt_menu==1)
            {
                printf(" Configurar SSID\n");
                printf(">Configurar PWD\n");
                lcd_send_string(" Configurar SSID", LCD_ROW_1);
	            lcd_send_string(">Configurar PWD", LCD_ROW_2);
                vTaskSuspend(task_handler_lcd);
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
            vTaskSuspend(task_handler_lcd);       
        }
        if(pagina_menu==4)
        {            
            lcd_send_command(0x01);
            SSID[cursor]=caracter;            
            lcd_send_string("Introducir SSID:", LCD_ROW_1);
	        lcd_send_string(SSID, LCD_ROW_2);
            printf("Introducir SSID:\n");
            printf("%s\n", SSID);
            vTaskSuspend(task_handler_lcd);            
        }
        if(pagina_menu==5)
        {
            lcd_send_command(0x01);
            PWD[cursor]=caracter;
            lcd_send_string("Introducir PWD:", LCD_ROW_1);
	        lcd_send_string(PWD, LCD_ROW_2);
            printf("Introducir PWD:\n");
            printf("%s\n", PWD);
            vTaskSuspend(task_handler_lcd);            
        }
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