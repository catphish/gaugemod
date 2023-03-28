#include "program.h"

unsigned short stepper_position = 0;
unsigned short stepper_target = 0;
char display_buffer[26] = "Hello, world!Goodbye world";

// Write a character from the display buffer to the display
void handle_display() {
  static unsigned char display_status = 0;
  switch (display_status) {
    case 0:
      _LCD_CTRL = 0x38;
      display_status = 1;
      break;
    case 1:
      _LCD_CTRL = 0x0C;
      display_status = 2;
      break;
    case 2:
      _LCD_CTRL = 0x01;
      display_status = 3;
      break;
    case 3:
      _LCD_CTRL = 0x80;
      display_status = 4;
      break;
    case 4 ... 16:
      _LCD_DATA = display_buffer[display_status - 4];
      display_status++;
      break;
    case 17:
      _LCD_CTRL = 0x80 + 0x40;
      display_status = 18;
      break;
    case 18 ... 30:
      _LCD_DATA = display_buffer[display_status - 5];
      display_status++;
      break;
    case 31:
      display_status = 3;
      break;
  }
}

// Receive serial data
void sci_handler() {
  _PORTA ^= 0x80;
  // TODO: Load a complete frame into a buffer for processing by the main loop.
  // TODO: Use the 9th data bit for more reliable framing.
  static unsigned char serial_state = 0;
  static unsigned char serial_command;
  static unsigned char serial_data[2];
  if (_SCSR & 0x20) {
    switch (serial_state) {
      case 0:
        // Wait for 0xFF to start a frame
        if (_SCDR == 0xFF) serial_state = 1;
        break;
      case 1:
        // Byte 1 is the command
        serial_command = _SCDR;
        serial_state = 2;
        break;
      case 2:
        // Byte 2 is the first data byte
        serial_data[0] = _SCDR;
        serial_state = 3;
        break;
      case 3:
        // Byte 3 is the second data byte
        serial_data[1] = _SCDR;
        // TODO: implement a checksum here
        switch (serial_command) {
          case 0:
            // Command 0 sets the stepper target
            stepper_target = (serial_data[0] << 8) | serial_data[1];
            break;
          case 1:
            // Command 1 sets a character in the display buffer
            if (serial_data[0] < 26) display_buffer[serial_data[0]] = serial_data[1];
            break;
        }
        serial_state = 0;
        break;
    }
  }
}

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
  _TFLG2 = 0x40;
}

int main() {
  // Clear the BSS segment
  unsigned short *bss = &_bss_start;
  while (bss < &_bss_end) {
    *bss = 0;
    bss++;
  }

  // Install the SCI handler
  _SCI_VECTOR.instruction = 0x7E;
  _SCI_VECTOR.address = (unsigned short)sci_handler;
  // Enable receive interrupt
  _SCCR2 = _SCCR2 | 0x20;
  // Unmask interrupts
  asm("cli");

  // Set bit 3 of PORT A (0x1000)
  // This illuminates the cluster and confirms the program is running
  _PORTA = 0x08;

  // Loop forever
  while (1) {
    // Run display update code every tick
    unsigned char cnt_1 = _TCNTH;
    static unsigned short prev_display_cnt = 0;
    if (cnt_1 != prev_display_cnt) {
      handle_display();
      prev_display_cnt = cnt_1;
    }

    // Run stepper code every 8th tick
    unsigned char cnt_8 = cnt_1 & 0xF8;
    static unsigned short prev_stepper_cnt = 0;
    if (cnt_8 != prev_stepper_cnt) {
      handle_steppers();
      prev_stepper_cnt = cnt_8;
    }
  }
}
