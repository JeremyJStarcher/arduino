cd ../examples/mega-to-mega-test/

killall minicom

PROJECT_NAME=mega-to-mega-test

rm ${PROJECT_NAME}*.elf
rm ${PROJECT_NAME}*.hex


MASTER_DEVICE=/dev/ttyACM1
SLAVE_DEVICE=/dev/ttyACM2

UPLOAD_FLAGS="--verify --verbose"
DEFINES=
arduino-cli compile --fqbn arduino:avr:mega `pwd`

echo Uploadig to master
arduino-cli upload --port ${MASTER_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}

echo Uploading to slave
arduino-cli upload --port ${SLAVE_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}

if [ -f ${PROJECT_NAME}.arduino.avr.mega.elf ]; then
  minicom --device ${MASTER_DEVICE} --baud 9600  --capturefile /tmp/master.cap
  minicom --device ${SLAVE_DEVICE} --baud 9600 --capturefile /tmp/slave.cap
fi

rm ${PROJECT_NAME}*.elf
rm ${PROJECT_NAME}*.hex
