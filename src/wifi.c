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
esp_netif_t *sta;
bool WIFI_IS_CONNECTED = false;
bool FORCE_DISCONECTION = false;
char WIFI_SSID[MAX_LENGTH_SSID] = {};
char WIFI_PSWD[MAX_LENGTH_PSWD] = {};
char WIFI_SSIDS[DEFAULT_SCAN_LIST_SIZE][MAX_LENGTH_SSID] = {""};
int  WIFI_SSIDS_SCANNED= 0;

void wifi_init(void)
{
  // INICIALIZACIONES (Alerta no pueden correrse mas de una vez)
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  sta=esp_netif_create_default_wifi_sta();
  assert(sta);

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  load_wifi_config();
  wifi_connect(); // if fails WIFI_IS_CONNECTED will be false

}

void wifi_connect()
{
  esp_log_level_set("wifi", ESP_LOG_NONE);
  esp_log_level_set("wifi_init", ESP_LOG_NONE);
  esp_log_level_set("system_api", ESP_LOG_NONE);
  esp_log_level_set("phy_init", ESP_LOG_NONE);

  if(WIFI_IS_CONNECTED)
  {
    // Detenemos conexión anterior
    FORCE_DISCONECTION= true;
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    FORCE_DISCONECTION= false;
  }

  // Declaramos la estructura wifi_config
  wifi_config_t wifi_config;
  // Inicializamos la estructura wifi_config
  memset(&wifi_config, 0, sizeof(wifi_config));
  // Rellenamos la estructura wifi_config
  strcpy((char*) wifi_config.sta.ssid, WIFI_SSID);
  strcpy((char*) wifi_config.sta.password, WIFI_PSWD);
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.pmf_cfg.capable= false;
  wifi_config.sta.pmf_cfg.required= false;

  // Iniciamos nueva conexión
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_connect());
  
  wifi_wait();  // if WiFi is connected, the FLAG will be activated

  // In case we don´t want to try reconnection if the WiFi is not connected
  // if(!WIFI_IS_CONNECTED)
  // {
  //   FORCE_DISCONECTION= true;
  //   esp_wifi_disconnect();
  //   FORCE_DISCONECTION= false;
  // }
}

/* Initialize Wi-Fi as sta and set scan method */
void wifi_scan(void)
{
  esp_log_level_set("wifi", ESP_LOG_NONE);
  esp_log_level_set("wifi_init", ESP_LOG_NONE);
  esp_log_level_set("system_api", ESP_LOG_NONE);
  esp_log_level_set("phy_init", ESP_LOG_NONE);

  // Detenemos conexión anterior o intento de reconexión
  FORCE_DISCONECTION= true;
  ESP_ERROR_CHECK(esp_wifi_disconnect());
  FORCE_DISCONECTION= false;

  uint16_t number = DEFAULT_SCAN_LIST_SIZE;
  wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
  uint16_t ap_count = 0;
  memset(ap_info, 0, sizeof(ap_info));

  ESP_ERROR_CHECK(esp_wifi_start());
  esp_wifi_scan_start(NULL, true);
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

  // printf("Total APs scanned = %u\n", ap_count);
  WIFI_SSIDS_SCANNED= ap_count;
  for (int i = 0; i < ap_count; i++)
  {
      printf("Detectamos SSID \t\t%s\n", ap_info[i].ssid);
      sprintf(WIFI_SSIDS[i],"%s", ap_info[i].ssid);
  }

  ESP_ERROR_CHECK(esp_wifi_scan_stop());
}

void wifi_wait(void)
{
  printf("Waiting for wifi connection\n");
  memset(&stCtx, 0x00, sizeof(stCtx));
  stCtx.stWifiEventGroup = xEventGroupCreate();
  stCtx.u32EventBits = xEventGroupWaitBits(stCtx.stWifiEventGroup, APP_WIFI_CONNECTED_BIT | APP_WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY); //);//1000/portTICK_PERIOD_MS);

  if (stCtx.u32EventBits & APP_WIFI_CONNECTED_BIT)
  {
    printf("Connected to SSID: %s password: %s\n", WIFI_SSID, WIFI_PSWD);
    WIFI_IS_CONNECTED = true;
  }
  else if (stCtx.u32EventBits & APP_WIFI_FAIL_BIT)
  {
    printf("Failed to connect to SSID: %s, password: %s\n", WIFI_SSID, WIFI_PSWD);
    WIFI_IS_CONNECTED = false;
  }
  else
  {
    printf("Wifi Timeout %d\n", stCtx.u32EventBits);
    WIFI_IS_CONNECTED = false;
  }
}

void wifi_event_handler(void *pvArg, esp_event_base_t pcEventBase, int32_t s32EventId, void *pvEventData)
{
  if ((WIFI_EVENT == pcEventBase) && (WIFI_EVENT_STA_START == s32EventId))
  {
    printf("[HANDLER] WiFi Firt Trying to connect\n");
    esp_wifi_connect();
  }
  else if ((WIFI_EVENT == pcEventBase) && (WIFI_EVENT_STA_DISCONNECTED == s32EventId))
  {
    if(!FORCE_DISCONECTION)
    {
      if (stCtx.u08RetryCount < APP_WIFI_MAXIMUM_RETRY)
      {
        esp_wifi_connect();
        stCtx.u08RetryCount++;
        printf("[HANDLER] WiFi Retrying to connect\n");
        xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_FAIL_BIT);
      }
      else
      {
        printf("[HANDLER] WiFi Disconnected Maximum retries\n");
        xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_FAIL_BIT);
        WIFI_IS_CONNECTED= false;
      }
    }
    else
    {
        printf("[HANDLER] WiFi Forced Disconnected\n");
        // xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_FAIL_BIT);
        WIFI_IS_CONNECTED= false;
    }
  }
  else if ((IP_EVENT == pcEventBase) && (IP_EVENT_STA_GOT_IP == s32EventId))
  {
    printf("[HANDLER] WiFi Connected\n");
    stCtx.pstIpEvent = (ip_event_got_ip_t *)pvEventData;
    //printf("Got IP:" IPSTR, IP2STR(&stCtx.pstIpEvent->ip_info.ip));
    stCtx.u08RetryCount = 0;
    WIFI_IS_CONNECTED= true;
    xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_CONNECTED_BIT);
  }
  else if((IP_EVENT == pcEventBase) && (WIFI_EVENT_STA_CONNECTED == s32EventId))
  {
    printf("[HANDLER] WiFi Connected\n");
    stCtx.pstIpEvent = (ip_event_got_ip_t *)pvEventData;
    stCtx.u08RetryCount = 0;
    WIFI_IS_CONNECTED= true;
    xEventGroupSetBits(stCtx.stWifiEventGroup, APP_WIFI_CONNECTED_BIT);
  }
  else
  {
    WIFI_IS_CONNECTED= false;
    printf("[HANDLER] Signal Handler not Captured\n");
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

  printf("Config file updated\n");

  return 0;
}

int load_wifi_config(void)
{
  FILE *config_file = fopen(WIFI_CONFIG_FILE, "r");

  if (config_file == NULL)
  {
    perror("fopen failed");    
    strcpy(WIFI_SSID, ""); // WIFI_SSID[0]= "\0";
    strcpy(WIFI_PSWD, ""); // WIFI_PSWD[0]= "\0";
    WIFI_IS_CONNECTED = false;
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

  printf("Leimos: WIFI_SSID: %s y WIFI_PSWD: %s\n", WIFI_SSID, WIFI_PSWD);

  free(KEY);
  fclose(config_file);

  return 0;
}