#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_spiffs.h"

void init_fs(esp_vfs_spiffs_conf_t* conf);