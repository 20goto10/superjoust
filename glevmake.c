// LEVEL DESIGNER [ grapical edition] for joust
// bits 1,2 : 0 = off, 1 == horiz, 2 == vert, 3 == N/A
// bit  3   : guard on top/left side
// bit  4   : guard on bottom/right side
// bit  5   : bother drawing the thing at all

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "palhs.h"
#include "modexhs.h"
#pragma inline

#define MAIN platforms[platptr].snafu
#define HEADING platforms[platptr].orient
#define ORIENT (platforms[MAIN].type & 3)
#define TYPE (platforms[MAIN].type)
int playptr=0;
int textptr=0;
#define MAXTEXTS 25
char numtexts=0;
char textd[MAXTEXTS]={0,0,0,0,0};
char textsc[MAXTEXTS]={44,44,44,44,44};
char texts[MAXTEXTS][52];
char texton[MAXTEXTS];
int textsx[MAXTEXTS],textsy[MAXTEXTS];
int platptr=0;
int end=0;
unsigned gameturn;
#define widthBytes 80
char filename[80];
int vara,varb,varc;
int incr=1,effectcount=0;

char question=0;
#define MAXSPRITEX 16
#define MAXSPRITEY 16
#define STRMAXLEN 52
#define STRMAXLEO 64
#define CR 13
#define BS 8
#define UPPRESS 5
#define GROUNDLEVEL 184
#define ROOFLEVEL 8
#define NUMMOUNTS 10
#define RIPICON 10
#define EXMOUNTS 11
palette pal;
int gamedelay=0,dinc=1,autowinner=0,buttcount,redo=0;
char backcolor,uho=1,curpage=0,waitforvr=0;
#define FONTX 5
#define FONTY 6
#define SHOTSPEED 16
char fontfore,hilite = 9,hiliteoff = 14,fontback=0,
     fontbackon=0,specon = 1,shownum = 0,platson = 1,keypress,keypress2=1;
int startx1,starty1,startx2,starty2,startx3,starty3,startx4,starty4;
int numplayers = 5,gameplayers = 3,gameover = 0,oldgameover=0;
#define REALWIDTH 320
#define WRAPWIDTH 304
int gravity = 1;

typedef char spritedata_t[MAXSPRITEX*MAXSPRITEY];
typedef struct sprite_t {
		  int n;
		  int x,y;
		} spritetype;
#define GOALSPRITE 2
spritedata_t spritedata[105];
spritetype sprite[16];

#define NORML 0
#define FIRE 1
#define FORCEFIELD 2
#define SHOOTABLE 3
#define CONVEYOR 4
#define INVISIBLE 5
#define BUTT 6
#define UNPASS 7

typedef char fontletter[6];
typedef fontletter fontl[256];

#define BLOWSPRITE 81
#define UNHARMSPRITE 79
#define SPLATSPRITE 86
#define OFFSPRITE 85
#define PLATSPRITE 73
#define MINESPRITE 77
#define LIGHTSPRITE 75
#define CONFSPRITE 83
#define FIRESPRITE 88
#define FORCEUP 92
#define FORCELEFT 94
#define FORCECORNER 96
#define CONVLEFT 97
#define CONVMID 99
#define CONVRIGHT 101
#define BUTTON 103
#define RIDERSPRITE 1

#define clearkeybuf(); *(int far *) MK_FP(0x40,0x1A) = *(int far *) MK_FP(0x40,0x1C);
#define odd(num) (num & 1)

unsigned int basesproff,basesprseg;
typedef char far stringset[20];
fontl font;
#define MAXLEVELS 100
int numlevels=255,curlevel;
char levelnames[MAXLEVELS][20];
int goalx,goaly;
char goal;
char levname[50],story[4][50],goalsprite[16],halfgrav=0;

char effectranges[8]  = { 7,16,16, 7, 7, 7, 5, 7};
char effectrangesy[8] = { 9, 9,16, 9, 9, 9, 5, 9};
char widths[8]	      = {16,16,16,16,16,16, 5,16};

typedef struct playerjunk {
  char dir;  // left = 0, right = 1
  char num;
  char playeron;
  char menuon;
  char compu;
  char plat;
  char wasonplat;
  int stime;
  int x,y;
  int xveloc,yveloc;
  int heightaim,rundir;
  char leftkey,rightkey,upkey;
  char speckey,nspeckey;
  char nleftkey,nrightkey,nupkey;
  char lkey,rkey,ukey,skey,upkeyon;
  char birdtype,specnum,wins;
  char stunned,invis,unharm,shot,mine,armored,curshot,mineon[10],isgoal;
  int shotdir[10];
  int minex[10],miney[10];
} player;
player players[5];

char rounds,computerplayers,livingplayers;
#define MAXMINES 9
char masses[EXMOUNTS]={3,3,4,2,3,3,2,5,2,2,0};
stringset animals[EXMOUNTS] = {"KILLER PARROT   ","PEGASUS         ","WINGED BUFFALO  ","MOTH            ","BANANA WARRIOR  ","CATBIRD         ","BIKE CONTRAPTION","PURPLE DINOSAUR ","FLOATING FISH   ","CLASSIC OSTRICH ","OFF             "};
char maxspeedx[EXMOUNTS]={12,14,12,12, 8,11,12, 8,13,10,0};
char maxspeedy[EXMOUNTS]={12, 8, 8,16,11, 8, 9, 8,12,12,0};
char accelx[EXMOUNTS] =  { 2, 3, 1, 2, 2, 3, 4, 1 ,3, 3,0};
char accely[EXMOUNTS] =  { 4, 2, 3, 5, 2, 3 ,2, 3, 4, 3,0};
char spechigh = 9;
stringset abilities[10] ={"NONE          ","STUNNING SQUAWK","LIGHTNING ARMOR","DROPPING BELLOW","TRANSMOGRIFY   ","INVISIBILITY   ","UNHARMABILITY  ","RANDOM TELEPORT","SHOOT BULLET   ","DROP MINE      "};
char specrate[10]={0,60,60,5,40,10,50,10,20,30};
char minenum[10]={'0','1','2','3','4','5','6','7','8','9'};

typedef struct platinfo_t {
  int startx,starty,length,orient,snafu;
  int xveloc,yveloc;
  int xlim1,xlim2,ylim1,ylim2;
  char type,temptype,effecttype,fieldtype,active;
  int oncycles,offcycles,count,numcycles;
  signed int cycletype;
  char extraeffects[20];
} platinfo;
platinfo platforms[150];
char platformnum = 0;

void putpixel(int x, int y, unsigned char color) {
	  outportb(0x3c4, 0x02);
	  outportb(0x3c5, 0x01 << (x & 3));
	  vga[(unsigned)(widthBytes * y) + (x >> 2) + actStart] = color;
	}

void updatepal() {
  _ES = FP_SEG(pal);
  _DX = FP_OFF(pal);
  _BX = 0;
  _CX = 256;
  _AH = 0x10;
  _AL = 0x12;
  geninterrupt(0x10);
}

void loadfont() {
   FILE *stream;

   if ((stream = fopen("small.fnt","rb")) == NULL) printf("Error.\n");
      else fread(&font, 1, 1536, stream);
   fclose(stream);
}

void box(int x1,int y1,int x2,int y2,int color) {
 int x;

 for (x=x1;x<=x2;x++) putpixel(x,y1,color);
 for (x=y1;x<=y2;x++) putpixel(x1,x,color);
 for (x=x1;x<=x2;x++) putpixel(x,y2,color);
 for (x=y1;x<=y2;x++) putpixel(x2,x,color);
}

void drawfont(int letter,int xoff,int yoff) {
 int x,y;
 char color;

 for (y=0;y<FONTY;y++) {
   color = font[letter][y];
   for (x=0;x<=(FONTX-1);x++)
    if (odd(color >> (FONTX-(1+x)))) putpixel(xoff+x,yoff+y,fontfore);
    else if (fontbackon) putpixel(xoff+x,yoff+y,fontback);
  }
}

void fixmode()
{
 initmode();
 fontback = 0;
 fontbackon = 1;
 updatepal();
}

void writetextxy2(char *string,int xoff,int yoff) {
 int x,len,p=0;

 strupr(string);
 len = strlen(string);
 if (len > STRMAXLEO) len = STRMAXLEO;
 for(x=0;x<=len;x++)  {
  if (string[x] == '@') if (fontfore == hilite) fontfore = hiliteoff;
			else fontfore = hilite;
  else
    drawfont((int) (string[x]),xoff+(p++*(FONTX)),yoff);
 }
}

void writetextxy(char *string,int xoff,int yoff) {
 int x,len,p=0;

 strupr(string);
 len = strlen(string);
 if (len > STRMAXLEN) len = STRMAXLEN;
 for(x=0;x<=len;x++) {
  if (string[x] == '@') if (fontfore == hilite) fontfore = hiliteoff;
			else fontfore = hilite;
  else
    drawfont((int) (string[x]),xoff+(p++*(FONTX+1)),yoff);
 }
}

void loadsprite(char *ch,int num) {
   FILE *stream;
   int a;

   if ((stream = fopen(ch,"rb")) == NULL) printf("Error.\n");
   else
    for (a=0;a<MAXSPRITEX*MAXSPRITEY;a++)
      fread(&spritedata[num][((a&3)<<6) + ((a-(a & 3))>>2)],sizeof(char),1,
	stream);
   fclose(stream);
}

void readin(char *foo)
{
  int x=0;
  char t;

  do
  {
    t = getch();
    if (t == BS)
      {
	if (x) x--;
	foo[x] = 0;
	x--;
      }
    else foo[x] = t;
    x++;
    printf("%c",t);
  }
  while ((t != CR) && (x < 80));
  foo[x-1] = NULL;
  printf("\n");
}


void drawnorm2(int spritenum,int xoff,int yoff) {
 int y,plane,trueplane,tedi,tdi,laps;

 if (xoff >= WRAPWIDTH) {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = 4+(xoff>>2);
  tedi = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  tdi = tedi;
  for (y=0;y<MAXSPRITEY;y++) {
  asm mov cx,4
  foo:;
  _DI = (laps-_CX)%80 + tdi;
  asm {
	lodsb;
	or al,al;
	jz skipme;
	stosb;
	loop foo;
	jmp exittor;
      }
  skipme:;
	asm loop foo;
  exittor:;
  tdi+=widthBytes;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
else {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = (xoff>>2);
  _BX = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX+laps;
  for (y=0;y<MAXSPRITEY;y++) {
  asm mov cx,4
  flofoo:;
  asm {
	lodsb;
	or al,al;
	jz askipme;
	stosb;
	loop flofoo;
	jmp bexittor;
      }
  askipme:;
	asm inc di;
	asm loop flofoo;
  bexittor:;
  _DI += 76;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
}

void drawnorm(int spritenum) {
  drawnorm2(sprite[spritenum].n,sprite[spritenum].x,sprite[spritenum].y);
}


void drawnorm3(int spritenum,int xoff,int yoff,int shorten) {
 int y,plane,trueplane,tdi,tedi,origsi,laps;

  if (xoff >= WRAPWIDTH) {
  _ES = 0xA000;
  _DS = basesprseg;
  origsi = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = 4+(xoff>>2);
  tedi = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  tdi = tedi;
  _SI = origsi+(plane<<6);
  for (y=0;y<shorten;y++) {
  asm mov cx,4
  foo:;
  _DI = (laps-_CX)%80 + tdi;
  asm {
	lodsb;
	or al,al;
	jz skipme;
	stosb;
	loop foo;
	jmp exittor;
      }
  skipme:;
	asm loop foo;
  exittor:;
  tdi+=widthBytes;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
else {
   _ES = 0xA000;
  _DS = basesprseg;
  origsi = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = (xoff>>2);
  _BX = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX+laps;
  _SI = origsi + (plane<<6);
  for (y=0;y<shorten;y++) {
  asm mov cx,4
  flofoo:;
  asm {
	lodsb;
	or al,al;
	jz askipme;
	stosb;
	loop flofoo;
	jmp bexittor;
      }
  askipme:;
	asm inc di;
	asm loop flofoo;
  bexittor:;
  _DI += 76;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
}

void drawback(int spritenum) {
 int y,plane,trueplane,laps,tedi,tdi;

 if (sprite[spritenum].x >= WRAPWIDTH) {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI =	basesproff + (sprite[spritenum].n<<8);
  trueplane = sprite[spritenum].x & 3;
  laps = sprite[spritenum].x >> 2;
  tedi = actStart + widthBytes*sprite[spritenum].y;
  for(plane=3;plane>=0;plane--) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  tdi = tedi;
  for (y=0;y<MAXSPRITEY;y++) {
  asm mov cx,4
  foo:;
  _DI = (laps + _CX)%80 + tdi;
  asm {
	lodsb;
	or al,al;
	jz skipme;
	stosb;
	loop foo;
	jmp exittor;
      }
  skipme:;
	asm loop foo;
  exittor:;
  tdi+=widthBytes;
  }
  trueplane--;
  if (trueplane < 0) { trueplane = 3; laps--; }
 }
 }
 else {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI = basesproff + (sprite[spritenum].n<<8);
  laps = 0;
  trueplane = sprite[spritenum].x & 3;
  tdi = actStart + widthBytes*sprite[spritenum].y + (sprite[spritenum].x >> 2) + 4;
  for(plane=3;plane>=0;plane--) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = tdi-laps;
  for (y=0;y<MAXSPRITEX;y++) {
  asm mov cx,4
  flofoo:;
  asm {
	lodsb;
	or al,al;
	jz askipme;
	stosb;
	sub di,2;
	loop flofoo;
	jmp bexittor;
      }
  askipme:;
	asm dec di;
	asm loop flofoo;
  bexittor:;
  _DI += 84;
  }
  trueplane--;
  if (trueplane < 0) { trueplane = 3; laps = 1; }
 }
 }
}

void drawtile(int spritenum,int xoff,int yoff) {
 int y,plane,trueplane,tedi,tdi,laps;

  if (xoff >= WRAPWIDTH) {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = xoff>>2;
  tedi = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  tdi = tedi;
  for (y=0;y<MAXSPRITEY;y++) {
  asm mov cx,4
  foo:;
  _DI = laps-_CX + tdi;
  asm {
        movsb
        loop foo;
      }
  tdi+=widthBytes;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
 else {
  _ES = 0xA000;
  _DS = basesprseg;
  _SI = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = (xoff>>2);
  _BX = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX+laps;
  for (y=0;y<MAXSPRITEY;y++) {
  asm mov cx,4
  asm rep movsb
  _DI += 76;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
}

void drawtile2(int spritenum,int xoff,int yoff,int cutoff) {
 int y,plane,trueplane,tedi,tdi,laps,origsi;

  if (xoff%320 >= WRAPWIDTH) {
  _ES = 0xA000;
  _DS = basesprseg;
  origsi = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = 4+(xoff>>2);
  tedi = actStart + widthBytes*yoff;

  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  tdi = tedi;
  _SI = origsi + (plane<<6);
  for (y=0;y<cutoff;y++) {
  asm mov cx,4
  foo:;
  _DI = (laps-_CX)%80 + tdi;
  asm movsb
  asm loop foo
  tdi+=widthBytes;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
 else {
  _ES = 0xA000;
  _DS = basesprseg;
  origsi = basesproff+(spritenum<<8);
  trueplane=(xoff & 3);
  laps = ((xoff%320)>>2);
  _BX = actStart + widthBytes*yoff;
  for(plane=0;plane<4;plane++)  {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX+laps;
  _SI = origsi+ (plane<<6);
  for (y=0;y<cutoff;y++) {
  asm mov cx,4
  asm rep movsb;
  _DI += 76;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 }
 }
}

void JclearPage(int page, char color) {
  int pageme;

  _ES = 0xA000;
  pageme = page*19200;
  asm {
    mov ax,0x0F02
    mov dx,0x3C4
    out dx,ax
    mov cx,8000
    mov di,pageme
    mov ah,color
    mov al,color
    rep stosw
  }
}

void backdraw() {
   unsigned int plane,loopti,power,melvis,n;

   JclearPage(!curpage,backcolor);
   for (loopti=0;loopti<numplayers;loopti++) {
    if ((players[loopti].playeron) && (players[loopti].specnum)
       && (!players[loopti].isgoal)) {
      melvis = (players[loopti].stime << 5) / (specrate[players[loopti].specnum]);
      n = 16492 + (20*loopti);
      for (power=0;power<=32;power++) {
	if (power >= melvis) {
	   vga[n + 19200] = 3;
	   vga[n] = 3;
	 }
	else {
	   vga[n] = 12;
	   vga[n + 19200] = 12;
	 }
	n += widthBytes;
       }
      }
  }
  fontback = 0;
  fontbackon = 1;
  for (loopti=0;loopti<numplayers;loopti++)
    if ((players[loopti].playeron) && (!players[loopti].compu)
       && (players[loopti].specnum == 9) && (!players[loopti].isgoal))
  drawfont(minenum[MAXMINES-players[loopti].curshot],80*loopti+62,230);
  fontbackon = 0;

  if (platson)
  for (plane = 0; plane < platformnum; plane++)
  {
  if ((platforms[plane].effecttype == BUTT) && (platforms[plane].type)) {
    if (platforms[plane].length) drawnorm3(BUTTON+1,platforms[plane].startx,
						    platforms[plane].starty,5);
    else drawnorm3(BUTTON,platforms[plane].startx,platforms[plane].starty,5);
  }
  else
  {
   n = !(!(platforms[plane].xveloc));
	if  (platforms[plane].type & 1)
   {
     switch (platforms[plane].effecttype) {
       case 7 :
       case SHOOTABLE:
       case 0 : for (loopti=0;loopti<platforms[plane].length;loopti++)
		drawtile2(PLATSPRITE,platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty,9);
		break;
       case 1 : for (loopti=0;loopti<platforms[plane].length;loopti++)
		drawnorm2(FIRESPRITE+((gameturn + loopti) & 3),platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty);
		break;
       case 2 : drawnorm2(FORCECORNER,platforms[plane].startx,platforms[plane].starty);
		for (loopti=1;loopti<platforms[plane].length-1;loopti++)
		drawnorm2(FORCELEFT+curpage,platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty);
		drawnorm2(FORCECORNER,platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty);
		break;
       case 4 : drawnorm2(CONVLEFT+curpage*n,platforms[plane].startx,platforms[plane].starty);
		for (loopti=1;loopti<platforms[plane].length-1;loopti++)
		drawnorm2(CONVMID+curpage*n,platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty);
		drawnorm2(CONVRIGHT+curpage*n,platforms[plane].startx+(MAXSPRITEX*loopti),platforms[plane].starty);
		break;
       }
     }
    else
	 if (platforms[plane].type & 2) {
      switch (platforms[plane].effecttype) {
       case 7 :
       case SHOOTABLE:
       case 0 : for (loopti=0;loopti<platforms[plane].length;loopti++)
		drawtile2(PLATSPRITE,platforms[plane].startx,platforms[plane].starty+(9*loopti),9);
		break;
       case 1 : for (loopti=0;loopti<platforms[plane].length;loopti++)
		drawnorm2(FIRESPRITE+((gameturn + loopti) & 3),platforms[plane].startx,platforms[plane].starty+(9*loopti));
		break;
       case 2 : drawnorm2(FORCECORNER,platforms[plane].startx,platforms[plane].starty);
		for (loopti=1;loopti<platforms[plane].length-1;loopti++)
		drawnorm2(FORCEUP+curpage,platforms[plane].startx,platforms[plane].starty+(loopti<<4));
		drawnorm2(FORCECORNER,platforms[plane].startx,platforms[plane].starty+(loopti<<4));
		break;
       case 4 : drawnorm2(CONVLEFT+curpage,platforms[plane].startx,platforms[plane].starty);
		for (loopti=1;loopti<platforms[plane].length-1;loopti++)
		drawnorm2(CONVMID+curpage,platforms[plane].startx,platforms[plane].starty+(9*loopti));
		drawnorm2(CONVRIGHT+curpage,platforms[plane].startx,platforms[plane].starty+(9*loopti));
		break;
       }
    }
   }
  }
}

int distancex(int px,int zx)
{
  if ((zx >= WRAPWIDTH) && (px < MAXSPRITEX))
     return (px+REALWIDTH)-zx;
  else
  if ((px >= WRAPWIDTH) && (zx < MAXSPRITEX))
     return (zx+REALWIDTH)-px;
  else
  return abs(zx-px);
}

int righter(int px,int zx)
{
// returns 1 if px is to the right of zx
  if ((zx % 320 >= WRAPWIDTH) && (px % 320 < MAXSPRITEX))
     return 1;
  else
  if ((px % 320 >= WRAPWIDTH) && (zx % 320 < MAXSPRITEX))
     return 0;
  else
  if ((zx < MAXSPRITEX) && (px < REALWIDTH) && ((px+MAXSPRITEX) % 320 < MAXSPRITEX))
     return 0;
  else
  if ((px < MAXSPRITEX) && (zx < REALWIDTH) && ((zx+MAXSPRITEX) % 320 < MAXSPRITEX))
     return 1;
  else
  return (px > zx);
}

void animate()
{
  unsigned int x,z,p,y,t,q,znum,pnum;

  backdraw();
  for (z=0;z<numtexts;z++)
   {
     fontfore = textsc[z];
     writetextxy(texts[z],textsx[z],textsy[z]);
   }
  for (z=0;z<5;z++)
     if (players[z].isgoal)
	  drawnorm2(GOALSPRITE,sprite[13].x,sprite[13].y);
	else
	if (players[z].playeron) {
	if (players[z].shot) {
	    for (x=0;x<MAXMINES;x++) {
	    if (players[z].mineon[x])
	      putpixel(players[z].minex[x],players[z].miney[x],14);
	    if (players[z].mineon[x]) {
	    for (p=0;p<numplayers;p++) {
	    if (players[p].playeron)
	    if (p != z) {
	    if (players[p].shot) /* shooting down other shots */ {
	    for (t=0;t<MAXMINES;t++)
	     if ((players[p].mineon[t]) && (players[z].mineon[x]))
	     if ((abs(players[p].minex[t]-players[z].minex[x]) <= SHOTSPEED) &&
		(abs(players[p].miney[t]-players[z].miney[x]) < 2)) {
		  players[z].mineon[x] = 0;
		  players[p].mineon[t] = 0;
		  drawnorm3(BLOWSPRITE+curpage,players[p].minex[t],players[p].miney[t],10);
		 }
		} /* end of shot section */
	     else if (players[p].mine) /* shooting down mines */ {
		  for (t=0;t<players[p].curshot;t++)
		  if (players[p].mineon[t])
		  if ((players[z].miney[x] > players[p].miney[t]) &&
		     (players[z].miney[x] < players[p].miney[t]+5)) {
		   if (((players[z].minex[x] > players[p].minex[t]) &&
		     (players[z].minex[x] < players[p].minex[t]+5)) ||
		     ((players[z].minex[x] < players[p].minex[t]) &&
		     (players[z].minex[x]-(players[z].shotdir[x]*SHOTSPEED) > players[p].minex[t])) ||
		     ((players[z].minex[x]+(players[z].shotdir[x]*SHOTSPEED) > players[p].minex[t]) &&
		     (players[z].minex[x]-(players[z].shotdir[x]*SHOTSPEED) < players[p].minex[t]))){
		    players[z].mineon[x] = 0;
		    players[p].mineon[t] = 0;
		    drawnorm3(BLOWSPRITE+curpage,players[p].minex[t],players[p].miney[t],10);
		   }
		   }
	     }
			      else
	      for (q=0;q<platformnum;q++)
	       for (t=0;t<MAXMINES;t++)
		if (players[z].mineon[t])
		if ((platforms[q].effecttype == BUTT) ||
		    (platforms[q].effecttype == SHOOTABLE) ||
		    (platforms[q].effecttype == UNPASS) ||
		    (platforms[q].effecttype == FIRE))
		if ((((distancex(players[z].minex[t],platforms[q].startx) <
			SHOTSPEED) ||
		   ((players[z].minex[t] >= platforms[q].startx) &&
		   (players[z].minex[t] <= platforms[q].startx+widths[platforms[q].effecttype])) ||
		   ((players[z].minex[t] <= platforms[q].startx) &&
		   (players[z].minex[t]+SHOTSPEED >= platforms[q].startx))) &&

		   ((players[z].miney[t] >= platforms[q].starty) &&
		   (players[z].miney[t] <= platforms[q].starty+
		   (effectrangesy[platforms[q].effecttype]*platforms[q].length)))))
			 {
			   if (platforms[q].effecttype == BUTT) {
			      players[z].mineon[t] = 0;
			   }
			   else
			   if (platforms[q].effecttype == SHOOTABLE) {
			      players[z].mineon[t] = 0;
					platforms[q].type = 0;
			      platforms[q].temptype = 0;
			   }
			   else
			   players[z].mineon[t] = 0;
			 }
	      } /* end of p != z */
	     } /* for p... */
	     } /* if players mineon */
	   } /* if x */
	} /* if players z */
	if (players[z].mine) {
	     for (y=0;y<MAXMINES;y++)
	       if (players[z].mineon[y])
	       drawnorm3(MINESPRITE+curpage,players[z].minex[y],players[z].miney[y],5);
	   }
	if (!players[z].invis) {
	x = z*3+1;
	if (players[z].dir) {
	   if (shownum) drawfont(players[z].num,sprite[x].x+5,sprite[x].y-7);
		if (players[z].armored)
		  drawnorm2(LIGHTSPRITE+curpage,sprite[x].x,sprite[x].y);
		drawnorm(x);
		drawnorm2(RIDERSPRITE,sprite[x].x,sprite[x].y);
		drawnorm(x+2);
		if (players[z].unharm)
		  drawnorm2(UNHARMSPRITE+curpage,sprite[x].x,sprite[x].y);
		if (players[z].stunned)
		  drawnorm3(CONFSPRITE+curpage,sprite[x].x,sprite[x].y - 4,3);
		}
	else {
	  if (shownum)
		drawfont(players[z].num,sprite[x].x+5,sprite[x].y-7);
		if (players[z].armored)
		  drawnorm2(LIGHTSPRITE+curpage,sprite[x].x,sprite[x].y);
		drawback(x);
		drawback(x+1);
		drawback(x+2);
		if (players[z].unharm)
		  drawnorm2(UNHARMSPRITE+curpage,sprite[x].x,sprite[x].y);
		if (players[z].stunned)
		  drawnorm3(CONFSPRITE+curpage,sprite[x].x,sprite[x].y - 4,3);
		}
	  }
   }
   for (z=0;((z<numplayers) && (gameover < gameplayers));z++){ /* man with lance */
   for (p=0;((p<numplayers) && (gameover < gameplayers));p++){ /* man supposedly being hit */
   if ((players[p].playeron) || (players[p].isgoal))
   if ((players[z].playeron) || (players[z].isgoal))
   if (p != z) /* so lance doesn't hit it's own sprite */ {
    znum = z*3+1;
    pnum = p*3+1;
    if (distancex(sprite[pnum].x,sprite[znum].x) < 16) {
    if (abs(sprite[pnum].y-sprite[znum].y) < 16) {
     if ((players[z].isgoal) && (goal)) {
       players[z].playeron = 0;
       autowinner = p;
       gameover = 10;
     }
     else
     if ((players[p].isgoal) && (goal)) {
       players[p].playeron = 0;
       autowinner = z;
       gameover = 10;
     }
     else
      if ((sprite[znum].y == sprite[pnum].y) && (players[p].dir == players[z].dir)) {
	  if (((!players[z].dir) && (!righter(sprite[znum].x,sprite[pnum].x))) ||
	       ((players[z].dir) && (righter(sprite[znum].x,sprite[pnum].x))));
	  }
	 }
	}
      }
    }
  }
  if (waitforvr) waitvr();
}

void reviseplats()
{
  int x;

  for (x=0;x<platformnum;x++)
   {
   /*
     if ((platforms[x].effecttype == 6) && (platforms[x].length))
	   platforms[x].length--;
     if ((platforms[x].xlim1 != -1) && (platforms[x].startx <= platforms[x].xlim1))
	platforms[x].xveloc *= -1;
     else
     if ((platforms[x].xlim2 != -1) && (platforms[x].startx >= platforms[x].xlim2))
	platforms[x].xveloc *= -1;
     else
     if ((platforms[x].ylim1 != -1) && (platforms[x].starty <= platforms[x].ylim1))
	platforms[x].yveloc *= -1;
     else
     if ((platforms[x].ylim2 != -1) && (platforms[x].starty >= platforms[x].ylim2))
	platforms[x].yveloc *= -1;
     if (platforms[x].effecttype != 4) platforms[x].startx += platforms[x].xveloc;
     platforms[x].starty += platforms[x].yveloc;
     */
     platforms[x].startx %= REALWIDTH;
     platforms[x].starty %= 192;
     platforms[x].count++;
/*     if ((platforms[x].cycletype & 4) || (!platforms[x].numcycles))
     {
     if (odd(platforms[x].cycletype >> 1))
	  {
	   if (platforms[x].count == platforms[x].offcycles)
	    {
	     platforms[x].count = 0;
	     if (platforms[x].numcycles > 0) platforms[x].numcycles--;
		  platforms[x].temptype = platforms[x].type;
		  platforms[x].type = 0;
	     if ((platforms[x].cycletype & 2) == 2)
	       platforms[x].cycletype ^= 2;
	    }
	  }
     else
	 {
	  if (platforms[x].count == platforms[x].oncycles)
	     {
	      platforms[x].count = 0;
			platforms[x].type = platforms[x].temptype;
	      platforms[x].cycletype ^= 2;
	     }
	 }
       } */
   }
}

int inrange(int px,int startx,int endx)
{
 int c;

 c = px+MAXSPRITEX;
 if ((c > startx) && (px < endx) && (endx < REALWIDTH)) return 1;
 else
 if ((endx >= REALWIDTH) && (px < endx % REALWIDTH)) return 1;
 else
 if ((endx >= REALWIDTH) && (c > startx)) return 1;
 else
 if ((startx < MAXSPRITEX) && (px < REALWIDTH) && (c%320 < MAXSPRITEX)) return 1;
 else
 return 0;
}

void initplayers()
{
  int p,n;

  for (p=0;p<numplayers;p++)
   {
     players[p].yveloc = 0;
     players[p].xveloc = 0;
     players[p].lkey = 0;
     players[p].rkey = 0;
     players[p].ukey = 0;
     players[p].skey = 0;
     players[p].curshot = 0;
     players[p].stime=0;
     players[p].stunned = 0;
     players[p].invis = 0;
     players[p].unharm = 0;
     players[p].isgoal = 0;
     players[p].shot = 0;
     players[p].mine = 0;
     players[p].armored=0;
     players[p].plat = 0;
     players[p].heightaim = 10;

     if (players[p].menuon) players[p].playeron = 1;

     for (n=0;n<10;n++)
       {
	 players[p].mineon[n] = 0;
	 players[p].shotdir[n] = 0;
	 players[p].minex[n] = 0;
	 players[p].miney[n] = 0;
       }
   }
}

void initplayall()
{
  int p;
  for (p=0;p<numplayers;p++)
   {
     players[p].menuon = 1;
     players[p].birdtype = p;
   }
  players[0].num = '1';
  players[1].num = '2';
  players[2].num = '3';
  players[3].num = '4';
}

void initsprites()
{
  int p;

  sprite[1].x = startx1;
  sprite[4].x = startx2;
  sprite[7].x = startx3;
  sprite[10].x = startx4;
  sprite[1].y = starty1;
  sprite[4].y = starty2;
  sprite[7].y = starty3;
  sprite[10].y = starty4;
  sprite[13].y = goaly;
  sprite[13].x = goalx;
  for (p=0;p<5;p++)
   {
       sprite[p*3+2].x = sprite[p*3+1].x;
       sprite[p*3+2].y = sprite[p*3+1].y;
       sprite[p*3+3].x = sprite[p*3+1].x;
       sprite[p*3+3].y = sprite[p*3+1].y;
   }
}

void loadlevel(char name[80])
{
  FILE *stream;
  int x,k,z;

  buttcount = 0;
  if ((stream = fopen(name,"rb")) == NULL) printf("Level nonexistant\n");
  else
  {
    fread(&backcolor,1,1,stream);
    fread(&platformnum,sizeof(platformnum),1,stream);
    fread(&startx1,sizeof(startx1),1,stream);
    fread(&starty1,sizeof(starty1),1,stream);
    fread(&startx2,sizeof(startx2),1,stream);
    fread(&starty2,sizeof(starty2),1,stream);
    fread(&startx3,sizeof(startx3),1,stream);
    fread(&starty3,sizeof(starty3),1,stream);
    fread(&startx4,sizeof(startx4),1,stream);
    fread(&starty4,sizeof(starty4),1,stream);
    fread(&numtexts,sizeof(numtexts),1,stream);
    for (x=0;x<platformnum;x++)
    {
      fread(&platforms[x].startx,sizeof(int),1,stream);
      fread(&platforms[x].starty,sizeof(int),1,stream);
      fread(&platforms[x].length,sizeof(int),1,stream);
      fread(&platforms[x].xveloc,sizeof(int),1,stream);
      fread(&platforms[x].yveloc,sizeof(int),1,stream);
      fread(&platforms[x].xlim1,sizeof(int),1,stream);
      fread(&platforms[x].xlim2,sizeof(int),1,stream);
      fread(&platforms[x].ylim1,sizeof(int),1,stream);
      fread(&platforms[x].ylim2,sizeof(int),1,stream);
		fread(&platforms[x].type,sizeof(char),1,stream);
      platforms[x].active = 1;
      if ((x % 3) == 1)
      {
		if ((platforms[x].type & 3) == 1)
	{
	  platforms[x-1].snafu = x+1;
	  platforms[x-1].orient = -1;
	  platforms[x].snafu = x+1;
	  platforms[x].orient = -1;
	  platforms[x+1].snafu = x+1;
	  platforms[x+1].orient = -1;
	}
		if ((platforms[x].type & 3) == 2)
	{
	  platforms[x-1].snafu = x-1;
	  platforms[x-1].orient = 1;
	  platforms[x].snafu = x-1;
	  platforms[x].orient = 1;
	  platforms[x+1].snafu = x-1;
	  platforms[x+1].orient = 1;
	}
      }
      fread(&platforms[x].effecttype,sizeof(char),1,stream);
      fread(&platforms[x].fieldtype,sizeof(char),1,stream);
      fread(&platforms[x].oncycles,sizeof(int),1,stream);
      fread(&platforms[x].offcycles,sizeof(int),1,stream);
      fread(&platforms[x].numcycles,sizeof(int),1,stream);
      fread(&platforms[x].cycletype,sizeof(int),1,stream);
      for (z=0;z<20;z++)
	fread(&platforms[x].extraeffects[z],sizeof(char),1,stream);
		platforms[x].temptype = platforms[x].type;
      platforms[x].count = 0;
//      if (platforms[x].effecttype == SHOOTABLE) platforms[x].length = 1;
    }
     if (numtexts)
      for (z=0;z<numtexts;z++)
       {
	fread(texts[z],sizeof(char),52,stream);
	fread(&textsx[z],sizeof(textsx[z]),1,stream);
	fread(&textsy[z],sizeof(textsy[z]),1,stream);
	fread(&textsc[z],sizeof(textsc[z]),1,stream);
        fread(&texton[z],sizeof(texton[z]),1,stream);
       }
    fread(&goal,sizeof(goal),1,stream);
    if (goal)
    {
      fread(&goalx,sizeof(goalx),1,stream);
      fread(&goaly,sizeof(goaly),1,stream);
      for (x=0;x<16;x++)
	fread(&goalsprite[x],1,1,stream);
    }
    for (k=0;k<4;k++)
      for (z=0;z<50;z++)
	fread(&story[k][z],1,1,stream);
    for (z=0;z<50;z++)
	fread(&levname[z],1,1,stream);
  }
  fclose(stream);
  if (goal)
    {
      loadsprite(goalsprite,GOALSPRITE);
      players[4].yveloc = 0;
      players[4].xveloc = 0;
      players[4].lkey = 0;
      players[4].rkey = 0;
      players[4].ukey = 0;
      players[4].skey = 0;

      players[4].curshot = 0;
      players[4].stime=0;
      players[4].stunned = 0;
      players[4].invis = 0;
      players[4].unharm = 0;
      players[4].shot = 0;
      players[4].mine = 0;
      players[4].armored=0;
      players[4].plat = 0;
      players[4].isgoal = 1;
      players[4].playeron = 0;
      players[4].menuon = 0;

      for (z=0;z<10;z++)
	{
	 players[4].mineon[z] = 0;
	 players[4].shotdir[z] = 0;
	 players[4].minex[z] = 0;
	 players[4].miney[z] = 0;
	}
      sprite[13].x = goalx;
      sprite[13].y = goaly;
    }
}

void savelevel()
{
 FILE *stream;
 int k,x,z;

  normmode();
  printf("Save Filename: \n");
  readin(filename);
  printf("Filename is... %s",filename);
  printf("Enter the name of the level, in under 50 characters.\n");
  printf("01234567890123456789012345678901234567890123456789\n");
  readin(levname);
  printf("Name is %s\n",levname);
  printf("Enter the story.  You have 4 lines, use less than 50 characters each.\n");
  printf("01234567890123456789012345678901234567890123456789\n");
  for (k=0;k<4;k++)
    readin(story[k]);
 if ((stream = fopen(filename,"wb")) == NULL)
  {
    printf("Some kind of error.\n");
    exit(0);
  }
  else
  {
    printf("Saving...\n");
    fwrite(&backcolor,sizeof(backcolor),1,stream);
    fwrite(&platformnum,sizeof(platformnum),1,stream);
    fwrite(&startx1,sizeof(startx1),1,stream);
    fwrite(&starty1,sizeof(starty1),1,stream);
    fwrite(&startx2,sizeof(startx2),1,stream);
    fwrite(&starty2,sizeof(starty2),1,stream);
    fwrite(&startx3,sizeof(startx3),1,stream);
    fwrite(&starty3,sizeof(starty3),1,stream);
    fwrite(&startx4,sizeof(startx4),1,stream);
    fwrite(&starty4,sizeof(starty4),1,stream);
    fwrite(&numtexts,sizeof(numtexts),1,stream);
    for (x=0;x<platformnum;x++)
    {
      fwrite(&platforms[x].startx,sizeof(int),1,stream);
      fwrite(&platforms[x].starty,sizeof(int),1,stream);
      fwrite(&platforms[x].length,sizeof(int),1,stream);
      fwrite(&platforms[x].xveloc,sizeof(int),1,stream);
      fwrite(&platforms[x].yveloc,sizeof(int),1,stream);
      fwrite(&platforms[x].xlim1,sizeof(int),1,stream);
      fwrite(&platforms[x].xlim2,sizeof(int),1,stream);
      fwrite(&platforms[x].ylim1,sizeof(int),1,stream);
      fwrite(&platforms[x].ylim2,sizeof(int),1,stream);
		fwrite(&platforms[x].type,sizeof(char),1,stream);
      fwrite(&platforms[x].effecttype,sizeof(char),1,stream);
      fwrite(&platforms[x].fieldtype,sizeof(char),1,stream);
      fwrite(&platforms[x].oncycles,sizeof(int),1,stream);
      fwrite(&platforms[x].offcycles,sizeof(int),1,stream);
      fwrite(&platforms[x].numcycles,sizeof(int),1,stream);
      fwrite(&platforms[x].cycletype,sizeof(int),1,stream);
      for (z=0;z<20;z++)
	fwrite(&platforms[x].extraeffects[z],sizeof(char),1,stream);
    }
    if (numtexts) for (z=0;z<numtexts;z++)
       {
	fwrite(texts[z],sizeof(char),STRMAXLEN,stream);
	fwrite(&textsx[z],sizeof(textsx[z]),1,stream);
	fwrite(&textsy[z],sizeof(textsy[z]),1,stream);
	fwrite(&textsc[z],sizeof(textsc[z]),1,stream);
        fwrite(&texton[z],sizeof(char),1,stream);
       }
    fwrite(&goal,sizeof(goal),1,stream);
    if (goal)
    {
    fwrite(&goalx,sizeof(goalx),1,stream);
    fwrite(&goaly,sizeof(goaly),1,stream);
    for (z=0;z<16;z++)
      fwrite(&goalsprite[z],1,1,stream);
    }
    for (k=0;k<4;k++)
      for (z=0;z<50;z++)
	fwrite(&story[k][z],1,1,stream);
    for (z=0;z<50;z++)
	fwrite(&levname[z],1,1,stream);
    fclose(stream);
  }
  printf("Done.\n");
  fixmode();
}

void keychek()
{
  char t,poin[4];
  int n;
  char f;

  t = getch();
  fontbackon = 1;
  switch (toupper(t))
    {
      case '\\': if (incr == 1) incr = 8; else incr = 1; break;
      case '+' : if (backcolor == 0xFF) backcolor = 0; else backcolor++;
		 break;
		case '-' : if (backcolor) backcolor--; else backcolor = 0xFF; break;
      case '[' : if (platptr) platptr-=3; break;
      case ']' : if (platptr < 150) platptr+=3; break;
      case '{' : if (textptr) textptr--; break;
      case '}' : if ((textptr+1 < MAXTEXTS) && (textd[textptr]))
			textptr++; break;
      case '|' : normmode();
		 printf("Enter text now:\n");
		 readin(texts[textptr]);
		 textd[textptr] = 1;
		 if (textptr >= numtexts) numtexts = textptr+1;
                 printf("Start on? [0=no,1=yes]\n");
					  scanf("%d",&question);
					  texton[textptr] = question;
		 fixmode();
		 break;
		case 'N' : platforms[platptr].type = 1+4+8+16+32+64;
		 if (platptr >= platformnum) platformnum = platptr+3;
		 platforms[platptr+2].type = 0;
		 HEADING = 1;
		 MAIN = platptr;
		 platforms[platptr].active = 1;
		 platforms[MAIN].length = 1;
		 platforms[MAIN].effecttype = 0;
		 break;
		case 'Q' : end=1; break;
		case 'F' : normmode();
		 printf("Load Level Filename: \n");
		 readin(filename);
		 printf("Filename is... %s",filename);
		 fixmode();
		 loadlevel(filename);
		 break;
		case 'G' : goal=!goal; players[4].isgoal = !players[4].isgoal;
		 players[4].playeron = !players[4].playeron;
		 break;
		case 'I' : normmode();
		 printf("Goal Sprite Filename: \n");
		 readin(filename);
		 printf("Filename is... %s",filename);
		 fixmode();
		 loadsprite(filename,GOALSPRITE);
		 break;
		case '1' : playptr = 0; break;
		case '2' : playptr = 1; break;
		case '3' : playptr = 2; break;
		case '4' : playptr = 3; break;
		case '5' : playptr = 4; break;
		case '6' : playptr = 5; break;
		case 'E' : savelevel(); break;
		case 'P' : switch (playptr) {
			case 0 : starty1-=incr; break;
			case 1 : starty2-=incr; break;
			case 2 : starty3-=incr; break;
			case 3 : starty4-=incr; break;
			case 4 : goaly-=incr; break;
			case 5 : textsy[textptr]-=incr; break;} break;
		case 'L' : switch (playptr) {
			case 0 : startx1-=incr; break;
			case 1 : startx2-=incr; break;
			case 2 : startx3-=incr; break;
			case 3 : startx4-=incr; break;
			case 4 : goalx-=incr; break;
			case 5 : textsx[textptr]-=incr; break; } break;
	  case ';' : switch (playptr) {
			case 0 : starty1+=incr; break;
			case 1 : starty2+=incr; break;
			case 2 : starty3+=incr; break;
			case 3 : starty4+=incr; break;
			case 4 : goaly+=incr; break;
			case 5 : textsy[textptr]+=incr; break;} break;
		case '\'': switch (playptr) {
			case 0 : startx1+=incr; break;
			case 1 : startx2+=incr; break;
			case 2 : startx3+=incr; break;
			case 3 : startx4+=incr; break;
			case 4 : goalx+=incr; break;
			case 5 : textsx[textptr]+=incr; break;} break;
		case '9' : if (textsc[textptr]) textsc[textptr]--; break;
		case '0' : textsc[textptr]++; break;
  }
  if (platforms[platptr].active)
  {
  switch (toupper(t))
	 {
		case '`' : printf("Does this platform exist after a button hit [0] or immediately[1]?\n");
		 scanf("%d",&f);
		 if (f) platforms[platptr].type |= 64;
			else platforms[platptr].type =
			(platforms[platptr].type | 64)-64;
		 f = 0;
		 break;
		case '(' : if (platforms[MAIN].effecttype)
		 platforms[MAIN].effecttype--; break;
		case ')' : if (platforms[MAIN].effecttype < 7)
		 platforms[MAIN].effecttype++; break;
		case 'O' : platforms[platptr].active = 0;
		 TYPE = 0;
		 break;
		case '!' : if (platforms[MAIN].xlim1 == -1)
			platforms[MAIN].xlim1 = platforms[MAIN].startx;
		 else platforms[MAIN].xlim1 = -1;
		 break;
		case '@' : if (platforms[MAIN].xlim2 == -1)
			platforms[MAIN].xlim2 = platforms[MAIN].startx;
		 else platforms[MAIN].xlim2 = -1;
		 break;
		case '#' : if (platforms[MAIN].ylim1 == -1)
			platforms[MAIN].ylim1 = platforms[MAIN].starty;
		 else platforms[MAIN].ylim1 = -1;
		 break;
		case '$' : if (platforms[MAIN].ylim2 == -1)
			platforms[MAIN].ylim2 = platforms[MAIN].starty;
		 else platforms[MAIN].ylim2 = -1;
		 break;
		case 'B' : normmode();
		 if (platforms[MAIN].effecttype == FORCEFIELD)
		 {
			printf("Enter field effect:\n7 means it gives 100 cycles unharm\n");
			printf("6 means it gives a stun\n");
			printf("5 means it gives a lightning armor\n");
			printf("4 means it gives a transmogrify\n");
			printf("3 means it turns ya invisible\n");
			printf("2 means it gives you a teleport\n");
			printf("1 means random effect\n");
			printf("8 means it kills you\n");
			scanf("%d",&platforms[MAIN].fieldtype);
			printf("Does it work more than once? [1=yes, 0 = no]\n");
			scanf("%d",&question);
			if (!question) platforms[MAIN].fieldtype^=128;
		 }
		 else
		 if (platforms[MAIN].effecttype == BUTT)
		 for (effectcount=0;effectcount<10;effectcount++)
		 {
		  printf("What is the #%d effect [of 10]:\n ",effectcount);
		  printf("1...Kill a platform triplet\n");
		  printf("2...Revive all players\n");
		  printf("3...Reverse a conveyor belt or all of them\n");
		  printf("4...Transmogrify all players\n");
		  printf("5...Set all players to unharm\n");
		  printf("6...Turn on a platform triplet or revive it\n");
		  printf("7...Stop a conveyor belt or platform triplet X-wise\n");
		  printf("8...Stop a platform triplet Y-wise\n");
		  printf("9...Kill the goal\n");
		  printf("10...Revive the goal\n");
		  printf("11...Flip a platform (if it's on, make it off, or vice versa)\n");
		  printf("12...Turn off a piece of text\n");
		  printf("13...Turn on a piece of text\n");
		  printf("14...Flip a piece of text\n");
		  printf("15...Make sure another button is being pressed before going on\n");
		  printf("0...End\n");
		  scanf("%d",&platforms[MAIN].extraeffects[effectcount*2]);
		  switch (platforms[MAIN].extraeffects[effectcount*2])
			{
			  case 1 :
			  case 3 :
			  case 6 :
			  case 7 :
			  case 8 :
			  case 15:
			  case 11:
			  case 12:
			  case 13:
			  case 14:
			  printf("Which one? [255 for all] \n");
					scanf("%d",&platforms[MAIN].extraeffects[effectcount*2+1]);
					break;
			  case 0 : platforms[MAIN].extraeffects[effectcount*2+1] = 0;
					effectcount = 12;	break;
			}
		 }
		 fixmode();
		 break;
		case 'A' : platforms[MAIN].startx-=incr;
		 break;
		case 'W' : platforms[MAIN].starty-=incr;
		 break;
		case 'D' : platforms[MAIN].startx+=incr;
		 break;
		case 'S' : platforms[MAIN].starty+=incr;
		 break;
		case 'V' : if ((ORIENT) == 1)
		 {
			TYPE += 1;
			MAIN = platptr+2;
			HEADING = -1;
			TYPE = platforms[platptr].type;
			platforms[MAIN].length = platforms[platptr].length;
			platforms[platptr].length = 1;
			platforms[MAIN].startx = platforms[platptr+1].startx;
			platforms[MAIN].starty = platforms[platptr+1].starty;
			platforms[MAIN].effecttype = platforms[platptr].effecttype;
		 }
		 break;
		case 'H' : if ((ORIENT) == 2)
		 {
			TYPE -= 1;
			MAIN = platptr;
			HEADING = 1;
			TYPE = platforms[platptr+2].type;
			platforms[MAIN].length = platforms[platptr+2].length;
			platforms[platptr+2].length = 1;
			platforms[MAIN].startx = platforms[platptr+1].startx;
			platforms[MAIN].starty = platforms[platptr+1].starty;
			platforms[MAIN].effecttype = platforms[platptr+2].effecttype;
		 }
		 break;
		case '<' : if (platforms[MAIN].length > 1)
		 platforms[MAIN].length--;
		 break;
		case '>' : if (platforms[MAIN].length < 20)
		  platforms[MAIN].length++; break;
		case 'X' : platforms[MAIN].xveloc--; break;
		case 'C' : platforms[MAIN].xveloc++; break;
		case '7' : platforms[MAIN].yveloc--; break;
		case '8' : platforms[MAIN].yveloc++; break;
		case 'R' : normmode();
		 if (!(platforms[MAIN].cycletype & 1))
		 {
			platforms[MAIN].cycletype = 1;
			printf("Enter on cycle time: (-1 to be always on)\n");
			scanf("%d",&platforms[MAIN].oncycles);
			printf("Enter off cycle time: (0 to avoid this question)\n");
			scanf("%d",&platforms[MAIN].offcycles);
			printf("Does it start ON or OFF [ON = 1, OFF = 0]\n");
			scanf("%d",&question);
			if (question) platforms[MAIN].cycletype+=2;
			printf("How many cycles does this platform make? -1 = infinite\n");
			scanf("%d",&platforms[MAIN].numcycles);
			if (platforms[MAIN].numcycles == -1) platforms[MAIN].cycletype += 4;
		 }
		 else
		 {
			platforms[MAIN].cycletype = 0;
		 }
		 fixmode();
		 break;
	 }
 }

 writetextxy("#",0,220);
 writetextxy(itoa(platptr,poin,10),10,220);
 writetextxy("L",40,220);
 writetextxy(itoa(platforms[MAIN].yveloc,poin,10),100,220);
 writetextxy(itoa(platforms[MAIN].xveloc,poin,10),100,230);
 writetextxy(itoa(platforms[MAIN].xlim1,poin,10),200,200);
 writetextxy(itoa(platforms[MAIN].xlim2,poin,10),200,210);
 writetextxy(itoa(platforms[MAIN].ylim1,poin,10),200,220);
 writetextxy(itoa(platforms[MAIN].ylim2,poin,10),200,230);
 writetextxy(itoa(textptr,poin,10),240,230);
 if (goal) writetextxy("GOAL ON ",0,200);
		else writetextxy("GOAL OFF",0,200);
 if (platforms[platptr].active == 0)
	writetextxy("INACTIVE",10,230);
 else
	writetextxy("ACTIVE  ",10,230);
 switch (platforms[MAIN].effecttype) {
	 case NORML      : writetextxy("NORMAL    ",10,210); break;
	 case FIRE       : writetextxy("FIRE      ",10,210); break;
	 case FORCEFIELD : writetextxy("FORCEFIELD",10,210); break;
	 case SHOOTABLE  : writetextxy("SHOOTABLE ",10,210); break;
	 case BUTT       : writetextxy("BUTTON    ",10,210); break;
	 case UNPASS     : writetextxy("UNPASS    ",10,210); break;
	 case CONVEYOR   : writetextxy("CONVEYOR  ",10,210); break;
	 case INVISIBLE  : writetextxy("INVISIBLE ",10,210); break;
	}
 if (platforms[platptr].active)
  {
	platforms[MAIN+(HEADING*2)].effecttype =
	platforms[MAIN+(HEADING)].effecttype = platforms[MAIN].effecttype;
	platforms[MAIN+HEADING].startx = platforms[MAIN].startx;
	platforms[MAIN+HEADING].starty = platforms[MAIN].starty;
	if (ORIENT == 1)
	  {
		 platforms[MAIN+(HEADING*2)].startx = platforms[MAIN].startx+(16*(platforms[MAIN].length-1));
		 platforms[MAIN+(HEADING*2)].starty = platforms[MAIN].starty;
	  }
	  else
	if (ORIENT == 2)
	  {
		 if (platforms[MAIN].effecttype == 2)
	 n = 16; else n = 9;
		 platforms[MAIN+(HEADING*2)].startx = platforms[MAIN].startx;
		 platforms[MAIN+(HEADING*2)].starty = platforms[MAIN].starty+(n*(platforms[MAIN].length-1));
	}
//	else
//	if (ORIENT == 0)
//	{
//	  platforms[MAIN+HEADING].type = 0;
//	  platforms[MAIN+(2*HEADING)].type = 0;
//	}

	platforms[MAIN+(HEADING*1)].length = 1;
   platforms[MAIN+(HEADING*2)].length = 1;

   platforms[MAIN+(HEADING*1)].xveloc = platforms[MAIN].xveloc;
   platforms[MAIN+(HEADING*1)].yveloc = platforms[MAIN].yveloc;
	platforms[MAIN+(HEADING*2)].xveloc = platforms[MAIN].xveloc;
   platforms[MAIN+(HEADING*2)].yveloc = platforms[MAIN].yveloc;

	platforms[MAIN+(HEADING*1)].xlim1 = platforms[MAIN].xlim1;
   platforms[MAIN+(HEADING*1)].ylim1 = platforms[MAIN].ylim1;
   platforms[MAIN+(HEADING*2)].xlim1 = platforms[MAIN].xlim1;
   platforms[MAIN+(HEADING*2)].ylim1 = platforms[MAIN].ylim1;
   platforms[MAIN+(HEADING*1)].xlim2 = platforms[MAIN].xlim2;
   platforms[MAIN+(HEADING*1)].ylim2 = platforms[MAIN].ylim2;
   platforms[MAIN+(HEADING*2)].xlim2 = platforms[MAIN].xlim2;
   platforms[MAIN+(HEADING*2)].ylim2 = platforms[MAIN].ylim2;
   if ((platforms[MAIN].xveloc != 0) && (platforms[MAIN].xlim1 != -1))
	  {
       platforms[MAIN+(HEADING*2)].xlim1 += (16*(platforms[MAIN].length-1));
       platforms[MAIN+(HEADING*2)].xlim2 += (16*(platforms[MAIN].length-1));
     }
     if ((platforms[MAIN].yveloc != 0) && (platforms[MAIN].ylim1 != -1))
     {
       platforms[MAIN+(HEADING*2)].ylim1 += (9*(platforms[MAIN].length-1));
       platforms[MAIN+(HEADING*2)].ylim2 += (9*(platforms[MAIN].length-1));
     }

     platforms[MAIN+(HEADING*1)].oncycles = platforms[MAIN].oncycles;
	  platforms[MAIN+(HEADING*2)].oncycles = platforms[MAIN].oncycles;
     platforms[MAIN+(HEADING*1)].offcycles = platforms[MAIN].offcycles;
     platforms[MAIN+(HEADING*2)].offcycles = platforms[MAIN].offcycles;
     platforms[MAIN+(HEADING*1)].numcycles = platforms[MAIN].numcycles;
     platforms[MAIN+(HEADING*2)].numcycles = platforms[MAIN].numcycles;
     platforms[MAIN+(HEADING*1)].cycletype = platforms[MAIN].cycletype;
     platforms[MAIN+(HEADING*2)].cycletype = platforms[MAIN].cycletype;

	  f = platforms[MAIN].type & 64;
     if (ORIENT == 2)
     {
		 platforms[MAIN+(HEADING*1)].type = 1+4+32+f;
		 platforms[MAIN+(HEADING*2)].type = 1+8+32+f;
     }
     else
     if (ORIENT == 1)
     {
		 platforms[MAIN+(HEADING*1)].type = 2+4+32+f;
		 platforms[MAIN+(HEADING*2)].type = 2+8+32+f;
	  }
	 if (
	 (platforms[MAIN].effecttype == FIRE)       ||
	 (platforms[MAIN].effecttype == CONVEYOR)   ||
	 (platforms[MAIN].effecttype == FORCEFIELD) ||
	 (platforms[MAIN].effecttype == BUTT))
	 {
		platforms[MAIN+(HEADING*1)].type = 0;
		platforms[MAIN+(HEADING*2)].type = 0;
		platforms[MAIN+(HEADING*2)].effecttype = 255;
		platforms[MAIN+(HEADING*1)].effecttype = 255;
	 }
	}
	initsprites();
	animate();
}

void initplats()
{
  int x;

  for (x=0;x<150;x++)
	{
	 platforms[x].xlim1 = -1;
	 platforms[x].xlim2 = -1;
	 platforms[x].ylim1 = -1;
	 platforms[x].ylim2 = -1;
	 platforms[x].type = 0;
   }
}

void showlevelinfo()
{
  int x,y;
  char foo[5];

  box(7,9,312,66,96);
  outport(0x3c4,0x0F02);
  for (x=2;x<78;x++)
   for (y=10;y<66;y++)
     vga[actStart + x + y*widthBytes] = 136;
  fontfore = hilite;
  x = (304-(strlen(levname) * 6)>>1)+8;
  writetextxy(levname,x,16);
  fontfore = hiliteoff;
  for (y=0;y<4;y++)
  {
    x = (304-(strlen(story[y]) * 6)>>1)+8;
    writetextxy(story[y],x,24+y*10);
    writetextxy(itoa(buttcount,foo,10),x,24+y*10);
  }
  setVisiblePage(!curpage);
  while (!kbhit());
}


void winnerbox(int winner)
{
  int x,y;

  box(99,79,220,160,96);
  outport(0x3c4,0x0F02);
  for (x=25;x<55;x++)
   for (y=80;y<160;y++)
   vga[actStart + x + y*widthBytes] = 136;
  writetextxy("THE WINNER IS...",110,88);
  writetextxy("PLAYER ",110,96);
  fontfore = hilite;
  drawfont(players[winner].num,150,96);
  fontfore = hiliteoff;
  writetextxy2("O = Options",110,106);
  writetextxy2("Q = quit, R = Replay",110,113);
  setVisiblePage(!curpage);
}

void main()
{
  int x=5;

  fontfore = hiliteoff;
  loadfont();
  loadpal("pal.col",pal);
  asm cld;
  loadsprite("blank.stn",0);
  loadsprite("rider1.stn",1);
  loadsprite("goblet.stn",2);
  loadsprite("par1.stn",3);
  loadsprite("par2.stn",4);
  loadsprite("par3.stn",5);
  loadsprite("par4.stn",6);
  loadsprite("par5.stn",7);
  loadsprite("parw1.stn",8);
  loadsprite("parw2.stn",9);
  loadsprite("hors1.stn",10);
  loadsprite("hors2.stn",11);
  loadsprite("hors3.stn",12);
  loadsprite("hors4.stn",13);
  loadsprite("hors5.stn",14);
  loadsprite("horsew1.stn",15);
  loadsprite("horsew2.stn",16);
  loadsprite("buff1.stn",17);
  loadsprite("buff2.stn",18);
  loadsprite("buff3.stn",19);
  loadsprite("buff4.stn",20);
  loadsprite("buff5.stn",21);
  loadsprite("buffw1.stn",22);
  loadsprite("buffw2.stn",23);
  loadsprite("moth1.stn",24);
  loadsprite("moth2.stn",25);
  loadsprite("moth3.stn",26);
  loadsprite("moth4.stn",27);
  loadsprite("moth5.stn",28);
  loadsprite("mothw1.stn",29);
  loadsprite("mothw2.stn",30);
  loadsprite("bana1.stn",31);
  loadsprite("bana2.stn",32);
  loadsprite("bana3.stn",33);
  loadsprite("bana4.stn",34);
  loadsprite("bana5.stn",35);
  loadsprite("banaw1.stn",36);
  loadsprite("banaw2.stn",37);
  loadsprite("cat1.stn",38);
  loadsprite("cat2.stn",39);
  loadsprite("cat3.stn",40);
  loadsprite("cat4.stn",41);
  loadsprite("cat5.stn",42);
  loadsprite("catw1.stn",43);
  loadsprite("catw2.stn",44);
  loadsprite("bike1.stn",45);
  loadsprite("bike2.stn",46);
  loadsprite("bike3.stn",47);
  loadsprite("bike4.stn",48);
  loadsprite("bike5.stn",49);
  loadsprite("bikew1.stn",50);
  loadsprite("bikew2.stn",51);
  loadsprite("dino1.stn",52);
  loadsprite("dino2.stn",53);
  loadsprite("dino3.stn",54);
  loadsprite("dino4.stn",55);
  loadsprite("dino5.stn",56);
  loadsprite("dinow1.stn",57);
  loadsprite("dinow2.stn",58);
  loadsprite("fish1.stn",59);
  loadsprite("fish2.stn",60);
  loadsprite("fish3.stn",61);
  loadsprite("fish4.stn",62);
  loadsprite("fish5.stn",63);
  loadsprite("fishw1.stn",64);
  loadsprite("fishw2.stn",65);
  loadsprite("classic1.stn",66);
  loadsprite("classic2.stn",67);
  loadsprite("classic3.stn",68);
  loadsprite("classic4.stn",69);
  loadsprite("classic5.stn",70);
  loadsprite("classiw1.stn",71);
  loadsprite("classiw2.stn",72);
  loadsprite("platform.stn",PLATSPRITE);
  loadsprite("platleft.stn",PLATSPRITE+1);
  loadsprite("light1.stn",LIGHTSPRITE);
  loadsprite("light2.stn",LIGHTSPRITE+1);
  loadsprite("mine1.stn",MINESPRITE);
  loadsprite("mine2.stn",MINESPRITE+1);
  loadsprite("unharm1.stn",UNHARMSPRITE);
  loadsprite("unharm2.stn",UNHARMSPRITE+1);
  loadsprite("blow1.stn",BLOWSPRITE);
  loadsprite("blow2.stn",BLOWSPRITE+1);
  loadsprite("conf1.stn",CONFSPRITE);
  loadsprite("conf2.stn",CONFSPRITE+1);
  loadsprite("off.stn",OFFSPRITE);
  loadsprite("splatt1.stn",SPLATSPRITE);
  loadsprite("splatt2.stn",SPLATSPRITE+1);
  loadsprite("fire1.stn",FIRESPRITE);
  loadsprite("fire2.stn",FIRESPRITE+1);
  loadsprite("fire3.stn",FIRESPRITE+2);
  loadsprite("fire4.stn",FIRESPRITE+3);
  loadsprite("force1.stn",FORCELEFT);
  loadsprite("force2.stn",FORCELEFT+1);
  loadsprite("force3.stn",FORCEUP);
  loadsprite("force4.stn",FORCEUP+1);
  loadsprite("forcec.stn",FORCECORNER);
  loadsprite("convl1.stn",CONVLEFT);
  loadsprite("convl2.stn",CONVLEFT+1);
  loadsprite("convm1.stn",CONVMID);
  loadsprite("convm2.stn",CONVMID+1);
  loadsprite("convr1.stn",CONVRIGHT);
  loadsprite("convr2.stn",CONVRIGHT+1);
  loadsprite("butt1.stn",BUTTON);
  loadsprite("butt2.stn",BUTTON+1);
  goal = 0;
  players[4].isgoal = 0;
  players[4].playeron = 0;
  basesproff = FP_OFF(spritedata[0]);
  basesprseg = FP_SEG(spritedata[0]);
  fontfore = 55;
  initplayall();
  initplayers();
  randomize();
  sprite[1].n = 3+(7*players[0].birdtype);
  sprite[2].n = 1;
  sprite[3].n = 5+sprite[1].n;
  sprite[4].n = 3+(7*players[1].birdtype);
  sprite[5].n = 1;
  sprite[6].n = 5+sprite[4].n;
  sprite[7].n = 3+(7*players[2].birdtype);
  sprite[8].n = 1;
  sprite[9].n = 5+sprite[7].n;
  sprite[10].n = 3+(7*players[3].birdtype);
  sprite[11].n = 1;
  sprite[12].n = 5+sprite[10].n;
  fixmode();
  initplats();
  initsprites();
  animate();
  while (!end)
  {
    curpage = !curpage;
    setActivePage(!curpage);
    x++;
    gameturn++;
    reviseplats();
    animate();
    keychek();
    setVisiblePage(!curpage);
  }
  normmode();
}


