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

#define CRTC_ADDR	0x3d4
#define SEQU_ADDR	0x3c4
#define GRAC_ADDR	0x3ce
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

void normmode()
{
  _AX = 3;
  geninterrupt(0x10);
}
