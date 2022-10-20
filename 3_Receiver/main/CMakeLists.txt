idf_component_register(SRCS "string_nvs.c" "softAP.c" "main.c"
                    INCLUDE_DIRS ".")

                    spiffs_create_partition_image(www "${CMAKE_CURRENT_SOURCE_DIR}/../frontend" FLASH_IN_PROJECT)