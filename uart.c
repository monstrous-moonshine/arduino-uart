#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>

static void uart_tx(uint8_t data);
static int uart_putchar(char c, FILE *stream) {
    (void)stream;
    uart_tx(c);
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
        _FDEV_SETUP_WRITE);

void uart_init() {
#define BAUD 115200
#define BAUD_TOL 3
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A = (1 << U2X0);
#endif
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);
}

static void uart_tx(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

static uint8_t uart_rx() {
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

static void uart_puts(const char *str) {
    for (const char *c = str; *c; c++)
        uart_tx(*c);
}

#define CTRL(c) (c & 0x1f)

ISR(USART_RX_vect) {
    char c = uart_rx();
    // printf("%d\r\n", c);
    switch (c) {
    case CTRL('a'):
        uart_puts("\033[G");
        break;
    case CTRL('l'):
        uart_puts("\033[H\033[J");
        break;
    default:
        uart_tx(c);
        break;
    }
    // toggle LED
    PINB |= 0x20;
}

int main() {
    // configure LED pin as output
    DDRB |= 0x20;

    stdout = &mystdout;
    uart_init();
    sei();
    while (1) {
        sleep_mode();
    }
}
