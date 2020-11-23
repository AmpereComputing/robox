WITH_EXAGEAR ?= true
ifeq ($(WITH_EXAGEAR),true)

ifeq ($(TARGET_BUILD_VARIANT),user)
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    exagear.debug=off
else
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    exagear.debug=off \
    exagear.log_dir="/data/exagear/log" \
    exagear.coredump_dir="/data/exagear/cores"
PRODUCT_COPY_FILES += \
    vendor/huawei/exagear/init/exagear-debug.rc:system/etc/init/exagear-debug.rc:huawei
endif

BOARD_SEPOLICY_DIRS += vendor/huawei/exagear/sepolicy

# Use 64-bit system servers
AUDIOSERVER_MULTILIB := 64
TARGET_ENABLE_MEDIADRM_64 := true

endif # WITH_EXAGEAR
