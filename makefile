APP1 := vibratorclient
APP2 := vibratorserver

NDK_DIR := /Users/jmcf/Tools/android-ndk
#NDK_DIR := ~/Android/android-ndk-1.6_r1
#NDK_HOST := windows
#NDK_HOST := linux-x86
NDK_HOST := darwin-x86
SDKTOOL := /Users/jmcf/Tools/android/platform-tools

TOOLCHAIN_PREFIX := $(NDK_DIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(NDK_HOST)/bin/arm-linux-androideabi-
CC := $(TOOLCHAIN_PREFIX)gcc
CPP := $(TOOLCHAIN_PREFIX)g++
LD := $(CC)

COMMON_FLAGS := -mandroid -ffunction-sections -fdata-sections -Os -g 	--sysroot=$(NDK_DIR)/platforms/android-9/arch-arm \
	-fPIC \
	-fvisibility=hidden \
	-D__NEW__

CFLAGS := $(COMMON_FLAGS)

CFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__ -DANDROID -DSK_RELEASE -DNDEBUG

CFLAGS += -UDEBUG -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -ffunction-sections -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic


CPPFLAGS := $(COMMON_FLAGS) \
	-fno-rtti -fno-exceptions \
	-fvisibility-inlines-hidden

LDFLAGS += --sysroot=$(NDK_DIR)/platforms/android-9/arch-arm
LDFLAGS +=  -Bdynamic -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc  
LDFLAGS += -L$(NDK_DIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(NDK_HOST)/lib/gcc/arm-linux-androideabi
LDFLAGS += -L$(NDK_DIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(NDK_HOST)/lib/gcc/arm-linux-androideabi/4.4.3
LDFLAGS += -L$(NDK_DIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(NDK_HOST)/lib/gcc
LDFLAGS += -L$(NDK_DIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(NDK_HOST)/lib
LDFLAGS += -nostdlib -lc  -llog -lgcc -landroid -lhardware_legacy -lwpa_client -lnetutils -lcutils -lImmVibeJ\
	--no-undefined -z $(NDK_DIR)/platforms/android-9/arch-arm/usr/lib/crtbegin_dynamic.o $(NDK_DIR)/platforms/android-9/arch-arm/usr/lib/crtend_android.o

OBJS1 += $(APP1).o
OBJS2 += $(APP2).o

all:	client server

print: 
	@echo $(OBJS1)
	@echo $(OBJS2)

client: $(APP1)

server: $(APP2)

$(APP1):	$(OBJS1)
	$(LD) $(LDFLAGS) -o $@ $^
	
$(APP2):	$(OBJS2)
	$(LD) $(LDFLAGS) -o $@ $^	

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@


%.o:	%.cpp
	$(CPP) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

	
%.o:	%.cc
	$(CPP) -c $(CFLAGS) $(CPPFLAGS) $< -o $@	


install:	$(APP1) $(APP2)
	$(SDKTOOL)/adb push $(APP1) /data/local/bin/$(APP1)
	$(SDKTOOL)/adb shell chmod 755 /data/local/bin/$(APP1)
	$(SDKTOOL)/adb push $(APP2) /data/local/bin/$(APP2)
	$(SDKTOOL)/adb shell chmod 755 /data/local/bin/$(APP2)


run:
	$(SDKTOOL)/adb shell /data/local/bin/$(APP1)


clean:
	@rm -f $(APP1).o $(APP1)
	@rm -f $(APP2).o $(APP2)