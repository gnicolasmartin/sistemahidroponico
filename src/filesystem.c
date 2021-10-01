// /////////////////////////////////////////////////////////////////////////////////////////
// //  Proyecto: Sistema Hidropónico Para Hogares                                         // 
// //  Alumnos: *Lucia Sucunza                                                            //
// //           *Nicolas Gomez                                                            //
// //           *Santiago Laborde                                                         //                          
// //                                                                                     // 
// //  Archivo: filesystem.c                                                              // 
// //  Descripción: Contiene todas las funciones asociadas al manejo del filesystem SPIFFS//
// //                                                                                     // 
// /////////////////////////////////////////////////////////////////////////////////////////

#include "filesystem.h"

const char* spiffs_test_partition_label = "flash_test";

void init_fs(void)
{
    printf("Initializing SPIFFS\n");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = spiffs_test_partition_label,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

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
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    
    if (ret != ESP_OK) 
    {
        printf("Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } 
    else 
    {
        printf("Partition size: total: %d, used: %d", total, used);
    }

    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    printf("Opening file\n");
    FILE* f = fopen("/spiffs/hello.txt", "w");
    
    if (f == NULL) 
    {
        printf("Failed to open file for writing\n");
        return;
    }

    fprintf(f, "Hello World!\n");
    fclose(f);
    printf("File written\n");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) 
    {
        // Delete it if it exists
        unlink("/spiffs/foo.txt");
    }

    // Rename original file
    printf("Renaming file\n");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) 
    {
        printf("Rename failed\n");
        return;
    }

    // Open renamed file for reading
    printf("Reading file\n");
    f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL) 
    {
        printf("Failed to open file for reading\n");
        return;
    }

    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos) 
    {
        *pos = '\0';
    }
    printf("Read from file: '%s'", line);

    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(conf.partition_label);
    printf("SPIFFS unmounted\n");
}