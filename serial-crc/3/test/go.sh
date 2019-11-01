killall xterm
rm /tmp/master
rm /tmp/slave
rm master.txt
rm slave.txt

 SLAVE_DEVICE="\"/dev/ttyUSB1\""
MASTER_DEVICE="\"/dev/ttyUSB0\""

DEFINES="-DBAUD=B${BAUD} -DSLAVE_DEVICE=${SLAVE_DEVICE} -DMASTER_DEVICE=${MASTER_DEVICE}"

GCC_ARGS="-g3 -Wall "

g++ ${GCC_ARGS} runner.cpp tests.cpp ../xmodem.cpp ${DEFINES} -DMASTER -o /tmp/master
g++ ${GCC_ARGS} runner.cpp tests.cpp ../xmodem.cpp ${DEFINES} -DSLAVE -o /tmp/slave

XTERM_OPTIONS="-fa 'Monospace' -fs 14 "

xterm ${XTERM_OPTIONS} -e /tmp/master  &
xterm ${XTERM_OPTIONS} -e /tmp/slave  &


