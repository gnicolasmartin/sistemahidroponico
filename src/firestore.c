// /////////////////////////////////////////////////////////////////////////////////////////
// //  Proyecto: Sistema Hidropónico Para Hogares                                         // 
// //  Alumnos: *Lucia Sucunza                                                            //
// //           *Nicolas Gomez                                                            //
// //           *Santiago Laborde                                                         //                          
// //                                                                                     // 
// //  Archivo: firestore.c                                                               // 
// //  Descripción: Contiene todas las funciones asociadas al manejo de la base de datos  //
// //               de firestore                                                          //
// //                                                                                     // 
// /////////////////////////////////////////////////////////////////////////////////////////

#include "firestore.h"

static firestore_ctx_t stCtx;

/**
 * @brief 
 * 
 * @return firestore_err_t 
 */
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

/**
 * @brief 
 * 
 * @param pcCollectionId 
 * @param pcDocumentId 
 * @param pcDocument 
 * @param pu32DocumentLen 
 * @return firestore_err_t 
 */
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

/**
 * @brief 
 * 
 * @param pcCollectionId 
 * @param pcDocumentId 
 * @param ppcDocument 
 * @param pu32DocumentLen 
 * @return firestore_err_t 
 */
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

/**
 * @brief 
 * 
 * @param pcCollectionId 
 * @param ppcDocs 
 * @param pu32DocsLen 
 * @return firestore_err_t 
 */
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

/**
 * @brief 
 * 
 * @param pcCollectionId 
 * @param pcDocumentId 
 * @param pcDocument 
 * @param pu32DocumentLen 
 * @return firestore_err_t 
 */
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

/**
 * @brief 
 * 
 * @param pcCollectionId 
 * @param pcDocumentId 
 * @return firestore_err_t 
 */
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