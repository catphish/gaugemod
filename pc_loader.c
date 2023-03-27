#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "/usr/include/asm-generic/ioctls.h"
#include "/usr/include/asm-generic/termbits.h"

int main(int argc, char* argv[]) {
  int fd;
  unsigned char send_ch, recv_ch;

  if (argc != 4) {
    printf("# 68HC11 loader #\n\nPlease specify serial device, loader, and program files.\nFor example:\n    %s /dev/ttyUSB0 loader.bin program.bin\n", argv[0]);
    return -1;
  }

  fd = open(argv[1], O_RDWR);
  if (fd < 1) {
    perror("Error opening serial device");
    return 1;
  }

  struct termios2 tio;
  ioctl(fd, TCGETS2, &tio);
  tio.c_cflag &= ~CBAUD;
  tio.c_cflag |= BOTHER;
  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_ispeed = 7200;
  tio.c_ospeed = 7200;
  int r = ioctl(fd, TCSETS2, &tio);
  if (r == 0) {
    printf("Serial port configured successfully.\n");
  } else {
    perror("Error configuring serial device");
    return 1;
  }

  char loaderbuf[256];
  memset(loaderbuf, 0, 256);
  int fd2 = open(argv[2], O_RDONLY);
  if (fd2 < 1) {
    perror("Error opening bootloader binary");
    return 1;
  }
  read(fd2, loaderbuf, 256);

  send_ch = 255;
  write(fd, &send_ch, 1);
  printf("Uploading bootloader...");
  for (int n = 0; n < 256; n++) {
    write(fd, loaderbuf + n, 1);
  }
  printf(" Done\n");

  usleep(10000);

  char progbuf[24576];
  memset(progbuf, 0, 24576);
  int fd3 = open(argv[3], O_RDONLY);
  if (fd2 < 1) {
    perror("Error opening loader binary");
    return 1;
  }
  int length = read(fd3, progbuf, 24576);
  unsigned char length_h = length >> 8;
  unsigned char length_l = length;
  write(fd, &length_h, 1);
  write(fd, &length_l, 1);

  printf("Uploading program...");
  for (int n = 0; n < length; n++) {
    write(fd, progbuf + n, 1);
  }
  printf(" Done\n");

  return 0;
}
