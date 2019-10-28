killall xterm
rm /tmp/master
rm /tmp/slave

 SLAVE_DEVICE="\"/dev/ttyUSB1\""
MASTER_DEVICE="\"/dev/ttyUSB0\""

DEFINES="-DBAUD=B${BAUD} -DSLAVE_DEVICE=${SLAVE_DEVICE} -DMASTER_DEVICE=${MASTER_DEVICE}"

GCC_ARGS="-g3 -Wall "

g++ ${GCC_ARGS} runner.cpp tests.cpp ${DEFINES} -DMASTER -o /tmp/master
g++ ${GCC_ARGS} runner.cpp tests.cpp ${DEFINES} -DSLAVE -o /tmp/slave

xterm -e /tmp/master -sb +j 2000 &
xterm -e /tmp/slave -sb +j 2000 &
