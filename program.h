int main(void) __attribute__((section(".main")));

void handle_sci() __attribute__((interrupt));
void handle_display();
void handle_steppers();

void initialize_display();
void initialize_stepper();
void initialize_serial();
void initialize_memory();

struct interrupt_vector {
  unsigned char instruction;
  unsigned short address;
} __attribute__((packed));

const unsigned char stepper_steps[2][6] = {{0x60, 0x40, 0xD0, 0x90, 0xB0, 0x20}, {0x06, 0x04, 0x0D, 0x09, 0x0B, 0x02}};

extern volatile unsigned char _PORTA;
extern volatile unsigned char _TFLG2;
extern volatile unsigned char _TCNTH;
extern volatile unsigned char _SCDR;
extern volatile unsigned char _SCSR;
extern volatile unsigned char _SCCR2;

extern volatile unsigned char _STEPPER;
extern volatile unsigned char _LCD_CTRL;
extern volatile unsigned char _LCD_DATA;

extern unsigned short _bss_start;
extern unsigned short _bss_end;

extern struct interrupt_vector _SCI_VECTOR;
