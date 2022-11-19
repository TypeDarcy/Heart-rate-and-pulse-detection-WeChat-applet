# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/newlib/platform_include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/newlib -lnewlib -lc_nano -lm -u __errno
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += newlib
COMPONENT_LDFRAGMENTS += 
component-newlib-build: 
