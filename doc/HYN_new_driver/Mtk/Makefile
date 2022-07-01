
# Linux driver folder
# ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/hynitron/
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/hwmon/include/

ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/hynitron/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
obj-y	+= hynitron_core.o
obj-y	+= hynitron_i2c.o
obj-y	+= hynitron_esd_check.o
obj-y	+= hynitron_tool_debug.o
obj-y	+= hynitron_update_firmware.o
obj-y	+= hynitron_gesture.o
obj-y	+= hynitron_proximity.o
