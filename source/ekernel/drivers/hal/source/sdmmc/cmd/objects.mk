ifeq ($(CONFIG_DRIVERS_SDMMC), y)
SDMMC_TEST_OBJECTS += $(BUILD_DIR)/drivers/chip-src/sdmmc/cmd/cmd_sd.o

SDMMC_TEST_OBJECTS += $(SDMMC_TEST_OBJECTS-y)

$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/drivers/chip-src/sdmmc/cmd/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/include/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/port/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/freertos/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/kernel/Posix/include/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/kernel/FreeRTOS/Source/portable/GCC/ARM_CA9/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/components/thirdparty/unity/src/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/components/thirdparty/unity/extras/fixture/src/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/osal/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/osal/FreeRTOS/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/hal/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/sys/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/components/thirdparty/console/
$(SDMMC_TEST_OBJECTS):CFLAGS += -I $(BASE)/include/drivers/chip/sdmmc/cmd/
$(SDMMC_TEST_OBJECTS):MODULE_NAME="sdmmc-test"

OBJECTS += $(SDMMC_TEST_OBJECTS)
endif
