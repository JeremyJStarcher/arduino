DEVICE		=atmega8
FILENAME	=blink
PROGRAMMER	=stk500v1
PORT		=/dev/ttyACM0
BAUD		=19200

COMPILE=avr-gcc -Wall -g -Os -mmcu=${DEVICE} 

default: compile upload clean

compile:
	${COMPILE} -c ${FILENAME}.c -o ${FILENAME}.o
	${COMPILE} -o ${FILENAME}.elf ${FILENAME}.o
	avr-objcopy -j .text -j .data -O ihex ${FILENAME}.elf ${FILENAME}.hex
	avr-size --format=avr --mcu=${DEVICE} ${FILENAME}.elf

upload:
#	avrdude -v -p${DEVICE} -c${PROGRAMMER} -P${PORT} -b${BAUD} -Uflash:w:${FILENAME}.hex:i

	avrdude -v -p ${DEVICE} -c ${PROGRAMMER} -P ${PORT} -b ${BAUD} -Uflash:w:${FILENAME}.hex:i


clean:
	rm ${FILENAME}.o
	rm ${FILENAME}.elf
	rm ${FILENAME}.hex


readfuse:
	avrdude -c ${PROGRAMMER} -p ${DEVICE} -P ${PORT} -b ${BAUD} -v -U lfuse:r:-:h 

writefuse:
	echo	avrdude -c ${PROGRAMMER} -p ${DEVICE} -P ${PORT} -b ${BAUD} -v -U lfuse:w:0xFF:m


