extern volatile unsigned char _io_ports[];

int main() {
  // Set bit 3 of PORT A (0x1000)
  // This illuminates the cluster and confirms success
  _io_ports[0] = 0x08;
  // Loop forever
  while(1);
}
