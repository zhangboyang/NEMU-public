##### global settings #####

# 'count' add by ZBY
.PHONY: nemu entry all_testcase kernel run gdb test submit clean count

CC := gcc
HOSTCC := gcc
TARGETCC := i386-nemu-linux-musl-gcc
LD := ld
HOSTLD := ld
TARGETLD := i386-nemu-linux-musl-ld
CXX := g++
HOSTCXX := g++
TARGETCXX := i386-nemu-linux-musl-g++

CFLAGS := -MMD -Wall -Werror -c -g

LIB_COMMON_DIR := lib-common
NEWLIBC_DIR := $(LIB_COMMON_DIR)/newlib
NEWLIBC := $(NEWLIBC_DIR)/newlib.a
FLOAT := obj/$(LIB_COMMON_DIR)/FLOAT.a

include config/Makefile.git
include config/Makefile.build

all: nemu all_testcase kernel game entry


##### rules for building the project #####

include nemu/Makefile.part
include testcase/Makefile.part
include lib-common/Makefile.part
include kernel/Makefile.part
include game/Makefile.part

nemu: $(nemu_BIN) $(FLOAT)
all_testcase: $(testcase_BIN)
kernel: $(kernel_BIN)
game: $(game_BIN)


##### rules for cleaning the project #####

clean-nemu:
	-rm -rf obj/nemu 2> /dev/null

clean-testcase:
	-rm -rf obj/testcase 2> /dev/null

clean-kernel:
	-rm -rf obj/kernel 2> /dev/null

clean-game:
	-rm -rf obj/game 2> /dev/null

clean: clean-cpp
	-rm -rf obj 2> /dev/null
	-rm -f *log.txt entry $(FLOAT) 2> /dev/null
#	cd ZBYcalc; make clean


##### some convinient rules #####

USERPROG := $(game_BIN)
#USERPROG := obj/testcase/checkzero
ENTRY := $(kernel_BIN)

entry: $(ENTRY)
	objcopy -S -O binary $(ENTRY) entry

run: $(nemu_BIN) $(USERPROG) entry
	$(call git_commit, "run")
	$(nemu_BIN) $(USERPROG)

gdb: $(nemu_BIN) $(USERPROG) entry
	gdb -s $(nemu_BIN) --args $(nemu_BIN) $(USERPROG)

test: $(nemu_BIN) $(testcase_BIN) entry
	bash test.sh $(testcase_BIN)

submit: clean
	cd .. && tar cvj $(shell pwd | grep -o '[^/]*$$') > $(STU_ID).tar.bz2
