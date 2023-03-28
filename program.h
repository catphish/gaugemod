int main(void) __attribute__((section(".main")));

void sci_handler() __attribute__((interrupt));

struct interrupt_vector {
  unsigned char instruction;
  unsigned short address;
} __attribute__((packed));

const unsigned char stepper_steps[6] = {0x66, 0x44, 0xDD, 0x99, 0xBB, 0x22};

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

void handle_display();
void handle_steppers();

extern struct interrupt_vector _SCI_VECTOR;
