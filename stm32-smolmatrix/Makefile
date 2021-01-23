PROJECT = stm32-smolmatrix
BUILD_DIR = bin

CFILES = $(wildcard *.c)
OPT = -O3
LDLIBS += -lm

# TODO - you will need to edit these two lines!
#DEVICE=stm32l433CBT6
OPENCM3_LIB=opencm3_stm32l4
OPENCM3_DEFS=-DSTM32L4
ARCH_FLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -flto
LDSCRIPT= "stm32l433CBT6_with_settings.ld"
OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

#include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
#include $(OPENCM3_DIR)/mk/genlink-rules.mk

