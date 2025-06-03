
example.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	48 8d 3d 38 00 00 00 	lea    0x38(%rip),%rdi        # 0x3f
   7:	c7 07 35 39 62 39    	movl   $0x39623935,(%rdi)
   d:	c7 47 04 39 37 66 61 	movl   $0x61663739,0x4(%rdi)
  14:	c7 47 08 00 00 00 00 	movl   $0x0,0x8(%rdi)
  1b:	c3                   	retq   
