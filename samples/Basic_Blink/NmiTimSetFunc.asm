00000000 <NmiTimSetFunc-0x8>:
   0:	00 fe ef 3f             	
   4:	00 00 00 00             	
			4: R_XTENSA_32	.bss

00000008 <NmiTimSetFunc>:
   8:	076c                	movi.n	a7, -32 ; 0xffffffe0
   a:	f60c                	movi.n	a6, 15  ; 0x0000000f
   c:	fffd51               	l32r	a5, 0 <NmiTimSetFunc-0x8>
			c: R_XTENSA_SLOT0_OP	.text.NmiTimSetFunc
   f:	0020c0               	memw
  12:	802542               	l32i	a4, a5, 0x200 ; NMI_INT_ENABLE_REG
  15:	104470               	and	a4, a4, a7    ; a4 &= 0xffffffe0
  18:	204460               	or	a4, a4, a6    ; a4 |= 0x0000000f
  1b:	0020c0               	memw
  1e:	fff931               	l32r	a3, 4 <NmiTimSetFunc-0x4>
			1e: R_XTENSA_SLOT0_OP	.text.NmiTimSetFunc+0x4
  21:	806542               	s32i	a4, a5, 0x200
  24:	0329                	s32i.n	a2, a3, 0
  26:	f00d                	ret.n

