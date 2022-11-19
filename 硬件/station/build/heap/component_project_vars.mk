# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/heap/include $(IDF_PATH)/components/heap/port/esp8266/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/heap -lheap
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += heap
COMPONENT_LDFRAGMENTS += 
component-heap-build: 
