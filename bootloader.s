start:
	# Set stack pointer to top of external RAM
	lds #0x7FFF

	# Set the HPRIO register to 0xE3
	# This switches to special test mode allowing us access to external
	# peripherals while continuing to use the bootstap interrupt vectors.
	ldaa #0xe3
	staa 0x103C

	# We can increase the baud rate up to 115200 as needed
	# 0x22 = 7200 (default), 0x11 = 19200, 0x00 = 115200
	# ldaa #0x11
	# staa 0x102B

	# I hope to use the 9th bit for framing of data during normal operation.
	# Unfortunately Linux doesn't support this so I'll have to use a different
	# method for framing data during early testing.
	# ldaa #0x10
	# staa 0x102C

receive_program_length:
	# Receive two bytes from the serial data register (SCDR) into registers A and B.
	# Register D will then contain the length of the data to receive.
	bsr waitbyte
	ldaa 0x102F
	bsr waitbyte
	ldab 0x102F
	# Move this to register Y
	xgdy

receive_program_data:
	# Write received bytes to RAM starting at address 0x2000
	ldx #0x2000
receive_loop:
	# Wait for a byte to be received
	bsr waitbyte
	# Load the received byte from SCDR into register A
	ldaa 0x102F
	# Store the byte into RAM at address X
	staa ,X
	# Increment X
	inx
	# Decrement Y
	dey
	# Loop until Y is zero
	bne receive_loop

run_program:
	# Set stack pointer back to the top of external RAM
	lds #0x7FFF
	# Jump to the loaded program at 0x2000
	jmp 0x2000

waitbyte:
	# Function to wait for a byte to be received
	# Push the current value of register A onto the stack
	psha
waitbyteloop:
	# Load the SCI status register into register A
	ldaa 0x102E
	# Check if the RDRF bit is set
	bita #0x20
	# If the RDRF bit is not set, loop
	beq waitbyteloop
	# Pop the value of register A off the stack
	pula
	# Return
	rts
