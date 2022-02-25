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
TaskHandle_t task_handler_motor, task_handler_firestore, task_handler_regulate_water, task_handler_measure_habitat, task_handler_input, task_handler_menu, task_handler_lcd, task_handler_dosificador;
uint32_t temperature=25, humidity=70, ph=3, ec=1800;

//Variables para el manejo de menúes
uint8_t pagina_menu= 0;
uint8_t opt_menu= 0;
char test_msg[17]= "                 ";

//Vectores para configuraciones de WiFi y variables
char SSID[16]="                ";
char PWD[16]="                ";
uint8_t cursor=0;
char caracter='0';
extern uint8_t init_ok;

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
                    // if(!entradas_antirrebote[i].level)
                    //     printf("Cambio el estado del pulsador %d a: %d\n",i, entradas_antirrebote[i].level);                 
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

//Tarea que mide EC y PH para regularlos con las soluciones
void regulate_water(void *pvParameter)
{
    static int ESTADO= INIT;
    int result;

    while (1) 
    {
        switch(ESTADO)
        {
            case INIT:
                // Bajamos la sonda
                motor_sonda(DEGREE_180_DOWN);
                printf("BAJAMOS SONDA\n");
                // Alimentamos los sensores
                // gpio_set_level(GPIO_ALIMENTACION_AUX, ON);
                // printf("ALIMENTAMOS SONDA\n");
                // Tiempo de establecimiento
                printf("ESTABILIZAMOS SONDA\n");
                sleep(30); //SONDA_STABILIZATION_TIME

                ESTADO= MEASURE;
            break;

            case MEASURE:
                printf("MEASURE: PH\n");
                medir_ph();
                printf("MEASURE: EC\n");
                medir_ec();

                ESTADO= ANALYZE;
            break;

            case ANALYZE:
                printf("ANALYZE\n");
                
                result= analyse_ph();

                if(result == REGULATED)
                {
                    result= analyse_ec();
                    if(result == DESREGULATED)
                    {
                        ESTADO= REGULATE_EC;
                    }
                    else
                    {
                        ESTADO= END;
                    }
                }
                else if(result == DESREGULATED)
                {
                    ESTADO= REGULATE_PH;
                }
                else
                {
                    ESTADO= END;
                }

            break;

            case REGULATE_PH:
                printf("REGULATE_PH\n");
                // Activa la bomba de acido acidulante
                dispenser_ph();
                ESTADO= MIX_WATER;

            break;

            case REGULATE_EC:
                printf("REGULATE_EC SOLUCION A\n");
                // Activa la bomba de solucion A y B
                dispenser_ec();
                ESTADO= MIX_WATER;

            break;

            case MIX_WATER:
                printf("MIX_WATER\n");
                MIX_ON= true;
                
                // Enciende bomba de riego
                gpio_set_level(GPIO_BOMBA_PRINCIPAL, ON);                
                sleep(WATER_STABILIZATION_TIME);
                
                if(!IRRIGATION_ON)
                {
                    // Apaga bomba de riego
                    gpio_set_level(GPIO_BOMBA_PRINCIPAL, OFF);
                }

                MIX_ON= false;
                ESTADO= MEASURE;
            break;

            case END:
                // ACA TENDRIAMOS QUE AVISARLE AL USUARIO QUE ESTA TODO MAL...
                // OPCION 1 EL ESP ESCRIBE ALGUNA ALERTA EN FIRESTORE LA APP LO LEE Y ALERTA
                // OPCION 2 EL ESP ESCRIBE COMO SIEMPRE, LA APP LEE COMO SIEMPRE Y VERIFICA SI ESTA TODO OK. SINO ALERTA

                printf("END\n");
                // Levanta la sonda
                motor_sonda(DEGREE_180_UP);
                printf("LEVANTAMOS SONDA\n");
                // Apaga alimentacion de las sondas
                // gpio_set_level(GPIO_ALIMENTACION_AUX, OFF);
                // printf("DESALIMENTAMOS SONDA\n");

                if(fs_stats_actualization(SYSTEM_ID)>0)
                {
                    printf("SE ACTUALIZÓ INFO EN FIREBASE\n");
                }

                // Tarea "regular agua" se autosuspende
                printf("ESTAMOS AUTOSUSPENDIENDO LA TAREA\n");
                vTaskSuspend(NULL); 
                ESTADO= INIT;

            break;
        }
    }
}

//Tarea para leer la temperatura y la humedad, imprimir el valor y guardarlo
void measure_habitat(void *pvParameter)
{
    uint8_t aux[5], i;
    while (1) 
    {
        if(dht11_start())
        {
            for(i=0;i<5;i++)
                aux[i]=dht11_read();

            if((aux[0]+aux[1]+aux[2]+aux[3]) == aux[4])
            {
                temperature = aux[2];
                humidity = aux[0];
                printf("TEMP.: %d°, HUMID.: %d \n", temperature, humidity);
                vTaskSuspend(NULL);
            }
            else
                printf(" --> CHECKSUM FAILED\n");
        }
        usleep(5000);
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
            printf("### Presionó: DERECHA\n");
            //Si se encuentra en la pantalla principal, accede al primer menú           
            if(pagina_menu==0)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
            //Si el programa está posicionado en el primer menú de opciones
            //-->Si está seleccionada la opción "Configuración WiFi"
            else if(pagina_menu==1 && opt_menu==0)
            {  
                //-->Ingresa a la Configuración de WiFi
                pagina_menu=2;
                
                memset(WIFI_SSIDS, 0, sizeof(WIFI_SSIDS));
                // escanea las redes que hay disponibles
                wifi_scan();

                //Posiciona el cursor en el primer elemento del vector de seteo e ingresa a la configuración de SSID
                cursor=0;
                
                vTaskResume(task_handler_lcd);
            }
            //-->Si está seleccionada la opción "Ver Status"
            else if(pagina_menu==1 && opt_menu==1)
            {
                //-->Ingresa a ver la cantidad de días de cosecha y la cantidad restante estimada
                opt_menu=0;
                pagina_menu=4;
                vTaskResume(task_handler_lcd);
            }
            //-->Si está seleccionada la opción "Smoke Test"
            else if(pagina_menu==1 && opt_menu==2)
            {
                //-->Comienza el testing
                opt_menu=2;
                pagina_menu=1;
                SMOKE_TEST= true;
                TEST_STATE = LIGHTS;
                vTaskResume(task_handler_lcd);
            }
            //-->Navega de "Configuración WiFi" -> "Configuración de PWD"
            else if(pagina_menu==2)
            {                
                // Se seteo la red WiFi
                sprintf(WIFI_SSID, "%s", WIFI_SSIDS[cursor]);

                //Posiciona el cursor en el primer elemento del vector
                cursor=0;
                //Ingresa a la configuración de PSWD
                pagina_menu=3;

                vTaskResume(task_handler_lcd);
            }               
            //Si se encuentra en la configuración de PWD mueve el cursor
            else if(pagina_menu==3)
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
        else if(!entradas_antirrebote[2].level)
        {
            printf("### Presionó: IZQUIERDA\n");
            //Navega desde el primer menú hacia la pantalla principal
            if(pagina_menu==1)
            {
                opt_menu=0;
                pagina_menu=0;
                SMOKE_TEST= false;
                vTaskResume(task_handler_lcd);
            }
            //Navega desde la configuración WiFi hacia el primer menú
            if(pagina_menu==2)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
            //Navega desde la configuración de PWD hacia la configuración WiFi
            if(pagina_menu==3)
            {
                opt_menu=0;
                pagina_menu=1;
                
                //Busca el final del PWD seteado y almacena el valor en char * que se usará para la conexión
                for(i=0;i<16;i++)
                {
                    if(PWD[i]==' ')
                    {
                        PWD[i]='\0';
                    }
                }

                sprintf(WIFI_PSWD, "%s", PASSWORD_HARDCODEADO);//PWD);
                wifi_connect();

                if(WIFI_IS_CONNECTED)
                {
                    save_wifi_config();
                    pagina_menu= 5;
                }
                else
                {
                    pagina_menu= 6;
                }
                
                vTaskResume(task_handler_lcd);
            }
            //Navega desde "Status" hacia el primer menú
            if(pagina_menu==4)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
             //Navega desde "Conexión exitosa!" hacia el primer menú
            if(pagina_menu==5)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
             //Navega desde "Conexión fallida!" hacia el primer menú
            if(pagina_menu==6)
            {
                opt_menu=0;
                pagina_menu=1;
                vTaskResume(task_handler_lcd);
            }
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ARRIBA"
        else if(!entradas_antirrebote[3].level)
        {
            printf("### Presionó: ARRIBA\n");
            if(pagina_menu==1)
            {
                //En menúes de dos opciones selecciona la opción superior
                if(opt_menu==1)
                {
                    opt_menu=0; 
                    vTaskResume(task_handler_lcd); 
                }
                if(opt_menu==2)
                {
                    opt_menu=1; 
                    vTaskResume(task_handler_lcd); 
                }
            }
            //Si se encuentra en la configuración de SSID mueve el cursor
            else if(pagina_menu==2)
            {
                cursor++;
                if(cursor>=WIFI_SSIDS_SCANNED)
                {
                    cursor=0;
                }                
                vTaskResume(task_handler_lcd);
            }
            else if(pagina_menu==3)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter--;
                if(caracter=='0')
                    caracter='y';
                vTaskResume(task_handler_lcd);
            }            
        }
        //Se detecta el nivel alto de la entrada antirrebote asociada al pulsador "ABAJO"
        else if(!entradas_antirrebote[4].level)
        {
            printf("### Presionó: ABAJO\n");
            if(pagina_menu==1)
            {
                //En menúes de dos opciones selecciona la opción inferior
                if(pagina_menu== 1 && opt_menu==1 && !CROP_RUNNING) // HIDDEN Menu for TESTING MODE (only in IDLE mode)
                {
                    opt_menu=2;
                    vTaskResume(task_handler_lcd);
                }
                else if(opt_menu == 0)
                {
                    opt_menu=1;
                    vTaskResume(task_handler_lcd);
                }
            }
            //Si se encuentra en la configuración de SSID mueve el cursor
            else if(pagina_menu==2)
            {
                cursor++;
                if(cursor>=WIFI_SSIDS_SCANNED)
                {
                    cursor=0;
                }                
                vTaskResume(task_handler_lcd);
            }
            if(pagina_menu==3)
            {
                //En los menúes de seteo scrollea los caracteres
                caracter++;
                if(caracter=='y')
                    caracter='0';
                vTaskResume(task_handler_lcd);
            }
        }    

        vTaskDelay(350 / portTICK_PERIOD_MS);
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
        else if(pagina_menu==1)
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
            else if(opt_menu==1)
            {
                printf(" Configurar WiFi\n");
                printf(">Ver Status\n");
                lcd_send_string(" Configurar WiFi", LCD_ROW_1);
	            lcd_send_string(">Ver Status", LCD_ROW_2);
                vTaskSuspend(task_handler_lcd);
            }
            else if(opt_menu==2) // This MENU is hidden, will be only available if state is IDLE and it is for activating the TEST MODE
            {
                sprintf(test_msg, "Smoke Test [%d/%d]", TEST_STATE, COOLERS);                
                printf("%s\n", test_msg);
                lcd_send_string(test_msg, LCD_ROW_1);
	            
                switch (TEST_STATE)
                {
                    case LIGHTS:
                        printf("Prueba luces\n");
                        lcd_send_string("Prueba luces", LCD_ROW_2);
                        break;
                    
                    // case PUMP:
                    //     printf("Prueba riego\n");
                    //     lcd_send_string("Prueba riego", LCD_ROW_2);
                    //     break;

                    // case SONDA_EC:
                    //     printf("Prueba sonda EC\n");
                    //     lcd_send_string("Prueba sonda EC", LCD_ROW_2);
                    //     break;

                    // case DOSIF_EC:
                    //     printf("Prueba dosif A\n");
                    //     lcd_send_string("Prueba dosif EC", LCD_ROW_2);
                    //     break;

                    // case SONDA_PH:
                    //     printf("Prueba sonda PH\n");
                    //     lcd_send_string("Prueba sonda PH", LCD_ROW_2);
                    //     break;

                    // case DOSIF_PH:
                    //     printf("Prueba dosif PH\n");
                    //     lcd_send_string("Prueba dosif PH", LCD_ROW_2);
                    //     break;

                    case REGULATE_WATER:
                        printf("Prueba Regular\n");
                        lcd_send_string("Prueba Regular", LCD_ROW_2);
                        break;

                    case DHT11:
                        printf("Prueba Temp-Hum\n");
                        lcd_send_string("Prueba Temp-Hum", LCD_ROW_2);
                        break;
                    
                    case COOLERS:
                        printf("Prueba Coolers\n");
                        lcd_send_string("Prueba Coolers", LCD_ROW_2);
                        break;

                    default:
                        printf(">Activate\n");
                        lcd_send_string(">Activate", LCD_ROW_2);
                        break;
                }

                vTaskSuspend(task_handler_lcd);
            }  
        }
        else if(pagina_menu==2) // Configurar wifi
        {
            lcd_send_command(0x01);
            if(opt_menu==0)
            {
                printf("Seleccione WiFi:\n");
                printf("%s\n", WIFI_SSIDS[cursor]);
                lcd_send_string("Seleccione WiFi:", LCD_ROW_1);
                lcd_send_string(WIFI_SSIDS[cursor], LCD_ROW_2);
            
                vTaskSuspend(task_handler_lcd);
            }
        }
        else if(pagina_menu==3) // Configurar password
        {
            lcd_send_command(0x01);

            PWD[cursor]= caracter;
            printf("Introduzca PSWD\n");
            printf("%s\n", PWD);

            lcd_send_string("Introduzca PSWD", LCD_ROW_1);
	        lcd_send_string(PWD, LCD_ROW_2);
            vTaskSuspend(task_handler_lcd);            
        }
        else if(pagina_menu==4)  // Ver Status
        {
            lcd_send_command(0x01);    

            printf("Dias de cosecha: \n");
            printf("Dias restantes: \n");

            lcd_send_string("Dias de cosecha: ", LCD_ROW_1);
	        lcd_send_string("Dias restantes: ", LCD_ROW_2);     
            vTaskSuspend(task_handler_lcd);       
        }
        else if(pagina_menu==5) // WiFi Conectado
        {
            lcd_send_command(0x01);
            printf("Configurando WiFi\n");
            printf("Conexion exitosa!\n");

            lcd_send_string("Configurando WiFi", LCD_ROW_1);
            lcd_send_string("Conexion exitosa!", LCD_ROW_2);
            vTaskSuspend(task_handler_lcd); 
        }
        else if(pagina_menu==6) // WiFi No Conectado
        {
            lcd_send_command(0x01);
            printf("Configurando WiFi\n");
            printf("Conexion fallida!\n");
            
            lcd_send_string("Configurando WiFi", LCD_ROW_1);
            lcd_send_string("Conexion fallida!", LCD_ROW_2);
            vTaskSuspend(task_handler_lcd);   
        }
    }
}