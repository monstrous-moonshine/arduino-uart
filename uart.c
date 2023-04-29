#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/*
#include <stdio.h>

static void uart_tx(uint8_t data);
static int uart_putchar(char c, FILE *stream) {
    (void)stream;
    uart_tx(c);
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
        _FDEV_SETUP_WRITE);
*/

#define LINEBUF_LEN 256
static struct {
    char buf[LINEBUF_LEN];
    int pos, len;
} line = {
    .pos = 0, .len = 0
};

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

static void uart_puti(int n) {
    if (n == 0) {
        uart_tx('0');
        return;
    }
    int digits[10];
    int neg = n < 0;
    n = neg ? -n : n;
    int i;
    for (i = 0; n; i++) {
        digits[i] = n % 10;
        n /= 10;
    }
    if (neg)
        uart_tx('-');
    while (i)
        uart_tx('0' + digits[--i]);
}

static void line_insert(char c) {
    if (line.len == LINEBUF_LEN)
        return;

    for (int i = line.len; i > line.pos; i--)
        line.buf[i] = line.buf[i-1];

    line.buf[line.pos] = c;
    line.pos++;
    line.len++;
}

static void line_delchr() {
    if (line.pos >= line.len)
        return;

    for (int i = line.pos; i < line.len - 1; i++)
        line.buf[i] = line.buf[i+1];

    line.len--;
}

static void disp_setcur(int n) {
    uart_tx('\033');
    uart_tx('[');
    uart_puti(n);
    uart_tx('G');
}

static void disp_line(int n_prevchar) {
    for (int i = line.pos - n_prevchar; i < line.len; i++)
        uart_tx(line.buf[i]);
}

static void disp_erase() {
    uart_puts("\033[K");
}

static void disp_home() {
    uart_puts("\033[G");
}

static void disp_end() {
    disp_setcur(line.len + 1);
}

static void disp_refresh(int n_prevchar) {
    disp_erase();
    disp_line(n_prevchar);
    disp_setcur(line.pos + 1);
}

#define CTRL(c) (c & 0x1f)

void handle_c0(char c) {
    switch (c) {
    case CTRL('a'):
        line.pos = 0;
        disp_home();
        break;
    case CTRL('b'):
        if (line.pos > 0) {
            line.pos--;
            uart_puts("\033[D");
        }
        break;
    case CTRL('d'):
        if (line.pos < line.len) {
            line_delchr();
            disp_refresh(0);
        }
        break;
    case CTRL('e'):
        line.pos = line.len;
        disp_end();
        break;
    case CTRL('f'):
        if (line.pos < line.len) {
            line.pos++;
            uart_puts("\033[C");
        }
        break;
    case CTRL('h'):
    case 127:
        if (line.pos > 0) {
            line.pos--;
            uart_puts("\033[D");
            line_delchr();
            disp_refresh(0);
        }
        break;
    case CTRL('i'):
        uart_tx(c);
        break;
    case CTRL('j'):
    case CTRL('m'):
        uart_puts("\r\n");
        line.pos = 0;
        line.len = 0;
        break;
    case CTRL('k'):
        disp_erase();
        line.len = line.pos;
        break;
    case CTRL('l'):
        uart_puts("\033[H\033[J");
        disp_line(line.pos);
        disp_setcur(line.pos + 1);
        break;
    case CTRL('u'):
        for (int i = 0; i < line.len - line.pos; i++)
            line.buf[i] = line.buf[line.pos + i];
        line.len -= line.pos;
        line.pos = 0;
        disp_home();
        disp_erase();
        disp_line(0);
        disp_home();
        break;
    default:
        break;
    }
}

ISR(USART_RX_vect) {
    char c = uart_rx();
    // uart_puti(c);
    // uart_puts("\r\n");
    if (c < 0x20 || c == 127) {
        handle_c0(c);
    } else {
        line_insert(c);
        disp_refresh(1);
    }
    // toggle LED
    PINB |= 0x20;
}

int main() {
    // configure LED pin as output
    DDRB |= 0x20;

    // stdout = &mystdout;
    uart_init();
    sei();
    while (1) {
        sleep_mode();
    }
}
