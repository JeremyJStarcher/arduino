#define USB_BAUD 9600
#define UART_BAUD 19200

#define FILL_BYTE 0xA0

const size_t buffer_size = 8;
const size_t msg_chunk_size = buffer_size - 2;

static uint8_t send_buffer[buffer_size];
static uint8_t receive_buffer[buffer_size];
