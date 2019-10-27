killall xterm
rm /tmp/master
rm /tmp/slave

 SLAVE_DEVICE="\"/dev/ttyUSB1\""
MASTER_DEVICE="\"/dev/ttyUSB0\""
BAUD=115200

DEFINES="-DBAUD=B${BAUD} -DSLAVE_DEVICE=${SLAVE_DEVICE} -DMASTER_DEVICE=${MASTER_DEVICE}"

GCC_ARGS="-g3 -Wall "

g++ ${GCC_ARGS} test.cpp ${DEFINES} -DMASTER -o /tmp/master
g++ ${GCC_ARGS} test.cpp ${DEFINES} -DSLAVE -o /tmp/slave

xterm -e /tmp/master &
xterm -e /tmp/slave &
