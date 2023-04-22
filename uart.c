#include <avr/io.h>

static void uart_init(uint32_t baud) {
    const uint32_t ubrr = (F_CPU / 4 / baud - 1) / 2;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr >> 0);
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
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

int main() {
    DDRB |= 0x20;
    uart_init(115200);
    while (1) {
        PINB |= 0x20;
        uart_tx(uart_rx());
    }
}
