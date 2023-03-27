#include "program.h"

unsigned short stepper_position = 0;
unsigned short stepper_target = 0;
char display_buffer[8];

// Write a character from the display buffer to the display
void handle_display() {}

// Move the stepper motor one step towards the target
void handle_steppers() {
  if (stepper_position < stepper_target) {
    stepper_position++;
  } else if (stepper_position > stepper_target) {
    stepper_position--;
  }
  unsigned char stepper_step = stepper_position % 6;
  // Write the step to the stepper motor
  _STEPPER = stepper_steps[stepper_step];
}

// ISR for UART peripheral
void isr_swi() {
  if (_SCSR & 0x20) {
    // Read the character
    char c = _SCDR;
    stepper_target = c;
  }
}

// ISR called at fixed intervals
void isr_rti() {
  handle_display();
  handle_steppers();
}

void isr_setup() {
  _ISR_TABLE_RTI.instruction = 0x7E;
  _ISR_TABLE_RTI.address = (unsigned short)isr_rti;
  _ISR_TABLE_SWI.instruction = 0x7E;
  _ISR_TABLE_SWI.address = (unsigned short)isr_swi;
}

int main() {
  // Set bit 3 of PORT A (0x1000)
  // This illuminates the cluster and confirms success
  _PORTA = 0x08;
  // Loop forever
  while (1) continue;
}
