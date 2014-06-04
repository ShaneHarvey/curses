CC=gcc
LIBS=-lncurses

SRC_DIR=.
EXE_DIR=./exe

EXES = \
    ${EXE_DIR}/clife

${EXE_DIR}/%: %.o
	${CC} -o $@ $< ${LIBS}

%.o: ${SRC_DIR}/%.c
	${CC} -o $@ -c $<

all:    ${EXES}

clean:
	@rm -f ${EXES}
