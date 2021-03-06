/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     // 
//  Archivo: firestore.h                                                               // 
//  Descripción: Archivo de cabecera para el manejo del wifi                           //
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

// CONFIG DEFINES
#ifndef WIFI_H_
#define WIFI_H_

// INCLUDES
#include <esp_wifi.h>
#include <esp_event.h>
#include "esp_log.h"
//_________ ver estos includes
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
//_________

// DEFINES
#define APP_WIFI_TAG            "APP_WIFI"
#define APP_WIFI_MAXIMUM_RETRY  0xFFFFFFFF
#define APP_WIFI_CONNECTED_BIT  BIT0
#define APP_WIFI_FAIL_BIT       BIT1
#define MAX_LENGTH_SSID         20
#define MAX_LENGTH_PSWD         20
#define DEFAULT_SCAN_LIST_SIZE  10
// WiFi Credentials
#define SSID_HARDCODEADO "Telecentro-1188_EXT"
#define PASSWORD_HARDCODEADO "poligono425"
#define WIFI_CONFIG_FILE "/spiffs/wifi.yml"

// STRUCTS
typedef struct
{
  EventGroupHandle_t stWifiEventGroup;
  int8_t u08RetryCount;
  ip_event_got_ip_t* pstIpEvent;
  EventBits_t u32EventBits;
}wifi_ctx_t;

// PROTOTYPES
void wifi_init(void);
void wifi_wait(void);
void wifi_event_handler(void* pvArg, esp_event_base_t pcEventBase, int32_t s32EventId, void* pvEventData);
int save_wifi_config(void);
int load_wifi_config(void);
void wifi_scan(void);
void wifi_remove(void);
void wifi_connect(void);
void print_cipher_type(int pairwise_cipher, int group_cipher);
void print_auth_mode(int authmode);

#endif /* WIFI_H_ */