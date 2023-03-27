#include "program.h"

unsigned short stepper_position = 0;
unsigned short stepper_target = 0;
char display_buffer[26] = "QWERT, wotld!Goodbwe world";

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
void handle_serial() {
  static unsigned char serial_state = 0;
  static unsigned char serial_command;
  static unsigned char serial_data[2];
  if (_SCSR & 0x20) {
    switch (serial_state) {
      case 0:
        serial_command = _SCDR;
        serial_state = 1;
        break;
      case 1:
        serial_data[0] = _SCDR;
        serial_state = 2;
        break;
      case 2:
        serial_data[1] = _SCDR;
        switch (serial_command) {
          case 0:
            stepper_target = (serial_data[0] << 8) | serial_data[1];
            break;
          case 1:
            display_buffer[serial_data[0]] = serial_data[1];
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

// Functio called at fixed intervals
void interval() {
  handle_display();
  handle_steppers();
}

int main() {
  // Clear the BSS segment
  unsigned short *bss = &_bss_start;
  while (bss < &_bss_end) {
    *bss = 0;
    bss++;
  }

  // Set bit 3 of PORT A (0x1000)
  // This illuminates the cluster and confirms success
  _PORTA = 0x08;

  // Loop forever
  while (1) {
    handle_serial();
    unsigned int cnt = _TCNTH >> 3;
    static unsigned short prev_stepper_cnt = 0;
    if (cnt != prev_stepper_cnt) {
      handle_steppers();
      prev_stepper_cnt = cnt;
    }
    cnt = _TCNTH >> 3;
    static unsigned short prev_display_cnt = 0;
    if (cnt != prev_display_cnt) {
      handle_display();
      prev_display_cnt = cnt;
    }
  }
}
