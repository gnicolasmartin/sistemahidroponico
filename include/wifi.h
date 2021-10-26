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
//_________ ver estos includes
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>
//_________

// DEFINES
#define APP_WIFI_TAG                            "APP_WIFI"
#define APP_WIFI_MAXIMUM_RETRY                   0xFFFFFFFF
#define APP_WIFI_CONNECTED_BIT                   BIT0
#define APP_WIFI_FAIL_BIT                        BIT1
// WiFi Credentials
#define WIFI_SSID "Telecentro-1188_EXT"
#define WIFI_PASSWORD "poligono425"

// STRUCTS
typedef struct
{
  EventGroupHandle_t stWifiEventGroup;
  int8_t u08RetryCount;
  ip_event_got_ip_t* pstIpEvent;
  EventBits_t u32EventBits;
}wifi_ctx_t;

// PROTOTYPES
void init_wifi(void);
void wifi_wait(void);
void wifi_event_handler(void* pvArg, esp_event_base_t pcEventBase, int32_t s32EventId, void* pvEventData);

#endif /* WIFI_H_ */