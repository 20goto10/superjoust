#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __DOS_H
#include <dos.h>
#endif

#ifndef __MEM_H
#include <mem.h>
#endif

#define CRTC_ADDR	0x3d4	/* Base port of the CRT Controller (color) */
#define SEQU_ADDR	0x3c4	/* Base port of the Sequencer */
#define GRAC_ADDR	0x3ce	/* Base port of the Graphics Controller */

typedef unsigned char unchar;
unchar *vga = (unchar*) MK_FP(0xA000,0);
unsigned int height,width,widthBytes,actStart,visStart;

void setActiveStart(unsigned offset)
	{
	actStart = offset;
	}

void setVisibleStart(unsigned offset)
	{
	visStart = offset;
	outport(CRTC_ADDR, 0x0C);		/* set high byte */
	outport(CRTC_ADDR+1, visStart >> 8);
	outport(CRTC_ADDR, 0x0D);		/* set low byte */
	outport(CRTC_ADDR+1, visStart & 0xff);
	}

void setActivePage(int page)
	{
	setActiveStart(page * 19200);
	}

void setVisiblePage(int page)
	{
	setVisibleStart(page * 19200);
	}

void initmode()
{
  _AX = 0x13;
  geninterrupt(0x10);
  outport(SEQU_ADDR, 0x0604);
  outport(CRTC_ADDR, 0xE317);
  outport(CRTC_ADDR, 0x0014);

  outport(SEQU_ADDR, 0x0F02);
  memset(vga+1, 0, 0xffff);
  vga[0] = 0;
  width   = 320;
  height  = 240;
  widthBytes = width >> 2;
  actStart = visStart = 0;
  setActivePage(0);
  setVisiblePage(0);
  outportb(0x3C2, 0xE3);
  outport(0x3D4, 0x2C11);		/* turn off write protect */
  outport(0x3D4, 0x0D06);		/* vertical total */
  outport(0x3D4, 0x3E07);		/* overflow register */
  outport(0x3D4, 0xEA10);		/* vertical retrace start */
  outport(0x3D4, 0xAC11);		/* vertical retrace end AND wr.prot */
  outport(0x3D4, 0xDF12);		/* vertical display enable end */
  outport(0x3D4, 0xE715);		/* start vertical blanking */
  outport(0x3D4, 0x0616);		/* end vertical blanking */ 
}

void clearPage(int page, char color)
{
  int pageme;

  _ES = 0xA000;
  pageme = page*19200;
  asm {
    mov ax,0x0F02
    mov dx,0x3C4
    out dx,ax
    mov cx,9600
    mov di,pageme
    mov ah,color
    mov al,color
    rep stosw
  }
}

void normmode()
{
  _AX = 3;
  geninterrupt(0x10);
}

unsigned char getpixel(int x, int y)
	{
	  outport(GRAC_ADDR, 0x04);
	  outport(GRAC_ADDR+1, x & 3);
	  return vga[(unsigned)(widthBytes * y) + (x >> 2) + actStart];
	}

unsigned char getpixel2(int x, int y)
// gets pixel from 3rd page, always
	{
	  outport(GRAC_ADDR, 0x04);
	  outport(GRAC_ADDR+1, x & 3);
	  return vga[(unsigned)(widthBytes * y) + (x >> 2) + 38400];
	}

void putpixel(int x, int y, unsigned char color)
{
  asm {
       mov al,0x02
       mov bl,0x01
       xor cl,cl
       mov cx,x
       and cx,0x0003
       shl bl,cl
       mov ah,bl
       mov dx,0x3c4
       out dx,ax

       mov ax,widthBytes
       mov bx,y
       imul bx
       mov bx,x
       xor cl,cl
       mov cl,2
       shr bx,cl
       add ax,bx
       mov bx,actStart
       add ax,bx
       mov di,ax
       mov ax,0xA000
       mov es,ax
       mov al,color
       stosb
      }
/*   outportb(0x3c4, 0x02);
   outportb(0x3c5, 0x01 << (x & 3));
   vga[(unsigned)(widthBytes * y) + (x >> 2) + actStart] = color; */
}

void putpixel2(int offset, int plane, unsigned char color)
{
   outportb(0x3c4, 0x02);
   outportb(0x3c5, 0x01 << plane); // offset = (widthbytes * y + x)
   vga[(unsigned) offset + actStart] = color;
}

void setScreenWidth(int SCwidth)
{
  widthBytes = SCwidth >> 2;
  width = SCwidth;
  outportb(CRTC_ADDR,0x13);
  outportb(CRTC_ADDR+1,SCwidth>>3);
}

