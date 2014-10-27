TARGET  := i2c-mips-gpio
WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes -fno-strict-aliasing -mips32
INCLUDE := -isystem /root/whiterussian/openwrt/build_mipsel/linux/include 
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE} -fno-pic -mno-abicalls -mlong-calls
CC      := /root/whiterussian/openwrt/staging_dir_mipsel/bin/mipsel-linux-uclibc-gcc

${TARGET}.o: ${TARGET}.c
