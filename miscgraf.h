void box(int x1,int y1,int x2,int y2,int color)
{
 int x,y;

 for (x=x1;x<=x2;x++) putpixel(x,y1,color);
 for (y=y1;y<=y2;y++) putpixel(x1,y,color);
 for (x=x1;x<=x2;x++) putpixel(x,y2,color);
 for (y=y1;y<=y2;y++) putpixel(x2,y,color);
}

void bar(int x1,int y1,int x2,int y2,int color)
{
 int x=0,y=0;

 outport(0x3c4,0x0F02);
 for (x=x1>>2;x<(x2>>2);x++)
  for (y=y1;y<y2;y++)
  vga[actStart+x+y*widthBytes] = color;
}

void goodbar(int x1,int y1,int x2,int y2,int color)
{
 int x=0,y=0;

 for (x=x1;x<=x2;x++)
  for (y=y1;y<=y2;y++)
    putpixel(x,y,color);
}

void superputpixel(int x,int y,int color)
{
  int xtemp;

  xtemp = x>>2;
  outportb(0x3C4,2);
  outportb(0x3c5,1 << (x & 3));
  vga[xtemp + y*80] = color;
  vga[19200 + xtemp + y*80] = color;
  vga[38400 + xtemp + y*80] = color;
}

void supergoodbar(int x1,int y1,int x2,int y2,int color)
// draws bar on 3 pages at once
{
 int x=0,y=0,xtemp;

 for (x=x1;x<=x2;x++)
 {
  xtemp = x>>2;
  outportb(0x3C4,2);
  outportb(0x3c5,1 << (x & 3));
  for (y=y1;y<=y2;y++)
   {
     vga[xtemp + y*80] = color;
     vga[19200 + xtemp + y*80] = color;
     vga[38400 + xtemp + y*80] = color;
   }
 }
}

void plot4 (int xOrigin, int yOrigin, int xOffset, int yOffset, int color)
{
  putpixel(xOrigin+xOffset, yOrigin+yOffset,color);
  putpixel(xOrigin+xOffset, yOrigin-yOffset,color);
  putpixel(xOrigin-xOffset, yOrigin+yOffset,color);
  putpixel(xOrigin-xOffset, yOrigin-yOffset,color);
}

void plot8(int xOrigin, int yOrigin, int xOffset, int yOffset, int color)
{
  plot4(xOrigin, yOrigin,xOffset,yOffset,color);
  plot4(xOrigin, yOrigin,yOffset,xOffset,color);
}

void circle(int xOrigin,int yOrigin, int radius, int color)
{
  int x = 0;
  int y;
  int error = 0;

  y = radius;
  while (x<=y)
  {
    plot8(xOrigin, yOrigin,x,y,color);
    error += (x << 1) + 1;
    x+=1;
    if (error >= y)
     {
       error -= (y << 1) - 1;
       y-=1;
     }
  }
}

void ellipse(int xOrigin,int yOrigin, int a, int b, int color)
{
  long int aSquared;
  long int bSquared;
  long int twoASquared;
  long int twoBSquared;
  long int twoXTimesBSquared,twoYTimesASquared;
  long int error,x,y;

//  if ((a != 0) || (b != 0))
  {
  aSquared=a*a;
  bSquared=b*b;
  twoASquared = 2*aSquared;
  twoBSquared = 2*bSquared;
  {
    x = 0;
    y = b;
    twoXTimesBSquared = 0;
    twoYTimesASquared = y*twoASquared;
    error = -y*aSquared;
    while (twoXTimesBSquared <= twoYTimesASquared)
    {
      plot4(xOrigin, yOrigin,x,y,color);
      x++;
      twoXTimesBSquared += twoBSquared;
      error += twoXTimesBSquared - bSquared;
      if (error >= 0)
      {
	y--;
	twoYTimesASquared -= twoASquared;
	error -= twoYTimesASquared;
      }
    }
  }
  {
    x = a;
    y = 0;
    twoXTimesBSquared = x*twoBSquared;
    twoYTimesASquared = 0;
    error = -x*bSquared;
    while (twoXTimesBSquared > twoYTimesASquared)
    {
      plot4(xOrigin,yOrigin,x,y,color);
      y++;
      twoYTimesASquared += twoASquared;
      error+=twoYTimesASquared - aSquared;
      if (error >= 0)
      {
       x--;
       twoXTimesBSquared -= twoBSquared;
       error -= twoXTimesBSquared;
      }
    }
  }
  }
}


// works fine in 0x13!
void thirtline(int x1,int y1,int x2,int y2,int color)
{
  int scr_off=actStart,error_term,y_unit,xdiff,x_unit,ydiff,AUTO_SIZE;

  AUTO_SIZE = sizeof(int);
  error_term = AUTO_SIZE;
  _ES = 0xA000;

  asm {
     mov ax,y1
     mov dx,320
     mul dx
     add ax,x1
     add ax,scr_off
     mov bx,ax
  }
  init_line:
  asm {
     mov dx,color
     mov error_term,0
     mov ax,y2
     sub ax,y1
     jns ypos
     mov y_unit,-320
     neg ax
     mov ydiff,ax
     jmp next
  }
  ypos:
  asm {
    mov y_unit,320
    mov ydiff,ax
  }
  next:
  asm {
    mov ax,x2
    sub ax,x1
    jns xpos
    mov x_unit,-1
    neg ax
    mov xdiff,ax
    jmp next2
  }
  xpos:
  asm {
    mov x_unit,1
    mov xdiff,ax
  }
  next2:
  asm {
    cmp ax,ydiff
    jc  yline
    jmp xline
  }
  xline:
  asm {
    mov cx,xdiff
    inc cx
  }
  xline1:
  asm {
    mov es:[bx],dl
    add bx,x_unit
    mov ax,error_term
    add ax,ydiff
    mov error_term,ax
    sub ax,xdiff
    jc xline2
    mov error_term,ax
    add bx,y_unit
  }
  xline2:
  asm {
    loop xline1
    jmp linedone
  }
  yline:
  asm {
    mov cx,ydiff
    inc cx
  }
  yline1:
  asm {
    mov es:[bx],dl
    add bx,y_unit
    mov ax,error_term
    add ax,xdiff
    mov error_term,ax
    sub ax,ydiff
    jc  yline2
    mov error_term,ax
    add bx,x_unit
  }
  yline2:
  asm loop yline1
  linedone:
}

#define MAP_MASK 0x2

#define modexaddr {                    \
		  asm mov	cl,bl; \
		  asm push      dx;    \
		  asm mov	dx,widthBytes; \
		  asm mul	dx; \
		  asm pop	dx; \
		  asm shr	bx,2; \
		  asm add	bx,ax; \
		  asm add       bx,actStart; \
		  asm and	cl,3; }

void line(int x1,int y1, int x2, int y2, int color)
{
  int vertincr,incr1,incr2,routine;

asm {
	mov	ax,0a000h
	mov	es,ax

	mov	dx,SEQU_ADDR
//  setup for plane mask access

// check for vertical line

	mov	si,widthBytes
	mov	cx,x2
	sub	cx,x1
	}
	if (!_CX) goto	VertLine;

// ; force x1 < x2

	asm {
	jns	L01
	neg	cx
	mov	bx,x2
	xchg	bx,x1
	mov	x2,bx
	mov	bx,y2
	xchg	bx,y1
	mov	y2,bx

// ; calc dy = abs(y2 - y1)
	}

L01:
      asm {
	mov	bx,y2
	sub	bx,y1
	jnz	short skip
	jmp     HorizLine
      }
skip:
       asm {
	jns	L03

	neg	bx
	neg	si

// ; select appropriate routine for slope of line
	}
L03:
       asm {
	mov	vertincr,si
//	mov	routine,offset LoSlopeLine
	}
	routine = 1;
	asm {
	cmp	bx,cx
	jle	L04
//	mov	routine,offset HiSlopeLine
	}
	routine = 0;
	asm xchg	bx,cx

// ; calc initial decision variable and increments

L04:
	asm {
	shl	bx,1
	mov	incr1,bx
	sub	bx,cx
	mov	si,bx
	sub	bx,cx
	mov	incr2,bx

// ; calc first pixel address

	push	cx
	mov	ax,y1
	mov	bx,x1
	}

	modexaddr

	asm {
	mov	di,bx
	mov	al,1
	shl	al,cl
	mov	ah,al		// ; duplicate nybble
	shl	al,4
	add	ah,al
	mov	bl,ah
	pop	cx
	inc	cx
	}
	if (routine) asm jmp LoSlopeLine;
	else asm jmp HiSlopeLine;

// ; routine for verticle lines

VertLine:
	asm {
	mov	ax,y1
	mov	bx,y2
	mov	cx,bx
	sub	cx,ax
	jge	L31
	neg	cx
	mov	ax,bx
	}
L31:
	asm {
	inc	cx
	mov	bx,x1
	push	cx
	}
	modexaddr
	asm {
	mov	ah,1
	shl	ah,cl
	mov	al,MAP_MASK
	out	dx,ax
	pop	cx
	mov	ax, word ptr color
	}
// ; draw the line

L32:
	asm {
	mov	es:[bx],al
	add	bx,si
	loop	L32
	jmp	Lexit
	}
// ; routine for horizontal line

HorizLine:
	asm {
	push	ds

	mov	ax,y1
	mov	bx,x1
	}

	modexaddr

	asm {
	mov	di,bx  //   ; set dl = first byte mask
	mov	dl,00fh
	shl	dl,cl

	mov	cx,x2  // ; set dh = last byte mask
	and	cl,3
	mov	dh,00eh
	shl	dh,cl
	not	dh

// ; determine byte offset of first and last pixel in line

	mov	ax,x2
	mov	bx,x1

	shr	ax,2     // ; set ax = last byte column
	shr	bx,2    // ; set bx = first byte column
	mov	cx,ax    // ; cx = ax - bx
	sub	cx,bx

	mov	ax,dx    // ; mov end byte masks to ax
	mov	dx,SEQU_ADDR // ; setup dx for VGA outs
	mov     bx, color

// ; set pixels in leftmost byte of line

	or	cx,cx      //; is start and end pt in same byte
	jnz	L42        //; no !
	and	ah,al      //; combine start and end masks
	jmp	short L44
       }

L42:
    asm {
	push    ax
	mov     ah,al
	mov     al,MAP_MASK
	out     dx,ax
	mov	al,bl
	stosb
	dec	cx
    }
// ; draw remainder of the line

L43:
	asm {
	mov	ah,0Fh
	mov	al,MAP_MASK
	out	dx,ax
	mov	al,bl
	rep	stosb
	pop     ax
	}
// ; set pixels in rightmost byte of line

L44:
	asm {
	mov	al,MAP_MASK
	out	dx, ax
	mov     byte ptr es:[di],bl
	pop	ds
	jmp	short Lexit
	}

// ; routine for dy >= dx (slope <= 1)

LoSlopeLine:
	asm {
	mov	al,MAP_MASK
	mov	bh,byte ptr color
	}
L10:
	asm mov	ah,bl

L11:
	asm {
	or	ah,bl
	rol	bl,1
	jc	L14

// ; bit mask not shifted out

	or	si,si
	jns	L12
	add	si,incr1
	loop	L11

	out	dx,ax
	mov	es:[di],bh
	jmp	short Lexit
	}
L12:
	asm {
	add	si,incr2
	out	dx,ax
	mov	es:[di],bh
	add	di,vertincr
	loop	L10
	jmp	short Lexit

// ; bit mask shifted out
	}
L14:
	asm {
	out	dx,ax
	mov	es:[di],bh
	inc	di
	or	si,si
	jns	L15
	add	si,incr1
	loop	L10
	jmp	short Lexit
	}
L15:
	asm {
	add	si,incr2
	add	di,vertincr
	loop	L10
	jmp	short Lexit

// ; routine for dy > dx (slope > 1)
	}

HiSlopeLine:
	asm {
	mov	bx,vertincr
	mov	al,MAP_MASK
	}
L21:    asm {
	out	dx,ax
	push	ax
	mov	ax,color
	mov	es:[di],al
	pop	ax
	add	di,bx
	}
L22:
	asm {
	or	si,si
	jns	L23

	add	si,incr1
	loop	L21
	jmp	short Lexit
	}
L23:
	asm {
	add	si,incr2
	rol	ah,1
	adc	di,0
	}
lx21:
	asm	loop	L21

// ; return to caller

Lexit:
}
