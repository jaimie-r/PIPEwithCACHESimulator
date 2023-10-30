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

# Generic rules

%.o: %.c
	${CC} ${CC_OPTIONS} ${CC_FLAGS} $<

# Targets

all: se test clean

se: 
	(cd base && make $@)
	(cd pipe && make $@)
	(cd cache && make $@)

test:
	(cd testbench && make $@)

depend:
	${MD} -- ${CC_OPTIONS} ${CC_FLAGS} -- ${SRCS}

clean:
	(cd base && make $@)
	(cd pipe && make $@)
	(cd cache && make $@)
	(cd testbench && make $@)
	${RM} *.o *.so *.bak
