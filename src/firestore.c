/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //                          
//                                                                                     // 
//  Archivo: firestore.c                                                               // 
//  Descripción: Contiene todas las funciones asociadas al manejo de la base de datos  //
//               de firestore                                                          //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "firestore.h"

static firestore_ctx_t stCtx;

extern uint32_t ec;
extern uint32_t ph;
extern uint32_t temperature;
extern uint32_t humidity;
extern uint32_t PH_MAX, PH_MIN, EC_MAX, EC_MIN;

firestore_err_t init_firestore(void)
{
    /* Initialize the firestore_ctx_t struct*/
    memset(&stCtx, 0x00, sizeof(stCtx));
    stCtx.stHttpconfig.host = FIRESTORE_HOSTNAME;
    stCtx.stHttpconfig.buffer_size = FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE;
    stCtx.stHttpconfig.buffer_size_tx = FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE;
    stCtx.stHttpconfig.cert_pem = FIRESTORE_FIREBASE_CA_CERT_PEM;
    stCtx.stHttpconfig.event_handler = firestore_http_event_handler;
    stCtx.stHttpconfig.transport_type = HTTP_TRANSPORT_OVER_SSL;
    stCtx.stHttpconfig.user_data = stCtx.tcHttpBody;
    
    return FIRESTORE_OK;
}

firestore_err_t firestore_update_document(char *pcCollectionId, char *pcDocumentId, char *pcDocument, uint32_t *pu32DocumentLen)
{
    int16_t s16HttpCode;
    int32_t s32Length;

    firestore_err_t eRetVal;

    eRetVal = FIRESTORE_OK;
    if(pcCollectionId && pcDocumentId)
    {
        s32Length = snprintf(stCtx.tcPath, FIRESTORE_HTTP_PATH_SIZE, "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s", FIRESTORE_FIREBASE_PROJECT_ID, pcCollectionId, pcDocumentId, FIRESTORE_FIREBASE_API_KEY);
        
        if(s32Length > 0)
        {
            stCtx.stHttpconfig.path = stCtx.tcPath;
            //printf("HTTP path: %s", stCtx.stHttpconfig.path);
            stCtx.pstHttpClient = esp_http_client_init(&stCtx.stHttpconfig);
            esp_http_client_set_method(stCtx.pstHttpClient, HTTP_METHOD_PATCH);
            esp_http_client_set_post_field(stCtx.pstHttpClient, pcDocument, strlen(pcDocument));
            esp_http_client_set_header(stCtx.pstHttpClient, "Content-Type", "application/json");
            

            if(ESP_OK == esp_http_client_perform(stCtx.pstHttpClient))
            {
                s16HttpCode = esp_http_client_get_status_code(stCtx.pstHttpClient);
                //printf("HTTP PATCH Status = %d, content_length = %d\n", esp_http_client_get_status_code(stCtx.pstHttpClient), esp_http_client_get_content_length(stCtx.pstHttpClient));
                
                if(200 != s16HttpCode)  // 200: OK
                {
                    printf("Firestore REST API call failed with HTTP code: %d\n", s16HttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
                else
                {
                    pcDocument = stCtx.tcHttpBody;
                    *pu32DocumentLen = stCtx.u32HttpBodyLen;
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }

    stCtx.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stCtx.pstHttpClient);
    
    return eRetVal;
}

firestore_err_t firestore_get_document(char *pcCollectionId, char *pcDocumentId, char *ppcDocument, uint32_t *pu32DocumentLen)
{
    int16_t s16HttpCode;
    int32_t s32Length;
    firestore_err_t eRetVal;

    eRetVal = FIRESTORE_OK;
    
    if(pcCollectionId && pcDocumentId)
    {
        s32Length = snprintf(stCtx.tcPath, FIRESTORE_HTTP_PATH_SIZE, "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s", FIRESTORE_FIREBASE_PROJECT_ID, pcCollectionId, pcDocumentId, FIRESTORE_FIREBASE_API_KEY);
        
        if(s32Length > 0)
        {
            stCtx.stHttpconfig.path = stCtx.tcPath;
            printf("HTTP path: %s", stCtx.stHttpconfig.path);
            stCtx.pstHttpClient = esp_http_client_init(&stCtx.stHttpconfig);
            
            if(ESP_OK == esp_http_client_perform(stCtx.pstHttpClient))
            {
                s16HttpCode = esp_http_client_get_status_code(stCtx.pstHttpClient);
                printf("HTTP code: %d\n content_length: %d\n", s16HttpCode, esp_http_client_get_content_length(stCtx.pstHttpClient));

                if(200 != s16HttpCode)
                {
                    printf("Firestore REST API call failed with HTTP code: %d", s16HttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
                else
                {
                    //*ppcDocument = stCtx.tcHttpBody;
                    strcpy(ppcDocument,stCtx.tcHttpBody);
                    *pu32DocumentLen = stCtx.u32HttpBodyLen;
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }

    stCtx.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stCtx.pstHttpClient);

    return eRetVal;
}

firestore_err_t firestore_get_collection(char *pcCollectionId, char *ppcDocs, uint32_t *pu32DocsLen)
{
    int16_t s16HttpCode;
    int32_t s32Length;
    firestore_err_t eRetVal;

    eRetVal = FIRESTORE_OK;
    if(pcCollectionId)
    {
        s32Length = snprintf(stCtx.tcPath, FIRESTORE_HTTP_PATH_SIZE, "/v1/projects/%s/databases/(default)/documents/%s?key=%s", FIRESTORE_FIREBASE_PROJECT_ID, pcCollectionId, FIRESTORE_FIREBASE_API_KEY);
        
        if(s32Length > 0)
        {
            stCtx.stHttpconfig.path = stCtx.tcPath;
            printf("HTTP path: %s", stCtx.stHttpconfig.path);
            stCtx.pstHttpClient = esp_http_client_init(&stCtx.stHttpconfig);

            if(ESP_OK == esp_http_client_perform(stCtx.pstHttpClient))
            {
                s16HttpCode = esp_http_client_get_status_code(stCtx.pstHttpClient);
                printf("HTTP code: %d, content_length: %d", s16HttpCode, esp_http_client_get_content_length(stCtx.pstHttpClient));

                if(200 != s16HttpCode)
                {
                    printf("Firestore REST API call failed with HTTP code: %d", s16HttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
                else
                {
                    //*ppcDocs = stCtx.tcHttpBody;
                    strcpy(ppcDocs,stCtx.tcHttpBody);
                    *pu32DocsLen = stCtx.u32HttpBodyLen;
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }
    
    stCtx.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stCtx.pstHttpClient);
    
    return eRetVal;
}

firestore_err_t firestore_add_document(char *pcCollectionId, char *pcDocumentId, char *pcDocument, uint32_t *pu32DocumentLen)
{
    int16_t s16HttpCode;
    int32_t s32Length;
    firestore_err_t eRetVal;

    eRetVal = FIRESTORE_OK;
    if(pcCollectionId && pcDocumentId)
    {
        s32Length = snprintf(stCtx.tcPath, FIRESTORE_HTTP_PATH_SIZE, "/v1/projects/%s/databases/(default)/documents/%s?documentId=%s&key=%s", FIRESTORE_FIREBASE_PROJECT_ID, pcCollectionId, pcDocumentId, FIRESTORE_FIREBASE_API_KEY);
        if(s32Length > 0)
        {
            stCtx.stHttpconfig.path = stCtx.tcPath;
            printf("HTTP path: %s", stCtx.stHttpconfig.path);
            stCtx.pstHttpClient = esp_http_client_init(&stCtx.stHttpconfig);
            esp_http_client_set_method(stCtx.pstHttpClient, HTTP_METHOD_POST);
            esp_http_client_set_header(stCtx.pstHttpClient, "Content-Type", "application/json");
            esp_http_client_set_post_field(stCtx.pstHttpClient, pcDocument, strlen(pcDocument));
        
            if(ESP_OK == esp_http_client_perform(stCtx.pstHttpClient))
            {
                s16HttpCode = esp_http_client_get_status_code(stCtx.pstHttpClient);
                printf("HTTP PATCH Status = %d, content_length = %d", esp_http_client_get_status_code(stCtx.pstHttpClient), esp_http_client_get_content_length(stCtx.pstHttpClient));
                
                if(200 != s16HttpCode)
                {
                    printf("Firestore REST API call failed with HTTP code: %d", s16HttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
                else
                {
                    pcDocument = stCtx.tcHttpBody;
                    *pu32DocumentLen = stCtx.u32HttpBodyLen;
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }   
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }
    
    stCtx.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stCtx.pstHttpClient);

    return eRetVal;
}

firestore_err_t firestore_delete_document(char *pcCollectionId, char *pcDocumentId)
{
    int16_t s16HttpCode;
    int32_t s32Length;
    firestore_err_t eRetVal;

    eRetVal = FIRESTORE_OK;
    if(pcCollectionId && pcDocumentId)
    {
        s32Length = snprintf(stCtx.tcPath, FIRESTORE_HTTP_PATH_SIZE, "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s", FIRESTORE_FIREBASE_PROJECT_ID, pcCollectionId, pcDocumentId, FIRESTORE_FIREBASE_API_KEY);

        if(s32Length > 0)
        {
            stCtx.stHttpconfig.path = stCtx.tcPath;
            printf("HTTP path: %s", stCtx.stHttpconfig.path);
            stCtx.pstHttpClient = esp_http_client_init(&stCtx.stHttpconfig);
            esp_http_client_set_method(stCtx.pstHttpClient, HTTP_METHOD_DELETE);
            
            if(ESP_OK == esp_http_client_perform(stCtx.pstHttpClient))
            {
                s16HttpCode = esp_http_client_get_status_code(stCtx.pstHttpClient);
                printf("HTTP code: %d, content_length: %d", s16HttpCode, esp_http_client_get_content_length(stCtx.pstHttpClient));

                if(200 != s16HttpCode)
                {
                    printf("Firestore REST API call failed with HTTP code: %d", s16HttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
                else
                {
                    printf("Document deleted successfully");
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }
    
    stCtx.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stCtx.pstHttpClient);
    
    return eRetVal;
}

/**
 * @brief 
 * 
 * @param pstEvent 
 * @return esp_err_t 
 */
esp_err_t firestore_http_event_handler(esp_http_client_event_t *pstEvent)
{
  esp_err_t s32RetVal;

  s32RetVal = ESP_OK;
  switch(pstEvent->event_id)
  {
    case HTTP_EVENT_ERROR:
        printf("HTTP error\n");
        break;

    case HTTP_EVENT_ON_CONNECTED:
        printf("HTTP connected to server\n");
        break;

    case HTTP_EVENT_HEADERS_SENT:
        printf("All HTTP headers are sent to server\n");
        break;

    case HTTP_EVENT_ON_HEADER:
        // printf("HTTP header received\n");
        break;

    case HTTP_EVENT_ON_DATA:
        /* If user_data buffer is configured, copy the response into it */
        if(pstEvent->user_data)
        {
            strncpy(pstEvent->user_data+stCtx.u32HttpBodyLen, (char*)pstEvent->data, pstEvent->data_len);
            stCtx.u32HttpBodyLen += pstEvent->data_len;
        }
        /* Else you can copy the response into a global HTTP buffer */
        break;

    case HTTP_EVENT_ON_FINISH:
        printf("HTTP session is finished\n");
        break;

    case HTTP_EVENT_DISCONNECTED:
        printf("HTTP connection is closed\n");
        break;
  }

  return s32RetVal;
}

int8_t fs_check_state(char *id, char *planta)
{
    static uint32_t u32DocLength;
    static char tcDoc[FIRESTORE_DOC_MAX_SIZE];

    if(init_firestore() != FIRESTORE_OK)
    {
        return -1;
    }
    
    u32DocLength = snprintf(tcDoc, sizeof(tcDoc), " ");

    // Leemos de firestore un documento
    firestore_err_t FIRESTORE_STATUS = firestore_get_document(SYSTEM_COLLECTION, id, tcDoc, &u32DocLength);

    if(FIRESTORE_STATUS != FIRESTORE_OK)
    {
        printf("ERROR: Couldn't get document\n");
        return -1;
    }

    // Creamos un archivo JSON para respaldar el documento leido de firestore
    FILE* f = fopen("/spiffs/system.json", "w");    

    // Chequeamos si hay error
    if (f != NULL)                                   
    {
        // Guardamos el JSON en un archivo
        fprintf(f, "%s", tcDoc);
        fputc('\0', f);
        fclose(f); 
    }

    char valor[100];
    int value = search_value("/spiffs/system.json", "plant", valor);

    if(value == -1)
    {
        printf("ERROR: Couldn´t search value in JSON file\n");
        return -2;
    }

    if(!strcmp("",valor))
    {
        return 0;
    }
    else
    {
        strcpy(planta, valor);
        return 1;
    }
    
}

int8_t fs_stats_actualization(char *id)
{
    static uint32_t u32DocLength;
    static char tcDoc[FIRESTORE_DOC_MAX_SIZE]; 
    char temp_fs[20], hum_fs[20], ec_fs[20], ph_fs[20];

    FILE* f = fopen("/spiffs/system.json", "w"); 

    if(init_firestore() != FIRESTORE_OK)
    {
        return -1;
    }

    itoa(temperature, temp_fs, 10);
    itoa(ph, ph_fs, 10);
    itoa(ec, ec_fs, 10);
    itoa(humidity, hum_fs, 10);

    firestore_err_t FIRESTORE_STATUS = firestore_get_document(SYSTEM_COLLECTION, id, tcDoc, &u32DocLength);

    // Chequeamos si hay error
    if (f != NULL)                                   
    {
        // Guardamos el JSON en un archivo
        fprintf(f, "%s", tcDoc);
        printf("Doc leído de la base: %s",tcDoc);
        fputc('\0', f);
        fclose(f);
    }

    int value = replace_value("/spiffs/system.json", "temperature", temp_fs);
    if(value == -1)
    {
        printf("ERROR: Couldn´t replace value in JSON file\n");
    }

    value = replace_value("/spiffs/system.json", "acidity", ph_fs);
    if(value == -1)
    {
        printf("ERROR: Couldn´t replace value in JSON file\n");
    }

    value = replace_value("/spiffs/system.json", "humidity", hum_fs);
    if(value == -1)
    {
        printf("ERROR: Couldn´t replace value in JSON file\n");
    }

    value = replace_value("/spiffs/system.json", "conductivity", ec_fs);
    if(value == -1)
    {
        printf("ERROR: Couldn´t replace value in JSON file\n");
    }

    // Abrimos el JSON con el campo actualizado
    f = fopen("/spiffs/system.json", "r");

    // Pasamos el contenido del JSON a una variable
    fread(tcDoc, FIRESTORE_DOC_MAX_SIZE, 1, f);
    u32DocLength = sizeof(tcDoc);
    printf("Doc a escribir de la base: %s\n\n",tcDoc);

    // Cerramos archivo JSON de respaldo
    fclose(f);

    // Actualizamos el documento en firestore
    FIRESTORE_STATUS= firestore_update_document(SYSTEM_COLLECTION, id, tcDoc, &u32DocLength);

    if(FIRESTORE_STATUS != FIRESTORE_OK)
    {
        printf("ERROR: Couldn't update document\n");
        return -1;
    }
    else
    {
        return 1;
    }
    
}

int8_t fs_check_limits(char *planta)
{
    static uint32_t u32DocLength;
    static char tcDoc[FIRESTORE_DOC_MAX_SIZE];

    if(init_firestore() != FIRESTORE_OK)
    {
        return -1;
    }
    
    u32DocLength = snprintf(tcDoc, sizeof(tcDoc), " ");

    // Leemos de firestore un documento
    firestore_err_t FIRESTORE_STATUS = firestore_get_document(PLANTS_COLLECTION_ID, planta, tcDoc, &u32DocLength);

    if(FIRESTORE_STATUS != FIRESTORE_OK)
    {
        printf("ERROR: Couldn't get document\n");
        return -1;
    }

    // Creamos un archivo JSON para respaldar el documento leido de firestore
    FILE* f = fopen("/spiffs/plant.json", "w");    

    // Chequeamos si hay error
    if (f != NULL)                                   
    {
        // Guardamos el JSON en un archivo
        fprintf(f, "%s", tcDoc);
        fputc('\0', f);
        fclose(f); 
    }

    char valor[100];
    int value = search_value("/spiffs/plant.json", "ec_max", valor);

    if(value == -1)
    {
        printf("ERROR: Couldn´t search value in JSON file\n");
        return -2;
    }

    EC_MAX = atoi(valor);

    value = search_value("/spiffs/plant.json", "ec_min", valor);

    if(value == -1)
    {
        printf("ERROR: Couldn´t search value in JSON file\n");
        return -2;
    }

    EC_MIN = atoi(valor);

    value = search_value("/spiffs/plant.json", "ph_min", valor);

    if(value == -1)
    {
        printf("ERROR: Couldn´t search value in JSON file\n");
        return -2;
    }

    PH_MIN = atoi(valor);

    value = search_value("/spiffs/plant.json", "ph_max", valor);

    if(value == -1)
    {
        printf("ERROR: Couldn´t search value in JSON file\n");
        return -2;
    }

    PH_MAX = atoi(valor);

    printf("Valores límite actualizados:\n *PH_MIN:%d\n *PH_MAX:%d\n *EC_MIN:%d\n *EC_MAX:%d\n",PH_MIN,PH_MAX,EC_MIN,EC_MAX);

    return 1;
}

//***************************************** EJEMPLOS DE LLAMADO A FUNCIONES *****************************************
/**
 * @brief VARIABLES COMUNES A TODOS LOS EJEMPLOS
 * 
 */
// static uint32_t u32DocLength;
// static char tcDoc[FIRESTORE_DOC_MAX_SIZE];
// firestore_err_t FIRESTORE_STATUS
// u32DocLength = snprintf(tcDoc, sizeof(tcDoc), " ");

/**
 * @brief GET DOCUMENT FROM FIRESTORE
 * 
 */
// FIRESTORE_STATUS= firestore_get_document(PLANTS_COLLECTION_ID, PLANT_DOCUMENT_ID, tcDoc, &u32DocLength);

// if(FIRESTORE_STATUS == FIRESTORE_OK)
// {
//     printf("Document got successfully\n");
//     // printf("\n %s \n", tcDoc);
// }
// else
// {
//     printf("ERROR: Couldn't get document\n");
// }

/**
 * @brief UPDATE DOCUMENT FROM FIRESTORE
 * 
 */
// FIRESTORE_STATUS= firestore_update_document(ESP_COLLECTION_ID, ESP_DOCUMENT_ID, tcDoc, &u32DocLength);

// if(FIRESTORE_STATUS == FIRESTORE_OK)
// {
//     printf("Document updated successfully\n");
// }
// else
// {
//     printf("ERROR: Couldn't update document\n");
// }

/**
 * @brief GET COLLECTION FROM FIRESTORE
 * 
 */
// FIRESTORE_STATUS= firestore_get_collection(FIRESTORE_COL_ID, tcDoc, &u32DocLength);

// if(FIRESTORE_STATUS == FIRESTORE_OK)
// {
//     printf("Collection got successfully\n");
//     printf("\n %s \n", tcDoc);
// }
// else
// {
//     printf("ERROR: Couldn't get collection\n");
// }

/**
 * @brief DELETE DOCUMENT FROM FIRESTORE
 * 
 */
// firestore_err_t FIRESTORE_STATUS= firestore_delete_document(FIRESTORE_COL_ID, FIRESTORE_DOC_ID);

// if(FIRESTORE_STATUS == FIRESTORE_OK)
// {
//     printf("Document deleted successfully\n");
// }
// else
// {
//     printf("ERROR: Couldn't delet document\n");
// }

/**
 * @brief ADD DOCUMENT FROM FIRESTORE (MUST EXIST THE COLLECTION)
 * 
 */
// firestore_err_t FIRESTORE_STATUS= firestore_add_document(FIRESTORE_COL_ID, FIRESTORE_DOC_ID, tcDoc, &u32DocLength);

// if(FIRESTORE_STATUS == FIRESTORE_OK)
// {
//     printf("Document added successfully\n");
// }
// else
// {
//     printf("ERROR: Couldn't add document\n");
// }

//*******************************************************************************************************************