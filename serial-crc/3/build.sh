killall minicom

SLAVE_DEVICE=/dev/tty2yyACM0
MASTER_DEVICE=/dev/ttyUSB0
UPLOAD_FLAGS="--verify --verbose"
DEFINES=
arduino-cli compile --fqbn arduino:avr:mega `pwd`

echo Uploading to slave
arduino-cli upload --port ${SLAVE_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}

echo Uploadig to master
arduino-cli upload --port ${MASTER_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}

rm *.elf
rm *.hex

#minicom --device ${MASTER_DEVICE} --baud 9600 
minicom --device ${SLAVE_DEVICE} --baud 9600
