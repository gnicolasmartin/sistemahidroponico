/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: firestore.h                                                               // 
//  Descripción: Archivo de cabecera para el manejo de firestore                       //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES
#include <stdint.h>
#include "json.h"
#include <string.h>
#include "esp_http_client.h"

// CONFIG DEFINES
#ifndef TAG
#define TAG                                      "FIRESTORE"
#endif /* TAG */

#ifndef FIRESTORE_HOSTNAME
#define FIRESTORE_HOSTNAME                       "firestore.googleapis.com"
#endif /* FIRESTORE_HOSTNAME */

#ifndef FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE
#define FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE      1024
#endif /* FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE
#define FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE      1024
#endif /* FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_RX_BUF_SIZE
#define FIRESTORE_HTTP_RX_BUF_SIZE               1024*2
#endif /* FIRESTORE_HTTP_RX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_PATH_SIZE
#define FIRESTORE_HTTP_PATH_SIZE                 256
#endif /* FIRESTORE_HTTP_PATH_SIZE */

#ifndef FIRESTORE_FIREBASE_PROJECT_ID
#define FIRESTORE_FIREBASE_PROJECT_ID            "lively-oxide-241518"
#endif /* FIRESTORE_FIREBASE_PROJECT_ID */

#ifndef FIRESTORE_FIREBASE_API_KEY
#define FIRESTORE_FIREBASE_API_KEY               "AIzaSyBH07ASH1TkgL3UPV584YO5vPapnzPGBPE"
#endif /* FIRESTORE_FIREBASE_API_KEY */

#ifndef FIRESTORE_FIREBASE_CA_CERT_PEM
/* 
  This SSL certificate you should create in your computer for enabling the HTTP conection 
  Run the following command in a bash shell:
  openssl s_client -showcerts -verify 5 -connect edgecert.googleapis.com:443 < /dev/null  
  Paste one of the 5 SSL certificates and you are done!                   
*/
#define FIRESTORE_FIREBASE_CA_CERT_PEM                                                             \
                                          "-----BEGIN CERTIFICATE-----\r\n"                        \
                            "MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX\r\n" \
                            "MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\r\n" \
                            "CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx\r\n" \
                            "OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\r\n" \
                            "GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx\r\n" \
                            "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63\r\n" \
                            "ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS\r\n" \
                            "iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k\r\n" \
                            "KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ\r\n" \
                            "DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk\r\n" \
                            "j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5\r\n" \
                            "cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW\r\n" \
                            "CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499\r\n" \
                            "iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei\r\n" \
                            "Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap\r\n" \
                            "sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b\r\n" \
                            "9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP\r\n" \
                            "BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf\r\n" \
                            "BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw\r\n" \
                            "JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH\r\n" \
                            "MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al\r\n" \
                            "oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy\r\n" \
                            "MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF\r\n" \
                            "AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9\r\n" \
                            "NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9\r\n" \
                            "WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw\r\n" \
                            "9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy\r\n" \
                            "+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi\r\n" \
                            "d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=\r\n" \
                                            "-----END CERTIFICATE-----\r\n"


#endif /* FIRESTORE_FIREBASE_CA_CERT_PEM */


// TYPEDEF DEFINES
typedef enum
{
  FIRESTORE_OK                = 0,               /**< No error, everything OK  */
  FIRESTORE_ERR               = -1,              /**< Generic error            */
  FIRESTORE_ERR_HTTP          = -2,              /**< Timeout                  */
  FIRESTORE_ERR_ARG           = -4,              /**< Illegal argument         */
  FIRESTORE_ERR_UNHANDLED     = -5,              /**< Unhandled feature/option */
  FIRESTORE_ERR_MCU           = -6,              /**< MCU error                */
  FIRESTORE_ERR_OS            = -7,              /**< Operating system error   */
}firestore_err_t;

// TYPEDEF STRUCT
typedef struct
{
  char tcPath[FIRESTORE_HTTP_PATH_SIZE];
  uint32_t u32PathLen;
  char tcHttpBody[FIRESTORE_HTTP_RX_BUF_SIZE];
  uint32_t u32HttpBodyLen;
  esp_http_client_handle_t pstHttpClient;
  esp_http_client_config_t stHttpconfig;
}firestore_ctx_t;

// DEFINES
#define FIRESTORE_DOC_MAX_SIZE 2048
#define PLANTS_COLLECTION_ID    "Plants"
#define PLANT_DOCUMENT_ID       "4SMhKaRUwBpVHXWFLNkN"
#define PLANT_DOCUMENT_FIELD    "Temperatura"
#define ESP_COLLECTION_ID       "Esp"
#define ESP_DOCUMENT_ID         "Lechuga"
#define ESP_DOCUMENT_FIELD      "Temperatura"

#define SYSTEM_ID               "57832"
#define SYSTEM_COLLECTION       "Systems"


// PROTOTYPES
firestore_err_t init_firestore(void);
esp_err_t firestore_http_event_handler(esp_http_client_event_t *pstEvent);
firestore_err_t firestore_get_collection(char *pcCollectionId, char *pcDocs, uint32_t *pu32DocsLen);
firestore_err_t firestore_get_document(char *pcCollectionId, char *pcDocumentId, char *ppcDocument, uint32_t *pu32DocumentLen);
firestore_err_t firestore_add_document(char *pcCollectionId, char *pcDocumentId, char *pcDocument, uint32_t *pu32DocumentLen);
firestore_err_t firestore_update_document(char *pcCollectionId, char *pcDocumentId, char *pcDocument, uint32_t *pu32DocumentLen);
firestore_err_t firestore_delete_document(char *pcCollectionId, char *pcDocumentId);

int8_t fs_check_state(char*, char*);
int8_t fs_stats_actualization(char *);
int8_t fs_check_limits(char *planta);