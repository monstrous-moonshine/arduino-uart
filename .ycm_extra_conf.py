def Settings(**kwargs):
    return {
            'flags': [
                '-I/usr/avr/include',
                '-D__AVR_ATmega328P__',
                '-DF_CPU=16000000UL',
                '-Wno-unknown-attributes',
                ]}
