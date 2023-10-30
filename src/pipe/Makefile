# Definitions

CC = gcc
CC_FLAGS = -Wall -ggdb -UDEBUG -I../../include  -I../../include/base -I../../include/pipe -I../../include/cache
CC_OPTIONS = -c
CC_SO_OPTIONS = -shared -fpic
CC_DL_OPTIONS = -rdynamic
RM = /bin/rm -f
LD = gcc
LIBS = -ldl
MD = gccmakedep

SRCS := \
forward.c \
hazard_control.c \
instr_base.c \
instr_Fetch.c \
instr_Decode.c \
instr_Execute.c \
instr_Memory.c \
instr_Writeback.c

# SRCS := $(HDRS:%.h=%.c)
OBJS := $(SRCS:%.c=%.o)

# Generic rules

%.o: %.c
	${CC} ${CC_OPTIONS} ${CC_FLAGS} $<

# Targets

all: se clean

se: ${OBJS}

depend:
	${MD} -- ${CC_OPTIONS} ${CC_FLAGS} -- ${SRCS}

clean:
	${RM} *.o *.so *.bak
