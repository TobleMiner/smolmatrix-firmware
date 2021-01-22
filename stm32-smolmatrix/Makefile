PROJECT = stm32-smolmatrix
BUILD_DIR = bin

CFILES = $(wildcard *.c)
OPT = -O3
LDLIBS += -lm

# TODO - you will need to edit these two lines!
DEVICE=stm32l433CBT6
OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
ARCH_FLAGS+=-flto
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

