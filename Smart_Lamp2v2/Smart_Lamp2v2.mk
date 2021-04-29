NAME := apps_SmartFlow_SmartTorch_Smart_Lamp2v2

$(NAME)_SOURCES := Smart_Lamp2v2.c
$(NAME)_SOURCES    += i2c.c

#GLOBAL_DEFINES += WICED_DISABLE_STDIO

WIFI_CONFIG_DCT_H := wifi_config_dct.h


APPLICATION_DCT := dct_data_write.c



