#define USB_BAUD 9600
#define UART_BAUD 9600

#define BOARD_ROLE_ADDRESS 0
#define BOARD_ROLE_MASTER 200
#define BOARD_ROLE_SLAVE 50


const size_t buffer_size = 8;
const size_t msg_chunk_size = buffer_size - 2;

static uint8_t send_buffer[buffer_size];
static uint8_t receive_buffer[buffer_size];
