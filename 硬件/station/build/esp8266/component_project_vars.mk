# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/esp8266/include $(IDF_PATH)/components/esp8266/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/esp8266 -lesp8266 -L$(IDF_PATH)/components/esp8266/lib -lgcc -lhal -lcore -lnet80211 -lphy -lrtc -lclk -lpp -lsmartconfig -lssc -lespnow -L $(IDF_PATH)/components/esp8266/ld -T esp8266_out.ld -T $(BUILD_DIR_BASE)/esp8266/esp8266.project.ld -Wl,--no-check-sections -u call_user_start -u g_esp_sys_info -T esp8266.rom.ld -T esp8266.peripherals.ld
COMPONENT_LINKER_DEPS += $(IDF_PATH)/components/esp8266/lib/libgcc.a $(IDF_PATH)/components/esp8266/lib/libhal.a $(IDF_PATH)/components/esp8266/lib/libcore.a $(IDF_PATH)/components/esp8266/lib/libnet80211.a $(IDF_PATH)/components/esp8266/lib/libphy.a $(IDF_PATH)/components/esp8266/lib/librtc.a $(IDF_PATH)/components/esp8266/lib/libclk.a $(IDF_PATH)/components/esp8266/lib/libpp.a $(IDF_PATH)/components/esp8266/lib/libsmartconfig.a $(IDF_PATH)/components/esp8266/lib/libssc.a $(IDF_PATH)/components/esp8266/lib/libespnow.a $(IDF_PATH)/components/esp8266/ld/esp8266.rom.ld $(IDF_PATH)/components/esp8266/ld/esp8266.peripherals.ld $(BUILD_DIR_BASE)/esp8266/esp8266.project.ld
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += esp8266
COMPONENT_LDFRAGMENTS += $(IDF_PATH)/components/esp8266/ld/esp8266_fragments.lf $(IDF_PATH)/components/esp8266/ld/esp8266_bss_fragments.lf $(IDF_PATH)/components/esp8266/linker.lf
component-esp8266-build: 
