killall xterm
rm ./master
rm ./slave

 SLAVE_DEVICE="\"/dev/ttyUSB1\""
MASTER_DEVICE="\"/dev/ttyUSB0\""
BAUD=115200

DEFINES="-DBAUD=B${BAUD} -DSLAVE_DEVICE=${SLAVE_DEVICE} -DMASTER_DEVICE=${MASTER_DEVICE}"

GCC_ARGS="-g3 -Wall "

gcc ${GCC_ARGS} test.c sleep.c ${DEFINES} -DMASTER -o /tmp/master
gcc ${GCC_ARGS} test.c sleep.c ${DEFINES} -DSLAVE -o /tmp/slave

xterm -e /tmp/master &
xterm -e /tmp/slave &


