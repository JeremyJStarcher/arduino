killall minicom

rm *.elf
rm *.hex

MASTER_DEVICE=/dev/ttyACM1
SLAVE_DEVICE=/dev/ttyACM2

UPLOAD_FLAGS="--verify --verbose"
DEFINES=
arduino-cli compile --fqbn arduino:avr:mega `pwd`

echo Uploading to slave
arduino-cli upload --port ${SLAVE_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}

echo Uploadig to master
arduino-cli upload --port ${MASTER_DEVICE} --fqbn arduino:avr:mega `pwd` ${UPLOAD_FLAGS}


if [ -f 3.arduino.avr.mega.elf ]; then
  minicom --device ${MASTER_DEVICE} --baud 9600  --capturefile /tmp/master.cap
  minicom --device ${SLAVE_DEVICE} --baud 9600 --capturefile /tmp/slave.cap
fi

rm *.elf
rm *.hex
