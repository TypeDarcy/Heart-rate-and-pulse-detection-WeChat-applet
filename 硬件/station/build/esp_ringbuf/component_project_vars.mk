# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/esp_ringbuf/include $(IDF_PATH)/components/esp_ringbuf/include/freertos
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/esp_ringbuf -lesp_ringbuf
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += esp_ringbuf
COMPONENT_LDFRAGMENTS += 
component-esp_ringbuf-build: 
