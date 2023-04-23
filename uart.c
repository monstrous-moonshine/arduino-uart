#include <avr/io.h>

void uart_init() {
#define BAUD 115200
#define BAUD_TOL 3
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A = (1 << U2X0);
#endif
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void uart_tx(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

uint8_t uart_rx() {
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

int main() {
    // configure LED pin as output
    DDRB |= 0x20;

    uart_init();
    while (1) {
        uart_tx(uart_rx());
        // toggle LED
        PINB |= 0x20;
    }
}
