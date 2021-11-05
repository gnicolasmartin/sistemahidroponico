/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         //
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //
//                                                                                     //
//  Archivo: wifi.c                                                                    //
//  Descripción: Contiene todas las implementaciones de las tareas para la conexión    //
//                wifi                                                                 //
//                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////

#include "wifi.h"

static wifi_ctx_t stCtx;
uint8_t wifi_connected = 0;
char WIFI_SSID[MAX_LENGTH_SSID] = {0};
char WIFI_PSWD[MAX_LENGTH_PSWD] = {0};

void wifi_init(void)
{
  printf("wifi initialization\n");

  if (load_wifi_config() != -1) // Carga red y contraseña wifi en caso de que ya las hayan ingresado
  {
    esp_err_t s32RetVal;

    memset(&stCtx, 0x00, sizeof(stCtx));
    s32RetVal = nvs_flash_init();
    if ((ESP_ERR_NVS_NO_FREE_PAGES == s32RetVal) || (ESP_ERR_NVS_NEW_VERSION_FOUND == s32RetVal))
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      s32RetVal = nvs_flash_init();
    }
    ESP_ERROR_CHECK(s32RetVal);
    stCtx.stWifiEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config =
        {
            .sta =
                {
                    .ssid = SSID_HARDCODEADO,//*WIFI_SSID,
                    .password = PASSWORD_HARDCODEADO,//*WIFI_PSWD,
                    .threshold.authmode = WIFI_AUTH_WPA2_PSK,
                    .pmf_cfg =
                        {
                            .capable = true,
                            .required = false},
                },
        };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_connected = true;
    printf("Successful wifi initialization\n");
  }
  else
  {
    printf("Failed to initialize wifi\n");
  }
  //ESP_LOGI(APP_WIFI_TAG, "Finished wifi initialization");
}

void wifi_wait(void)
{
  printf("Waiting for wifi connection\n");
  //ESP_LOGI(APP_WIFI_TAG, "Waiting for wifi connection");
  stCtx.u32EventBits = xEventGroupWaitBits(stCtx.stWifiEventGroup, APP_WIFI_CONNECTED_BIT | APP_WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  if (stCtx.u32EventBits & APP_WIFI_CONNECTED_BIT)
  {
    //ESP_LOGI(APP_WIFI_TAG, "Connected to AP, SSID: %s password: %s", WIFI_SSID, WIFI_PASSWORD);
  }
  else if (stCtx.u32EventBits & APP_WIFI_FAIL_BIT)
  {
    //ESP_LOGI(APP_WIFI_TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_PASSWORD);
  }
  else
  {
    printf("Wifi Timeout\n");
  }
  ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler));
  ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
  vEventGroupDelete(stCtx.stWifiEventGroup);
}

void wifi_event_handler(void *pvArg, esp_event_base_t pcEventBase, int32_t s32EventId, void *pvEventData)
{
  if ((WIFI_EVENT == pcEventBase) && (WIFI_EVENT_STA_START == s32EventId))
  {
    esp_wifi_connect();
  }
  else if ((WIFI_EVENT == pcEventBase) && (WIFI_EVENT_STA_DISCONNECTED == s32EventId))
  {
    if (stCtx.u08RetryCount < APP_WIFI_MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      stCtx.u08RetryCount++;
      //ESP_LOGI(APP_WIFI_TAG, "Retrying to connect to the AP");
    }
    else
    {
      xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_FAIL_BIT);
    }
    //ESP_LOGI(APP_WIFI_TAG,"Connection to the AP fail");
  }
  else if ((IP_EVENT == pcEventBase) && (IP_EVENT_STA_GOT_IP == s32EventId))
  {
    stCtx.pstIpEvent = (ip_event_got_ip_t *)pvEventData;
    //ESP_LOGI(APP_WIFI_TAG, "Got IP:" IPSTR, IP2STR(&stCtx.pstIpEvent->ip_info.ip));
    stCtx.u08RetryCount = 0;
    xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_CONNECTED_BIT);
  }
}

int save_wifi_config(void)
{
  FILE *config_file = fopen(WIFI_CONFIG_FILE, "w");

  if (config_file == NULL)
  {
    perror("fopen failed");
    return -1;
  }

  fprintf(config_file, "WIFI_SSID: %s\n", WIFI_SSID);
  fprintf(config_file, "WIFI_PSWD: %s\n", WIFI_PSWD);
  fputc('\0', config_file);

  fclose(config_file);
  return 0;
}

int load_wifi_config(void)
{
  FILE *config_file = fopen(WIFI_CONFIG_FILE, "r");

  if (config_file == NULL)
  {
    perror("fopen failed");
    return -1;
  }

  int KEY_LEN = strlen("WIFI_SSID") + 1; // Todas las keys son del mismo largo
  char *KEY = malloc(KEY_LEN);

  while (!feof(config_file))
  {
    fgets(KEY, KEY_LEN, config_file);

    if (!strcmp(KEY, "WIFI_SSID"))
    {
      fgetc(config_file);                   // Lee el ":"
      fscanf(config_file, "%s", WIFI_SSID); // Lee el value de la key
    }
    else if (!strcmp(KEY, "WIFI_PSWD"))
    {
      fgetc(config_file);                   // Lee el ":"
      fscanf(config_file, "%s", WIFI_PSWD); // Lee el value de la key
    }
  }

  printf("Leimos: WIFI_SSID: %s\n", WIFI_SSID);
  printf("Leimos: WIFI_PSWD: %s\n", WIFI_PSWD);
  free(KEY);
  fclose(config_file);

  return 0;
}

/* Scan Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
    This example shows how to scan for available set of APs.
*/
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/event_groups.h"
// #include "esp_wifi.h"
// #include "esp_log.h"
// #include "esp_event.h"
// #include "nvs_flash.h"

// #define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE

// static const char *TAG = "scan";

// static void print_auth_mode(int authmode)
// {
//     switch (authmode) {
//     case WIFI_AUTH_OPEN:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
//         break;
//     case WIFI_AUTH_WEP:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
//         break;
//     case WIFI_AUTH_WPA_PSK:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
//         break;
//     case WIFI_AUTH_WPA2_PSK:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
//         break;
//     case WIFI_AUTH_WPA_WPA2_PSK:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
//         break;
//     case WIFI_AUTH_WPA2_ENTERPRISE:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
//         break;
//     case WIFI_AUTH_WPA3_PSK:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
//         break;
//     case WIFI_AUTH_WPA2_WPA3_PSK:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
//         break;
//     default:
//         ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
//         break;
//     }
// }

// static void print_cipher_type(int pairwise_cipher, int group_cipher)
// {
//     switch (pairwise_cipher) {
//     case WIFI_CIPHER_TYPE_NONE:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
//         break;
//     case WIFI_CIPHER_TYPE_WEP40:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
//         break;
//     case WIFI_CIPHER_TYPE_WEP104:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
//         break;
//     case WIFI_CIPHER_TYPE_TKIP:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
//         break;
//     case WIFI_CIPHER_TYPE_CCMP:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
//         break;
//     case WIFI_CIPHER_TYPE_TKIP_CCMP:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
//         break;
//     default:
//         ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
//         break;
//     }

//     switch (group_cipher) {
//     case WIFI_CIPHER_TYPE_NONE:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
//         break;
//     case WIFI_CIPHER_TYPE_WEP40:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
//         break;
//     case WIFI_CIPHER_TYPE_WEP104:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
//         break;
//     case WIFI_CIPHER_TYPE_TKIP:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
//         break;
//     case WIFI_CIPHER_TYPE_CCMP:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
//         break;
//     case WIFI_CIPHER_TYPE_TKIP_CCMP:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
//         break;
//     default:
//         ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
//         break;
//     }
// }

// /* Initialize Wi-Fi as sta and set scan method */
// static void wifi_scan(void)
// {
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
//     assert(sta_netif);

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     uint16_t number = DEFAULT_SCAN_LIST_SIZE;
//     wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
//     uint16_t ap_count = 0;
//     memset(ap_info, 0, sizeof(ap_info));

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_start());
//     esp_wifi_scan_start(NULL, true);
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
//     ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
//     for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
//         ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
//         ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
//         print_auth_mode(ap_info[i].authmode);
//         if (ap_info[i].authmode != WIFI_AUTH_WEP) {
//             print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
//         }
//         ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
//     }

// }

// void app_main(void)
// {
//     // Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK( ret );

//     wifi_scan();
// }
