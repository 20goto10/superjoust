#ifndef __STDIO_H
#include <stdio.h>
#endif

typedef char palette[256][3];

void waitvr()
{
asm {
      mov     dx,0x3DA
    }
aWaitVR1:
asm {
		in      al,dx
		test    al,8
		jne     aWaitVR1
    }
aWaitVR2:
asm {
	in      al,dx
	test    al,8
	je      aWaitVR2
    }
}

void oldsetpal(palette pal)
{
  _ES = FP_SEG(pal);
  _DI = FP_OFF(pal);
  _BX = 0;
  _CX = 256;
  _AX = 0x1012;
  geninterrupt(0x10);
}

void setpal(palette pal)
{
 _DS = FP_SEG(pal);
 _SI = FP_OFF(pal);
 asm {
      mov dx,0x3c8
      mov al,0
      mov cx,768
      out dx,al
      inc dx
      rep outsb
     }
}

int loadpal(char *ch, palette pal)
{
  FILE *stream;
  int ret=1,a,b;

  if ((stream = fopen(ch,"rb")) == NULL) ret = 0;
  else
   {
     fread(pal, 1, 768, stream);
     fclose(stream);
     ret = 1;
   }
  return ret;
}

int savepal(char *ch, palette pal)
{
  FILE *stream;
  int ret=1,a,b;

  if ((stream = fopen(ch,"wb")) == NULL) ret = 0;
  else
   {
     fwrite(pal, 1, 768, stream);
     fclose(stream);
     ret = 1;
   }
  return ret;
}

/*
void palrot(int dir, palette pal, int palseg, int paloff)
{
  char tempr,tempg,tempb;

  if (!dir)
  {
    tempr=pal[1][0];
    tempg=pal[1][1];
    tempb=pal[1][2];
    movedata(palseg,paloff+6,palseg,paloff+3,762);
    pal[255][0] = tempr;
    pal[255][1] = tempg;
    pal[255][2] = tempb;
  }
  else
  if (dir == 1)
  {
    tempr=pal[255][0];
    tempg=pal[255][1];
    tempb=pal[255][2];
    movmem((const *) MK_FP(palseg,paloff+3),MK_FP(palseg,paloff+6),762);
    pal[1][0] = tempr;
    pal[1][1] = tempg;
    pal[1][2] = tempb;
  }
}*/