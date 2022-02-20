/////////////////////////////////////////////////////////////////////////////////////////
//  Proyecto: Sistema Hidropónico Para Hogares                                         // 
//  Alumnos: *Lucia Sucunza                                                            //
//           *Nicolas Gomez                                                            //
//           *Santiago Laborde                                                         //                          
//                                                                                     // 
//  Archivo: filesystem.c                                                              // 
//  Descripción: Contiene todas las funciones asociadas al manejo del filesystem SPIFFS//
//                                                                                     // 
/////////////////////////////////////////////////////////////////////////////////////////

#include "filesystem.h"

const char* spiffs_test_partition_label = "flash_test";


void init_rom()
{
    esp_err_t s32RetVal = nvs_flash_init();
    if ((ESP_ERR_NVS_NO_FREE_PAGES == s32RetVal) || (ESP_ERR_NVS_NEW_VERSION_FOUND == s32RetVal))
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      s32RetVal = nvs_flash_init();
    }

    ESP_ERROR_CHECK(s32RetVal);
}

void fs_init(esp_vfs_spiffs_conf_t* conf)
{
    printf("Initializing SPIFFS\n");

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(conf);

    if (ret != ESP_OK) 
    {
        if (ret == ESP_FAIL) 
        {
            printf("Failed to mount or format filesystem\n");
        } 
        else if (ret == ESP_ERR_NOT_FOUND) 
        {
            printf("Failed to find SPIFFS partition\n");
        } 
        else 
        {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info((*conf).partition_label, &total, &used);
    
    if (ret != ESP_OK) 
    {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    } 
    else 
    {
        printf("Partition size: total: %d, used: %d\n", total, used);
    }
}