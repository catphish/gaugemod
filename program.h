const unsigned char stepper_steps[6] = {0x90, 0x80, 0xE0, 0x60, 0x70, 0x10};

struct interrupt_table_entry {
  unsigned char instruction;
  unsigned short address;
};

extern volatile unsigned char _PORTA;
extern volatile unsigned char _SCDR;
extern volatile unsigned char _SCSR;

extern volatile unsigned char _STEPPER;
extern volatile unsigned char _LCD_CTRL;
extern volatile unsigned char _LCD_DATA;

extern volatile struct interrupt_table_entry _ISR_TABLE_RTI;
extern volatile struct interrupt_table_entry _ISR_TABLE_SWI;

int main(void) __attribute__((section(".main")));
void __attribute__((interrupt)) isr_rti();
void __attribute__((interrupt)) isr_swi();

void isr_setup();

void handle_display();
void handle_steppers();
