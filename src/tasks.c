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
#include "firestore.h"
#include "filesystem.h"

//Estructura de entradas antirrebote
extern antirrebote_t entradas_antirrebote[CANTIDAD_ANTIRREBOTE];

// Just a simple idle task
void idle(void *pvParameter)
{
    while(1)
    {

    }
}

//Tarea que lee las entradas implementando antirrebote por Software
void leer_entradas(void *pvParameter)
{
    printf("\n**************************************************************"  );
    printf("\n                    READING PORTS:                            "  ); 
    printf("\n--------------------------------------------------------------\n");

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

        vTaskDelay(READPORTS_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

//Dummy para probar GPIO
void toggle_led(void *pvParameter)
{
    printf("\n**************************************************************"  );
    printf("\n                    TOGGLE LED                                "  ); 
    printf("\n--------------------------------------------------------------\n");
    
    while(1)
    {
        if(entradas_antirrebote[0].level)   
        {
            gpio_set_level(GPIO_TEST_LED,0);
        }
        else
        {
            gpio_set_level(GPIO_TEST_LED,1);
        }

        vTaskDelay(TOGGLED_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

#define PLANTS_COLLECTION_ID    "Plants"
#define PLANT_DOCUMENT_ID       "4SMhKaRUwBpVHXWFLNkN"
#define PLANT_DOCUMENT_FIELD    "Temperatura"

#define ESP_COLLECTION_ID       "Esp"
#define ESP_DOCUMENT_ID         "Lechuga"
#define ESP_DOCUMENT_FIELD      "Temperatura"

// Update values in firestore database 
void firestore_task(void *pvParameter)
{
    printf("\n**************************************************************"  );
    printf("\n                    FIRESTORE:                                "  ); // 5 TABS OF SEPARATION
    printf("\n--------------------------------------------------------------\n");

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