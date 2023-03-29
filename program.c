#include "program.h"

static unsigned char lcd_col = 0;
static unsigned char lcd_row = 0;

unsigned char serial_data[2][4];
unsigned char serial_buf = 0;
unsigned char serial_pos = 0;
unsigned char serial_buffer_full[2] = {0, 0};

unsigned short stepper_position[2] = {0, 0};
unsigned short stepper_target[2] = {0, 0};

char display_buffer[2][13] = {"Hello, world!", "Goodbye world"};

// Write a character from the display buffer to the display
void handle_display() {
  if (lcd_col) {
    _LCD_DATA = display_buffer[lcd_row][lcd_col - 1];
  } else {
    _LCD_CTRL = (lcd_row ? 0xC0 : 0x80);
  }
  lcd_col++;
  if (lcd_col == 14) {
    lcd_col = 0;
    lcd_row = 1 - lcd_row;
  }
}

// Receive serial data into a double buffer
void handle_sci() {
  // TODO: Use the 9th data bit for better framing.
  // For now, we will just use 0xFF as a frame start byte.

  // Receive one byte of data
  unsigned char scsr = _SCSR;
  unsigned char byte = _SCDR;
  // If the byte is 0xFF, resync position
  if (byte == 0xff) serial_pos = 0;
  // Append the byte to the serial data buffer
  serial_data[serial_buf][serial_pos++] = byte;
  // If we have received 4 bytes, try to swap buffers
  if (serial_pos == 4) {
    // Reset to start of buffer
    serial_pos = 0;
    if (serial_buffer_full[1 - serial_buf] == 0) {
      // If the other buffer is empty, mark this buffer as full
      serial_buffer_full[serial_buf] = 1;
      // ...and prepare to write into the empty buffer
      serial_buf = 1 - serial_buf;
      // else, we will just overwrite this buffer again
    }
  }
}

// Move the stepper motor one step towards the target
void handle_steppers() {
  unsigned char stepper_output = 0;
  unsigned char i;
  for (i = 0; i < 2; i++) {
    if (stepper_position[i] < stepper_target[i]) {
      stepper_position[i]++;
    } else if (stepper_position[i] > stepper_target[i]) {
      stepper_position[i]--;
    }
    unsigned char stepper_step;
    stepper_step = stepper_position[i] % 6;
    stepper_output |= stepper_steps[i][stepper_step];
  }
  // Write the step to the stepper motors
  _STEPPER = stepper_output;
  _TFLG2 = 0x40;
}

void initialize_display() {
  _LCD_CTRL = 0x38;
  while (_LCD_CTRL & 0x80) continue;
  _LCD_CTRL = 0x0C;
  while (_LCD_CTRL & 0x80) continue;
  _LCD_CTRL = 0x01;
}

void initialize_stepper() { _STEPPER = 0x00; }

void initialize_serial() {
  // Install the SCI handler
  _SCI_VECTOR.instruction = 0x7E;
  _SCI_VECTOR.address = (unsigned short)handle_sci;
  // Enable receive and receive interrupt
  _SCCR2 = 0x24;
  // Unmask interrupts
  asm("cli");
}

// Clear the BSS segment
void initialize_memory() {
  unsigned short *bss = &_bss_start;
  while (bss < &_bss_end) {
    *bss = 0;
    bss++;
  }
}

void process_serial_buffer_0() {
  switch (serial_data[0][1]) {
    case 0x01:
      stepper_target[0] = *(unsigned short *)&serial_data[0][2];
      break;
    case 0x02:
      stepper_target[1] = *(unsigned short *)&serial_data[0][2];
      break;
    case 0x03:
      display_buffer[0][serial_data[0][2]] = serial_data[0][3];
      break;
  }
  serial_buffer_full[0] = 0;
}
void process_serial_buffer_1() {
  switch (serial_data[1][1]) {
    case 0x01:
      stepper_target[0] = *(unsigned short *)&serial_data[1][2];
      break;
    case 0x02:
      stepper_target[1] = *(unsigned short *)&serial_data[1][2];
      break;
    case 0x03:
      display_buffer[0][serial_data[1][2]] = serial_data[1][3];
      break;
  }
  serial_buffer_full[1] = 0;
}

int main() {
  // Configure all hardware
  initialize_memory();
  initialize_display();
  initialize_stepper();
  initialize_serial();

  // Set bit 3 of PORT A (0x1000)
  // This illuminates the cluster and confirms the program is running
  _PORTA = 0x08;

  // Loop forever
  while (1) {
    // If the display is ready, update it
    if ((_LCD_CTRL & 0x80) == 0) {
      handle_display();
    }

    // If a serial buffer is full, process it
    if (serial_buffer_full[0]) process_serial_buffer_0();
    if (serial_buffer_full[1]) process_serial_buffer_1();

    // Run stepper code every 8th tick
    static unsigned short prev_stepper_cnt = 0;
    unsigned char cnt = _TCNTH;
    char ticks = cnt - prev_stepper_cnt;
    if (ticks > 10) {
      handle_steppers();
      prev_stepper_cnt = cnt;
    }
  }
}
