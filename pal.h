#ifndef __STDIO_H
#include <stdio.h>
#endif

typedef char palette[512][3];
int palseg,paloff,offset;

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
  _DX = FP_OFF(pal);
  _BX = 0;
  _CX = 256;
  _AX = 0x1012;
  geninterrupt(0x10);
}

void setpal(int offset)
{
 _DS = palseg;
 _SI = paloff+offset;
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
     for (a=0;a<256;a++)
      for (b=0;b<3;b++)
        {
          fread(&pal[a][b], 1, 1, stream);
          pal[a+256][b] = pal[a][b];
        }
     fclose(stream);
     ret = 1;
   }
   palseg = FP_SEG(pal);
   paloff = FP_OFF(pal);
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

void palify(int dir)
{
  if (dir)
  {
    offset+=3;
    if (offset == 768) offset = 0;
  }
  else
  {
    if (!offset) offset = 768;
    offset-=3;
  }
  waitvr();
  setpal(offset);
}