# STUDENTS: DO NOT MODIFY.
#
# Definitions

CC = gcc
CC_FLAGS = -Wall -ggdb -UDEBUG -I../include -I../include/base -I../include/pipe -I../include/cache
CC_OPTIONS = -c
CC_SO_OPTIONS = -shared -fpic
CC_DL_OPTIONS = -rdynamic
RM = /bin/rm -f
LD = gcc
LIBS = -ldl
MD = gccmakedep

SRCS := \
test-csim.c \
test-se.c

OBJS := $(SRCS:%.c=%.o)

# Generic rules

%.o: %.c
	${CC} ${CC_OPTIONS} ${CC_FLAGS} $<

# Targets

all: se test clean

se: ${OBJS}

test: ${OBJS}

depend:
	${MD} -- ${CC_OPTIONS} ${CC_FLAGS} -- ${SRCS}

clean:
	${RM} *.o *.so *.bak
