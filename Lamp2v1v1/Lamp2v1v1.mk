NAME := apps_SmartFlow_SmartTorch_Lamp2v1v1

$(NAME)_SOURCES := Lamp2v1v1.c
$(NAME)_SOURCES    += i2c.c

#GLOBAL_DEFINES += WICED_DISABLE_STDIO

WIFI_CONFIG_DCT_H := wifi_config_dct.h


APPLICATION_DCT := dct_data_write.c



