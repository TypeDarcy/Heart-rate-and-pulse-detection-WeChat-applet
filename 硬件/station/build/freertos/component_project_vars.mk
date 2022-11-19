# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/freertos/include $(IDF_PATH)/components/freertos/include $(IDF_PATH)/components/freertos/include/freertos $(IDF_PATH)/components/freertos/include/freertos/private $(IDF_PATH)/components/freertos/port/esp8266/include $(IDF_PATH)/components/freertos/port/esp8266/include/freertos
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/freertos -lfreertos
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += freertos
COMPONENT_LDFRAGMENTS += $(IDF_PATH)/components/freertos/linker.lf
component-freertos-build: 
