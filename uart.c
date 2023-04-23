#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

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
#ifdef UART_SYNC_TX
    while (!(UCSR0A & (1 << UDRE0)))
        ;
#endif
    UDR0 = data;
}

static uint8_t uart_rx() {
#ifdef UART_SYNC_RX
    while (!(UCSR0A & (1 << RXC0)))
        ;
#endif
    return UDR0;
}

ISR(USART_RX_vect) {
    uart_tx(uart_rx());
    // toggle LED
    PINB |= 0x20;
}

int main() {
    // configure LED pin as output
    DDRB |= 0x20;

    uart_init();
    sei();
    while (1) {
        sleep_mode();
    }
}
