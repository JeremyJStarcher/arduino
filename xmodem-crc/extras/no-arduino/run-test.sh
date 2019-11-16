killall xterm
rm /tmp/master
rm /tmp/slave
rm master.txt
rm slave.txt

 SLAVE_DEVICE="\"/dev/ttyUSB1\""
MASTER_DEVICE="\"/dev/ttyUSB0\""

DEFINES="-DBAUD=B${BAUD} -DSLAVE_DEVICE=${SLAVE_DEVICE} -DMASTER_DEVICE=${MASTER_DEVICE} -DNO_ARDUINO"
GCC_ARGS="-g3 -Wall -I ../../src"
FILES="runner.cpp ../../src/xmodem.cpp ../../examples/mega-to-mega-test/tests.cpp"

g++ ${GCC_ARGS} ${FILES} ${DEFINES} -DMASTER -o /tmp/master
g++ ${GCC_ARGS} ${FILES} ${DEFINES} -DSLAVE -o /tmp/slave

XTERM_OPTIONS="-fa 'Monospace' -fs 14 "

xterm ${XTERM_OPTIONS} -e /tmp/master  &
xterm ${XTERM_OPTIONS} -e /tmp/slave  &

