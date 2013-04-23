#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "./palhs.h"
#include "./modexhs.h"

#pragma inline

unsigned gameturn;
#define widthBytes 80

#define MAXSPRITEX 16
#define MAXSPRITEY 16
#define STRMAXLEN 52
#define STRMAXLEO 64
#define UPPRESS 5
#define GROUNDLEVEL 184
#define ROOFLEVEL 8
#define NUMMOUNTS 10
#define RIPICON 10
#define EXMOUNTS 11
palette pal;
int gamedelay=1,dinc=1,autowinner=0,redo=0;
char backcolor,uho=1,curpage=0,gameend=0,waitforvr=0;
#define FONTX 5
#define FONTY 6
#define SHOTSPEED 12
char fontfore,hilite = 9,hiliteoff = 14,fontback=0,
     fontbackon=0,specon = 1,shownum = 0,platson = 1,keypress,keypress2=1;
int startx1,starty1,startx2,starty2,startx3,starty3,startx4,starty4;
int numplayers = 4,gameplayers = 3,gameover = 0,oldgameover=0;
#define REALWIDTH 320
#define WRAPWIDTH 304
int gravity = 1;
#define MAXTEXTS 15
char numtexts=5,texts[MAXTEXTS][STRMAXLEN],textsc[MAXTEXTS],texton[MAXTEXTS];
int textsx[MAXTEXTS],textsy[MAXTEXTS];
typedef char spritedata_t[MAXSPRITEX*MAXSPRITEY];
typedef struct sprite_t {
		  int n;
		  int x,y;
		} spritetype;
#define GOALSPRITE 2
spritedata_t spritedata[105];
spritetype sprite[14];
#define NORML 0
#define FIRE 1
#define FORCEFIELD 2
#define SHOOTABLE 3
#define CONVEYOR 4
#define INVISIBLE 5
#define BUTT 6
#define UNPASS 7
char icon[EXMOUNTS][1600];
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
#define drawnorm(spritenum) drawnorm2(sprite[spritenum].n,sprite[spritenum].x,sprite[spritenum].y);

char far image[64000U];
unsigned int basesproff,basesprseg,imageseg,imageoff;
typedef char far stringset[20];
fontl font;
#define MAXLEVELS 100
int numlevels=255,curlevel,goalx,goaly;
char levelnames[MAXLEVELS][20],goal;
char levname[50],story[4][50],goalsprite[16],halfgrav=0;
char effectranges[8]  = { 7,16,16, 7, 7, 7, 5, 7};
char effectrangesy[8] = { 9, 9,16, 9, 9, 9, 5, 9};
char widths[8]	      = {16,16,16,16,16,16, 5,16};
typedef struct playerjunk { //dirs left = 0, right = 1
  char dir, num, playeron, menuon, compu, plat, wasonplat;
  int stime,x,y,xveloc,yveloc,heightaim,rundir;
  char leftkey,rightkey,upkey,speckey,nspeckey,nleftkey,nrightkey,nupkey,
       lkey,rkey,ukey,skey,upkeyon,birdtype,specnum,wins,stunned,invis,
       unharm,shot,mine,armored,curshot,mineon[10],isgoal;
  int shotdir[10],minex[10],miney[10];
} player;
player players[5];

char rounds,computerplayers,livingplayers;
#define MAXMINES 9
char masses[EXMOUNTS]={3,3,4,2,3,3,2,5,2,2,0};
stringset animals[EXMOUNTS] = {"KILLER PARROT   ","PEGASUS         ","WINGED BUFFALO  ","MOTH            ","BANANA WARRIOR  ","CATBIRD         ","BIKE CONTRAPTION","PURPLE DINOSAUR ","FLOATING FISH   ","CLASSIC OSTRICH ","OFF             "};
char maxspeedx[EXMOUNTS]={10,12,10,10, 6,10,11, 6,11,9,0};
char maxspeedy[EXMOUNTS]={12, 8, 8,16,11, 8, 9, 8,12,12,0};
char accelx[EXMOUNTS] =  { 2, 3, 1, 2, 2, 3, 4, 1 ,3, 3,0};
char accely[EXMOUNTS] =  { 4, 2, 3, 5, 2, 3 ,2, 3, 4, 3,0};
char spechigh = 9;
stringset abilities[10] ={"NONE          ","STUNNING SQUAWK","LIGHTNING ARMOR","DROPPING BELLOW","TRANSMOGRIFY   ","INVISIBILITY   ","UNHARMABILITY  ","RANDOM TELEPORT","SHOOT BULLET   ","DROP MINE      "};
char specrate[10]={0,60,60,5,40,10,50,10,20,30};
char minenum[10]={'0','1','2','3','4','5','6','7','8','9'};

typedef struct platinfo_t {
  int startx,starty,length,xveloc,yveloc,xlim1,xlim2,ylim1,ylim2;
  char type,temptype,effecttype,fieldtype,extraeffects[20];
  signed int oncycles,offcycles,count,numcycles,cycletype;
} platinfo;
platinfo platforms[150];
char platformnum = 5;

void keychek();

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

void loadicon(char *ch,int num) {
   FILE *stream;
   int a;

   if ((stream = fopen(ch,"rb")) == NULL) printf("Error.\n");
   else
   for (a=0;a<1600;a++)
      fread(&icon[num][((a&3)*400) + ((a-(a & 3))>>2)],sizeof(char),1,
	stream);
   fclose(stream);
}

void loadkeys() {
   FILE *stream;
  int a;
  if ((stream = fopen("keys.dat","rb")) == NULL) printf("Creating new key file.\n");
  else {
    for (a=0;a<4;a++) {
      fread(&players[a].leftkey,1,1,stream);
      fread(&players[a].rightkey,1,1,stream);
      fread(&players[a].upkey,1,1,stream);
      fread(&players[a].speckey,1,1,stream);
      players[a].nleftkey = players[a].leftkey | 128;
      players[a].nrightkey = players[a].rightkey | 128;
      players[a].nupkey = players[a].upkey | 128;
      players[a].nspeckey = players[a].speckey | 128;
    }
    fclose(stream);
  }
}

void drawicon(int spritenum,int xoff,int yoff) {
  int y,plane,trueplane,laps=0;
  _ES = 0xA000;
  _DS = FP_SEG(icon[spritenum]);
  _SI = FP_OFF(icon[spritenum]);
  trueplane=xoff & 3;
  _BX = actStart + widthBytes*yoff + (xoff >> 2);
  for(plane=0;plane<4;plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX+laps;
  for (y=0;y<40;y++) {
  asm mov cx,10
  foo:;
  asm {
	lodsb;
	or al,al;
	jz skipme;
	stosb;
	loop foo;
	jmp exittor;
      }
  skipme:;
	asm inc di;
	asm loop foo;
  exittor:;
  _DI += 70;
  }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps = 1; }
 }
}

void drawiconback(int spritenum,int xoff,int yoff) {
 int y,plane,trueplane,laps=0;

  _ES = 0xA000;
  _DS = FP_SEG(icon[spritenum]);
  _SI = FP_OFF(icon[spritenum]);
  trueplane = xoff & 3;
  _BX = actStart + widthBytes*yoff + (xoff >> 2) + 10;
  for(plane=3;plane>=0;plane--)
  {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << trueplane);
  _DI = _BX-laps;
  for (y=0;y<40;y++)
  {
  asm mov cx,10
  foo:;
  asm {
	lodsb;
        or al,al;
	jz skipme;
	stosb;
        sub di,2;
        loop foo;
        jmp exittor;
      }
  skipme:;
	asm dec di;
        asm loop foo;
  exittor:;
  _DI += 90;
  }
  trueplane--;
  if (trueplane < 0) { trueplane = 3; laps = 1; }
 }
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
	jmp exittor; }
  skipme:;
	asm loop foo;
  exittor:;
  tdi+=widthBytes; }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; } } }
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
	jmp bexittor; }
  askipme:;
	asm inc di;
	asm loop flofoo;
  bexittor:;
  _DI += 76; }
  trueplane++;
  if (trueplane == 4) { trueplane = 0; laps++; }
 } } }

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

void drawpan() {
 unsigned int plane;

 for (plane = 0; plane < 4; plane++) {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << plane);
  movedata(imageseg,imageoff+3200*plane,0xA000,38400U,3200);
 }
}

void drawpana() {
   int melvis;
   melvis = _DS;
   asm {
    MOV AX,0x0F02
    MOV DX,0x3C4
    OUT DX,AX

    MOV AX,0x4105
    MOV DX,0x3CE
    OUT DX,AX
    }
    _ES = 41960U+(1200*(!curpage));
    _DS = 43360U;
    asm {
    XOR SI,SI
    MOV DI,SI
    MOV CX,3200
    REP MOVSB
    MOV AX,0x4005
    MOV DX,0x3CE
    OUT DX,AX
   }
    _DS = melvis;
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

void kill(int playernum) {
  players[playernum].playeron = 0;
  gameover++;
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
  fontback = 36;
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
  if (platforms[plane].type & 16)
  {
   n = !(!(platforms[plane].xveloc));
   if  (platforms[plane].type & 1) {
     switch (platforms[plane].effecttype) {
       case 7 :
       case SHOOTABLE :
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
       case SHOOTABLE :
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

void loadpanel() {
  FILE *stream;
  unsigned int x;
  stream = fopen("scorebrd.cut","rb");

  for (x=0;x<12800;x++)
     fread(&image[((x & 3)*3200)+((x-(x & 3))>>2)],sizeof(char), 1, stream);
  fclose(stream);
}

void loadimage(char *ch) {
   FILE *stream;
   unsigned int x;
  stream = fopen(ch,"rb");
  for (x=0;x<64000U;x++)
     fread(&image[((x & 3)*16000)+((x-(x & 3))>>2)],1,1,stream);
  fclose(stream);
}

int distancex(int px,int zx) {
  if ((zx >= WRAPWIDTH) && (px < MAXSPRITEX))
     return (px+REALWIDTH)-zx;
  else
  if ((px >= WRAPWIDTH) && (zx < MAXSPRITEX))
     return (zx+REALWIDTH)-px;
  else
  return abs(zx-px);
}

int righter(int px,int zx) {
// returns 1 if px is to the right of zx
  if ((zx % 320 >= WRAPWIDTH) && (px % 320 < MAXSPRITEX)) return 1;
  else
  if ((px % 320 >= WRAPWIDTH) && (zx % 320 < MAXSPRITEX)) return 0;
  else
  if ((zx < MAXSPRITEX) && (px < REALWIDTH) && ((px+MAXSPRITEX) % 320 < MAXSPRITEX))
     return 0;
  else
  if ((px < MAXSPRITEX) && (zx < REALWIDTH) && ((zx+MAXSPRITEX) % 320 < MAXSPRITEX))
     return 1;
  else return (px > zx);
}

void animate();

void redrawpanel() {
  int x,p;

  for (x=0;x<2;x++)
  {
    drawpana();
    for (p=0;p<4;p++)
      {
	drawfont(players[p].num,80*p+41,210);
	if ((!odd(p)) && (players[p].menuon))
	{
	  if (!players[p].playeron) drawicon(RIPICON,80*p,200); else
	     if (!players[p].invis) drawicon(players[p].birdtype,80*p,200);
	}
	else
	if (!players[p].playeron) drawicon(RIPICON,80*p,200); else
	   if (!players[p].invis) drawiconback(players[p].birdtype,80*p,200);
       }
    curpage=!curpage;
    setActivePage(!curpage);
    setVisiblePage(curpage);
  }
}

void button(int plats) {
  char effect;
  int loopy,lop;
  unsigned char coopt;

  if (!platforms[plats].length)
  for (loopy=0;loopy<20;loopy+=2)   {
   effect = platforms[plats].extraeffects[loopy];
   coopt = platforms[plats].extraeffects[loopy+1];
   switch (effect) {
    case 1 :  if (coopt == 255)
	      for (lop=0;lop<platformnum;lop++)
		platforms[lop].type = 0;
	      else {
		platforms[coopt].type = 0;
		platforms[coopt+1].type = 0;
		platforms[coopt+2].type = 0;
	      }
	      break;
    case 2 :  for (lop=0;lop<numplayers;lop++)
		if ((players[lop].menuon) && (!players[lop].isgoal))
		  players[lop].playeron = 1;
	      gameover = 0;
	      redrawpanel();
	      break;
    case 3 :  if (coopt == 255)
	      for (lop=0;lop<platformnum;lop++)
	      if (platforms[lop].effecttype == CONVEYOR)
		platforms[lop].xveloc *= -1;
	      else
	      if (platforms[lop].effecttype == CONVEYOR) {
		platforms[coopt].xveloc *= -1;
		platforms[coopt+1].xveloc *= -1;
		platforms[coopt+2].xveloc *= -1;
	      }
	      break;
    case 4 : for (lop=0;lop<numplayers;lop++)
	     if (players[lop].playeron) {
		players[lop].birdtype = random(NUMMOUNTS);
		sprite[lop*3+1].n = 3+(7*players[lop].birdtype);
		sprite[lop*3+3].n = 5+sprite[lop*3+1].n;
	     }
	     redrawpanel();
	     break;
    case 5 : for (lop=0;lop<numplayers;lop++)
	     if (players[lop].playeron) players[lop].unharm = 50;
	     break;
    case 6 :  if (coopt == 255)
	      for (lop=0;lop<platformnum;lop++)
		platforms[lop].type = platforms[lop].temptype;
	      else {
		platforms[coopt].type = platforms[coopt].temptype;
		platforms[coopt+1].type = platforms[coopt+1].temptype;
		platforms[coopt+2].type = platforms[coopt+2].temptype;
	      }
	      break;
    case 7 :  if (coopt == 255)
	      for (lop=0;lop<platformnum;lop++) platforms[lop].xveloc = 0;
	      else {
		platforms[coopt].xveloc = 0;
		platforms[coopt+1].xveloc = 0;
		platforms[coopt+2].xveloc = 0;
	      }
	      break;
    case 8 :  if (coopt == 255)
	      for (lop=0;lop<platformnum;lop++) platforms[lop].yveloc = 0;
	      else {
		platforms[coopt].yveloc = 0;
		platforms[coopt+1].yveloc = 0;
		platforms[coopt+2].yveloc = 0;
	      }
	      break;
    case 9 : for (lop=0;lop<numplayers;lop++)
	     if (players[lop].isgoal) players[lop].playeron = 0;
	     break;
    case 10 : for (lop=0;lop<numplayers;lop++) if (players[lop].isgoal)
		players[lop].playeron = 1;
	     break;
    case 11 : if (coopt == 255) {
	         for (lop=0;lop<platformnum;lop++)
                   if (!platforms[lop].type)
  		         platforms[lop].type = platforms[lop].temptype;
                   else  platforms[lop].type = 0;
                }
    	      else {
    	      if (platforms[coopt].type) {
		platforms[coopt].type = 0;
		platforms[coopt+1].type = 0;
		platforms[coopt+2].type = 0;
	      }
	      else {
		platforms[coopt].type = platforms[coopt].temptype;
		platforms[coopt+1].type = platforms[coopt+1].temptype;
		platforms[coopt+2].type = platforms[coopt+2].temptype;
              }
      	   }
	      break;
    case 12 : if (coopt == 255)
                for (lop=0;lop<MAXTEXTS;lop++)
                 texton[lop] = 0;
    	      else
    		 texton[coopt] = 0;
	      break;
    case 13 : if (coopt == 255)
                for (lop=0;lop<MAXTEXTS;lop++)
                 texton[lop] = 1;
    	      else
    		 texton[coopt] = 1;
	      break;
    case 14 : if (coopt == 255)
                for (lop=0;lop<MAXTEXTS;lop++)
                 texton[lop] = !texton[lop];
    	      else
    		 texton[coopt] = !texton[coopt];
	      break;
    case 15 : if (coopt == 255)
              {
                 for (lop=0;lop<platformnum;lop++)
                   if (lop != plats)
                   {
                    if (!((platforms[lop].effecttype == BUTT) &&
                        (platforms[coopt].length)))
                            loopy = 50;
                   }
              }
              else
              if (!((platforms[coopt].effecttype == BUTT) &&
                 (platforms[coopt].length)))
                   loopy = 50;
  }
 }
 platforms[plats].length = 10;
}


void animate()
{
  unsigned int x,z,p,y,t,q,znum,pnum;
  char oc=0;
  float massz,massp,vinitp,vinitz;

  curpage = !curpage;
  setActivePage(!curpage);
  keychek();
  backdraw();
  keychek();
  oc = fontfore;
  for (z=0;z<numtexts;z++) {
    fontfore = textsc[z];
    if (texton[z]) writetextxy(texts[z],textsx[z],textsy[z]);
   }
  fontfore = oc;
  for (z=0;z<numplayers;z++)
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
	       if (platforms[q].type)
	       for (t=0;t<MAXMINES;t++)
		if (players[z].mineon[t])
		if ((platforms[q].effecttype == BUTT) ||
		    (platforms[q].effecttype == SHOOTABLE) ||
		    (platforms[q].effecttype == UNPASS) ||
		    (platforms[q].effecttype == FIRE))
		if ((((distancex(players[z].minex[t],platforms[q].startx) <=
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
			      button(q);
			      players[z].mineon[t] = 0;
			   }
			   else
			   if (platforms[q].effecttype == SHOOTABLE) {
			      players[z].mineon[t] = 0;
					platforms[q-(q%3)].type = 0;
					platforms[q-(q%3)+1].type = 0;
					platforms[q-(q%3)+2].type = 0;
					platforms[q-(q%3)].temptype = 0;
					platforms[q-(q%3)+1].temptype = 0;
					platforms[q-(q%3)+2].temptype = 0;
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
	   if (shownum) {
		drawfont(players[z].num,sprite[x].x+5,sprite[x].y-7);
		keychek(); }
		if (players[z].armored)
		  drawnorm2(LIGHTSPRITE+curpage,sprite[x].x,sprite[x].y);
		drawnorm(x);
		keychek();
		drawnorm2(RIDERSPRITE,sprite[x].x,sprite[x].y);
		keychek();
		drawnorm(x+2);
		keychek();
		if (players[z].unharm)
		  drawnorm2(UNHARMSPRITE+curpage,sprite[x].x,sprite[x].y);
		if (players[z].stunned)
		  drawnorm3(CONFSPRITE+curpage,sprite[x].x,sprite[x].y - 4,3);
		}
	else {
	  if (shownum) {
		drawfont(players[z].num,sprite[x].x+5,sprite[x].y-7);
		keychek(); }
		if (players[z].armored)
		  drawnorm2(LIGHTSPRITE+curpage,sprite[x].x,sprite[x].y);
		drawback(x);
		keychek();
		drawback(x+1);
		keychek();
		drawback(x+2);
		keychek();
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
     if (players[p].armored != players[z].armored) {
     if (players[p].armored) {
	if (!players[z].unharm)
	  kill(z);
     }
     else if (!players[p].unharm) kill(p);
     }
     else
     if ((sprite[pnum].y < sprite[znum].y) && (!players[z].unharm))
      kill(z);
     else
     if ((sprite[znum].y < sprite[pnum].y) && (!players[p].unharm))
       kill(p);
      else
      if ((sprite[znum].y == sprite[pnum].y) && (players[p].dir != players[z].dir))
      {
	   massz = masses[players[z].birdtype];
	   massp = masses[players[p].birdtype];
	   vinitp = players[p].xveloc;
	   if (massz == massp) {
	       players[p].xveloc = players[z].xveloc;
	       players[z].xveloc = vinitp;
	     }
	   else {
	     vinitz = players[z].xveloc;
	     players[p].xveloc = (int) ((massp*vinitp)+(2*(massz*vinitz))-
				     (massz*vinitp))/(massp+massz);
	     players[z].xveloc = players[p].xveloc + vinitp - vinitz;
	   }
	   if (righter(sprite[pnum].x,sprite[znum].x))
	     sprite[pnum].x = (sprite[znum].x+MAXSPRITEX) % 320;
		if (distancex(sprite[znum].x,sprite[pnum].x) < MAXSPRITEX);
	   sprite[pnum].x+=players[p].xveloc;
	   sprite[znum].x+=players[z].xveloc;
	}
       else
      if ((sprite[znum].y == sprite[pnum].y) && (players[p].dir == players[z].dir)) {
	  if (((!players[z].dir) && (!righter(sprite[znum].x,sprite[pnum].x))) ||
	       ((players[z].dir) && (righter(sprite[znum].x,sprite[pnum].x))))
	  if (!players[z].unharm)
		kill(z);
	  else
	   if (!players[p].unharm)
	      kill(z);
	  }
	 }
	}
      }
    }
  }
  keychek();
  if (waitforvr) waitvr();
  setVisiblePage(!curpage);
}

void reviseplats()
{
  int x;
  for (x=0;x<platformnum;x++)
   {
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
     platforms[x].startx %= REALWIDTH;
     platforms[x].starty %= 192;
     platforms[x].count++;
     if ((platforms[x].cycletype & 4 == 4) || (!platforms[x].numcycles))
     {
     if (odd(platforms[x].cycletype >> 1))
	  {
	   if (platforms[x].count == platforms[x].offcycles)
	    {
	     platforms[x].count = 0;
	     if (platforms[x].numcycles > 0) platforms[x].numcycles--;
	     platforms[x].temptype = platforms[x].type;
	     platforms[x].type = 0;
	     if (platforms[x].cycletype & 2 == 2)
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
       }
   }
}

int inrange(int px,int startx,int endx) {
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

void ashify(int playernum) {
    if (!players[playernum].unharm) {
      animate();
      players[playernum].playeron = 0;
      if (players[playernum].isgoal) players[playernum].isgoal = 0;
      else gameover++;
      setVisiblePage(!curpage);
    }
}

void special2(int p,int plats) {
  char effect;

  effect = platforms[plats].fieldtype & 0x7F;
  if (effect == 1) effect = random(6)+2;
  switch (effect) {
    case 2 : sprite[p*3+1].y = random(184);
	     sprite[p*3+2].y = sprite[p*3+1].y;
	     sprite[p*3+3].y = sprite[p*3+1].y;
	     sprite[p*3+1].x = random(REALWIDTH);
	     sprite[p*3+2].x = sprite[p*3+1].x;
	     sprite[p*3+3].x = sprite[p*3+1].x;
	     break;
    case 3 : players[p].invis = 50;
	     redrawpanel();
	     break;
    case 4 : players[p].birdtype = random(NUMMOUNTS);
	     sprite[p*3+1].n = 3+(7*players[p].birdtype);
	     sprite[p*3+3].n = 5+sprite[p*3+1].n;
	     redrawpanel();
	     break;
    case 5 : players[p].armored = 40; break;
    case 6 : players[p].stunned = 30; break;
    case 7 : players[p].unharm = 100; break;
    case 8 : kill(p); break;
  }
  if ((platforms[plats].fieldtype & 128) == 128)
   {
    platforms[plats].fieldtype = 0;
    if (!(plats % 3))
    if (platforms[plats].type & 1 == 1)
      {
	platforms[plats+1].type = 0;
	platforms[plats+2].type = 0;
      }
    else
      {
	platforms[plats-1].type = 0;
	platforms[plats-2].type = 0;
      }
    platforms[plats].type = 0;
   }
}

void platformscan(int playernum)
{
 char plats,spritenum;
 int tempn,b,c,range;

 spritenum=playernum*3+1;
 players[playernum].plat = 0;
 for (plats=0;plats<platformnum;plats++)
  {
	 if (platforms[plats].type & 1)
	 {
		range = effectranges[platforms[plats].effecttype];
		tempn = platforms[plats].startx+((platforms[plats].length)*MAXSPRITEX);
		b = sprite[spritenum].x;
		c = (b+MAXSPRITEX);
		if (inrange(b,platforms[plats].startx,tempn))
		{
		if ((sprite[spritenum].y+MAXSPRITEY == platforms[plats].starty) ||
	 ((sprite[spritenum].y < platforms[plats].starty) &&
	  (sprite[spritenum].y+MAXSPRITEY > platforms[plats].starty)) ||
	  ((sprite[spritenum].y < platforms[plats].starty) &&
	  (sprite[spritenum].y+MAXSPRITEY+players[playernum].yveloc > platforms[plats].starty))) // top
	  {
	  if ((platforms[plats].type & 4) || (platforms[plats].effecttype == BUTT)
		 || (platforms[plats].effecttype == SHOOTABLE))
	  {
			if (platforms[plats].effecttype == FIRE) ashify(playernum); else
			if (platforms[plats].effecttype == FORCEFIELD) special2(playernum,plats); else
			if ((platforms[plats].effecttype == BUTT) && (platforms[plats].type)) button(plats); else
			{
		players[playernum].plat=plats+1;
		sprite[spritenum].y = platforms[plats].starty-MAXSPRITEX;
		sprite[spritenum+1].y = sprite[spritenum].y;
		sprite[spritenum+2].y = sprite[spritenum].y;
		plats += 3-(plats % 3);
			}
	    }
	  }
       else
       if (((sprite[spritenum].y >= platforms[plats].starty) && // bot
	   (sprite[spritenum].y < platforms[plats].starty+range)) ||
	   ((sprite[spritenum].y >= platforms[plats].starty) &&
	   (sprite[spritenum].y+players[playernum].yveloc < platforms[plats].starty+range)))
	   {
	   if (platforms[plats].type & 8)
	   {
	     if (platforms[plats].effecttype == FIRE) ashify(playernum); else
	     if (platforms[plats].effecttype == FORCEFIELD) special2(playernum,plats); else
	     if (platforms[plats].effecttype == BUTT) button(plats); else
	     {
	      if (players[playernum].yveloc < 0) players[playernum].yveloc*=-1;
	      sprite[spritenum].y = platforms[plats].starty+range+2;
	      sprite[spritenum+1].y = sprite[spritenum].y;
	      sprite[spritenum+2].y = sprite[spritenum].y;
	      plats += 3-(plats % 3);
	     }
	   }
	 }
     }
    }
    else if (platforms[plats].type & 2) // vertical
    if (distancex(platforms[plats].startx,sprite[spritenum].x) <= 16)
    {
      tempn = platforms[plats].starty+((platforms[plats].length-1)*
		effectrangesy[platforms[plats].effecttype])+
		effectranges[platforms[plats].effecttype];
      b = sprite[spritenum].y;
      c = (b+MAXSPRITEY);
      if ((c >= platforms[plats].starty) && (b <= tempn))
      {
      if ((!righter(sprite[spritenum].x,platforms[plats].startx) &&
	  righter(sprite[spritenum].x+MAXSPRITEX,platforms[plats].startx)) ||
	  (!righter(sprite[spritenum].x,platforms[plats].startx) &&
	  righter(sprite[spritenum].x+MAXSPRITEX+players[playernum].xveloc,platforms[plats].startx))) // top
	  {
	    if (platforms[plats].type & 4)
	    {
	      if (platforms[plats].effecttype == FIRE) ashify(playernum); else
	      if (platforms[plats].effecttype == FORCEFIELD) special2(playernum,plats); else
	      if (platforms[plats].effecttype == BUTT) button(plats); else
	      {
	      players[playernum].xveloc = (-players[playernum].xveloc) / 3;
	      sprite[spritenum].x = platforms[plats].startx-MAXSPRITEX+players[playernum].xveloc;
	      sprite[spritenum+1].x = sprite[spritenum].x;
	      sprite[spritenum+2].x = sprite[spritenum].x;
	      plats += 3-(plats % 3);
	      }
	    }
	 }
       else
       if ((righter(sprite[spritenum].x,platforms[plats].startx) && //bot
	   (!righter(sprite[spritenum].x,platforms[plats].startx+MAXSPRITEX))) ||
	   ((righter(sprite[spritenum].x,platforms[plats].startx)) &&
	   (!righter(sprite[spritenum].x+players[playernum].xveloc,platforms[plats].startx+MAXSPRITEX))))
	 {
	 if (platforms[plats].type & 8)
	 {
	     if (platforms[plats].effecttype == FIRE) ashify(playernum); else
	     if (platforms[plats].effecttype == FORCEFIELD) special2(playernum,plats); else
	     if (platforms[plats].effecttype == BUTT) button(plats); else
	     {
	     players[playernum].xveloc = (-players[playernum].xveloc / 3);
	     sprite[spritenum].x = platforms[plats].startx+MAXSPRITEX+players[playernum].xveloc;
	     sprite[spritenum+1].x = sprite[spritenum].x;
	     sprite[spritenum+2].x = sprite[spritenum].x;
	     plats += 3-(plats % 3);
	     }
	   }
	 }
     }
    }
  }
}

void displayimage()
{
 char plane;

 setActivePage(2);
 for (plane = 0; plane < 4; plane++)
 {
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 1 << plane);
  movedata(imageseg,imageoff+16000*plane,0xA000,actStart,16000);
 }
 setVisiblePage(2);
 while (!kbhit());
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
     if (uho) players[p].unharm = 100;
      else players[p].unharm = 0;
     players[p].isgoal = 0;
     players[p].shot = 0;
     players[p].mine = 0;
     players[p].armored=0;
     players[p].plat = 0;
     players[p].heightaim = 70;

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
  loadkeys();
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
  for (p=0;p<numplayers;p++)
   {
     if (!players[p].isgoal)
     {
       sprite[p*3+2].x = sprite[p*3+1].x;
       sprite[p*3+2].y = sprite[p*3+1].y;
       sprite[p*3+3].x = sprite[p*3+1].x;
       sprite[p*3+3].y = sprite[p*3+1].y;
     }
   }
}



void findnumbers()
{
  int p;

  gameplayers = 0;
  for (p=0;p<numplayers;p++)
   if (players[p].menuon)
   gameplayers++;
  gameplayers--;
  computerplayers = 0;
  livingplayers = 0;
  for (p=0;p<numplayers;p++)
   {
    if (players[p].menuon)
      {
	livingplayers++;
	if (players[p].compu) computerplayers++;
      }
    players[p].playeron = players[p].menuon;
   }
}

void optionsmenu()
{
   unsigned int x,y,p;
   char key;
   char mass[5][3],accx[5][3],accy[5][3],maxx[5][3],maxy[5][3],wins[5][3];
   char gspeed[6];
   char roundses[3];

  top:;
  setActivePage(!curpage);
  fontbackon = 1;
  fontback = 0;
  outportb(0x3c4, 0x02);
  outportb(0x3c5, 15);
  memset(vga+actStart,0,19200);
  for (p=0;p<4;p++)
  {
    itoa(masses[players[p].birdtype],mass[p],10);
    itoa(accelx[players[p].birdtype],accx[p],10);
    itoa(accely[players[p].birdtype],accy[p],10);
    itoa(maxspeedx[players[p].birdtype],maxx[p],10);
    itoa(maxspeedy[players[p].birdtype],maxy[p],10);
    itoa(players[p].wins,wins[p],10);
  }
  itoa(rounds,roundses,10);
  itoa(gamedelay,gspeed,10);
  fontfore = hiliteoff;
  writetextxy("-= @OPTIONS@ =-",0,0);
  writetextxy2("@Q@UIT",299,230);
  writetextxy2("ROUNDS SO FAR: ",120,0);
  fontfore = hilite;
  writetextxy2(roundses,190,0);
  fontfore = hiliteoff;
  writetextxy("@R@ESET WINS",0,200);
  writetextxy("@P@LATFORMS ",0,208);
  writetextxy((platson ? "@ON@  " : "@OFF@  "),60,208);
  writetextxy("@S@PECIAL ABILITIES",0,216);
  writetextxy((specon ? "@ON@  " : "@OFF@  "),109,216);
  writetextxy("SHOW PLAYER @N@UMBERS",0,224);
  writetextxy((shownum ? "@ON@  " : "@OFF@  "),122,224);
  writetextxy("@V@ERT RETRACE WAIT",160,200);
  writetextxy((waitforvr ? "@ON@ " : "@OFF@"),270,200);
  writetextxy("GAME DELAY @+@ or @-@",160,208);
  writetextxy(gspeed,270,208);
  writetextxy("@U@NHARMABLE OPENING",160,216);
  writetextxy((uho ? "@ON@ " : "@OFF@"),270,216);
  writetextxy("@H@ALF-SPEED GRAVITY",160,224);
  writetextxy((halfgrav ? "@ON@ " : "@OFF@"),270,224);
  writetextxy("MOUNT  # @1@:",0,8);
  writetextxy("MOUNT  # @2@:",0,53);
  writetextxy("MOUNT  # @3@:",0,98);
  writetextxy("MOUNT  # @4@:",0,143);
  writetextxy("@SPACE@ TO BEGIN",0,232);
  writetextxy2("MASS:   ACCELERATION-X:   Y:   MAX SPEED-X:    Y: ",0,16);
  writetextxy2("MASS:   ACCELERATION-X:   Y:   MAX SPEED-X:    Y: ",0,61);
  writetextxy2("MASS:   ACCELERATION-X:   Y:   MAX SPEED-X:    Y: ",0,106);
  writetextxy2("MASS:   ACCELERATION-X:   Y:   MAX SPEED-X:    Y: ",0,151);
  writetextxy2("SPECIAL @A@BILITY: ",0,24);
  writetextxy2("SPECIAL A@B@ILITY: ",0,69);
  writetextxy2("SPECIAL AB@I@LITY: ",0,114);
  writetextxy2("SPECIAL ABI@L@ITY: ",0,159);
  writetextxy(animals[players[0].birdtype],0,32);
  writetextxy(animals[players[1].birdtype],0,77);
  writetextxy(animals[players[2].birdtype],0,122);
  writetextxy(animals[players[3].birdtype],0,167);
  writetextxy(abilities[players[0].specnum],100,24);
  writetextxy(abilities[players[1].specnum],100,69);
  writetextxy(abilities[players[2].specnum],100,114);
  writetextxy(abilities[players[3].specnum],100,159);
  *(int far *) MK_FP(0x40,0x1A) = *(int far *) MK_FP(0x40,0x1C);
  key = 0;
  setVisiblePage(!curpage);
  while ((key != ' ') && (!gameend))
   {
       x++;
       y = (x % 5)+3;

       for (p=0;p<4;p++)
	{
	  fontfore = hilite;
	  writetextxy2(mass[p],30,16+p*45);
	  writetextxy2(accx[p],114,16+p*45);
	  writetextxy2(accy[p],140,16+p*45);
	  writetextxy2(maxx[p],216,16+p*45);
	  writetextxy2(maxy[p],250,16+p*45);
	  fontfore = hiliteoff;
	  if ((players[p].menuon) && (players[p].birdtype != NUMMOUNTS))
	  {
	   drawtile(y+(7*players[p].birdtype),300,8+45*p);
	   drawnorm2(RIDERSPRITE,300,8+45*p);
	   drawnorm2(random(2)+8+(7*players[p].birdtype),300,8+45*p);
	  }
	  else
	  if ((!players[p].menuon) || (players[p].birdtype == NUMMOUNTS))
	   drawtile(OFFSPRITE,300,8+45*p);
	  writetextxy(((players[p].compu) ? "@COMPUTER@" : "        "),80,8+45*p);
	  writetextxy(((players[p].menuon) ? "     " : "@OFF@"),140,8+45*p);
	  writetextxy("WINS:", 170,8+45*p);
	  writetextxy(wins[p],206,8+45*p);
	}
       if (kbhit()) key = getch(); else key = 0;
       switch (toupper(key)) {
	 case '1' : players[0].birdtype++;
		    players[0].menuon = 1;
		    if (players[0].birdtype == NUMMOUNTS)
			players[0].menuon = 0;
		    else
		    if (players[0].birdtype > NUMMOUNTS)
		      {
			players[0].birdtype = 0;
			players[0].compu = !players[0].compu;
		      }
		    itoa(masses[players[0].birdtype],mass[0],10);
		    itoa(accelx[players[0].birdtype],accx[0],10);
		    itoa(accely[players[0].birdtype],accy[0],10);
		    itoa(maxspeedx[players[0].birdtype],maxx[0],10);
		    itoa(maxspeedy[players[0].birdtype],maxy[0],10);
		    writetextxy(animals[players[0].birdtype],0,32);
		    break;
	 case '2' : players[1].birdtype++;
		    players[1].menuon = 1;
		    if (players[1].birdtype == NUMMOUNTS)
			players[1].menuon = 0;
		    else if (players[1].birdtype > NUMMOUNTS) {
			players[1].birdtype = 0;
			players[1].compu = !players[1].compu;
		      }
		    itoa(masses[players[1].birdtype],mass[1],10);
		    itoa(accelx[players[1].birdtype],accx[1],10);
		    itoa(accely[players[1].birdtype],accy[1],10);
		    itoa(maxspeedx[players[1].birdtype],maxx[1],10);
		    itoa(maxspeedy[players[1].birdtype],maxy[1],10);
		    writetextxy(animals[players[1].birdtype],0,77);
		    break;
	 case '3' : players[2].birdtype++;
		    players[2].menuon = 1;
		    if (players[2].birdtype == NUMMOUNTS)
			players[2].menuon = 0;
		    else
		    if (players[2].birdtype > NUMMOUNTS) {
			players[2].birdtype = 0;
			players[2].compu = !players[2].compu;
		      }
		    itoa(masses[players[2].birdtype],mass[2],10);
		    itoa(accelx[players[2].birdtype],accx[2],10);
		    itoa(accely[players[2].birdtype],accy[2],10);
		    itoa(maxspeedx[players[2].birdtype],maxx[2],10);
		    itoa(maxspeedy[players[2].birdtype],maxy[2],10);
		    writetextxy(animals[players[2].birdtype],0,122);
		    break;
	 case '4' : players[3].birdtype++;
		    players[3].menuon = 1;
		    if (players[3].birdtype == NUMMOUNTS)
			players[3].menuon = 0;
		    else
		    if (players[3].birdtype > NUMMOUNTS)
		      {
		       players[3].birdtype = 0;
		       players[3].compu = !players[3].compu;
		      }
		    itoa(masses[players[3].birdtype],mass[3],10);
		    itoa(accelx[players[3].birdtype],accx[3],10);
		    itoa(accely[players[3].birdtype],accy[3],10);
		    itoa(maxspeedx[players[3].birdtype],maxx[3],10);
		    itoa(maxspeedy[players[3].birdtype],maxy[3],10);
		    writetextxy(animals[players[3].birdtype],0,167);
		    break;
	 case 'S' : specon = !specon;
		    writetextxy((specon ? "@ON@  " : "@OFF@  "),109,216);
		    break;
	 case 'N' : shownum = !shownum;
		    writetextxy((shownum ? "@ON@  " : "@OFF@  "),122,224);
		    break;
	 case 'A' : players[0].specnum++;
		    if (players[0].specnum > spechigh) players[0].specnum = 0;
		    writetextxy(abilities[players[0].specnum],100,24);
		    break;
	 case 'B' : players[1].specnum++;
		    if (players[1].specnum > spechigh) players[1].specnum = 0;
		    writetextxy(abilities[players[1].specnum],100,69);
		    break;
	 case 'I' : players[2].specnum++;
		    if (players[2].specnum > spechigh) players[2].specnum = 0;
		    writetextxy(abilities[players[2].specnum],100,114);
		    break;
	 case 'L' : players[3].specnum++;
		    if (players[3].specnum > spechigh) players[3].specnum = 0;
		    writetextxy(abilities[players[3].specnum],100,159);
		    break;
	 case 'P' : platson = !platson;
		    writetextxy((platson ? "@ON@  " : "@OFF@  "),60,208);
		    break;
	 case 'R' : for (p = 0; p< 4;p++)
			{
			  players[p].wins = 0;
			  itoa(players[p].wins,wins[p],10);
			}
		    rounds = 0;
		    writetextxy2("ROUNDS SO FAR: ",120,0);
		    fontfore = hilite;
		    itoa(rounds,roundses,10);
		    writetextxy2("    ",190,0);
		    writetextxy2(roundses,190,0);
		    fontfore = hiliteoff;
		    break;
	 case 'V' : waitforvr = !waitforvr;
		    writetextxy((waitforvr ? "@ON@ " : "@OFF@"),270,200);
		    break;
	 case '+' : if (gamedelay == 10) dinc = 10; else
		    if (gamedelay == 100) dinc = 100; else
		    if (gamedelay == 1000) dinc = 1000;
		    if (gamedelay < 1999) gamedelay+=dinc;
		    itoa(gamedelay,gspeed,10);
		    fontback=0;
		    writetextxy("      ",270,208);
		    writetextxy(gspeed,270,208);
		    break;
	 case '-' : if (gamedelay == 10) dinc = 1;
		    if (gamedelay == 100) dinc = 10; else
		    if (gamedelay == 1000) dinc = 100;
		    if (gamedelay) gamedelay-=dinc;
		    itoa(gamedelay,gspeed,10);
		    writetextxy("      ",270,208);
		    writetextxy(gspeed,270,208);
		    break;
	 case 'U' : uho = !uho;
		    writetextxy((uho ? "@ON@ " : "@OFF@"),270,216);
		    break;
	 case 'H' : halfgrav = !halfgrav;
		    writetextxy((halfgrav ? "@ON@ " : "@OFF@"),270,224);
		    break;
	 case 'Q' : gameend = 1;
       }
   }
  fontbackon = 0;
  findnumbers();
  if (gameplayers <= 0) goto top;
}

void keychek()
{
  int p;

  keypress2 = inp(0x60);
  if (keypress2 != keypress) {
  keypress = keypress2;
  for (p=0;p<numplayers;p++)
  if (((players[p].playeron) && (!players[p].isgoal)) &&
	((!players[p].compu) && (!players[p].stunned)))
     {
     if ((keypress == players[p].speckey) && (!players[p].skey) && (specon))
	 {
	   players[p].skey = 1;
	   if (!((players[p].specnum == 9) && (players[p].curshot == MAXMINES)))
	     players[p].stime = specrate[players[p].specnum];
	 } else
     if (keypress == players[p].leftkey)
	 { players[p].lkey = 1;  players[p].rkey = 0;} else
     if (keypress == players[p].rightkey)
	 { players[p].rkey = 1;  players[p].lkey = 0;} else
     if (keypress == players[p].upkey)
	 { players[p].ukey = 1; players[p].upkeyon = UPPRESS;} else
     if (keypress == players[p].nrightkey) players[p].rkey = 0; else
     if (keypress == players[p].nupkey) players[p].ukey = 0; else
     if (keypress == players[p].nleftkey) players[p].lkey = 0;
   }
 }
 *(int far *) MK_FP(0x40,0x1A) = *(int far *) MK_FP(0x40,0x1C);
}

void specialize(int p)
{
 int d;
 switch (players[p].specnum) {
   case 0 : break;
   case 1 : for (d=0;d<numplayers;d++)
		if ((players[d].playeron) && (d != p))
		players[d].stunned = 20;
		break;
   case 2 : players[p].armored = 20; break;
   case 3 : for (d=0;d<numplayers;d++)
		if ((players[d].playeron) && (d != p))
		{
	    sprite[d*3+1].y += 15;
	    sprite[d*3+2].y += 15;
	    sprite[d*3+3].y += 15;
	    }
	    break;
   case 4 : players[p].birdtype = random(NUMMOUNTS);
	    sprite[p*3+1].n = 3+(7*players[p].birdtype);
	    sprite[p*3+3].n = 5+sprite[p*3+1].n;
	    redrawpanel();
	    break;
   case 5 : players[p].invis = 40;
	    redrawpanel();
	    break;
   case 6 : players[p].unharm = 40;
	    break;
   case 7 : sprite[p*3+1].y = random(184);
	    sprite[p*3+2].y = sprite[p*3+1].y;
	    sprite[p*3+3].y = sprite[p*3+1].y;
	    sprite[p*3+1].x = random(REALWIDTH);
	    sprite[p*3+2].x = sprite[p*3+1].x;
	    sprite[p*3+3].x = sprite[p*3+1].x;
	    break;
   case 8 : if (!players[p].mineon[players[p].curshot])
	    {
	    if (players[p].dir) players[p].shotdir[players[p].curshot] = 1;
		else players[p].shotdir[players[p].curshot] = -1;
	    players[p].shot = 1;
	    players[p].mineon[players[p].curshot] = 1;
	    players[p].miney[players[p].curshot] = sprite[p*3+1].y + 4;
	    players[p].minex[players[p].curshot] = sprite[p*3+1].x+
              (MAXSPRITEX*players[p].dir);
            players[p].curshot++;
            if (players[p].curshot == MAXMINES) players[p].curshot = 0;
            }
            break;
   case 9 : if (players[p].curshot <= MAXMINES)
                {
                  players[p].mine = 1;
                  players[p].mineon[players[p].curshot] = 1;
		  players[p].minex[players[p].curshot] = sprite[p*3+1].x+5;
                  players[p].miney[players[p].curshot] = sprite[p*3+1].y+5;
                  players[p].curshot++;
		  if (players[p].curshot > MAXMINES) players[p].curshot = MAXMINES;
                } break;
  }
}

void loadlevel(char name[80])
{
  FILE *stream;
  int x,k,z;

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
      if (odd(platforms[x].cycletype))
	 if (!odd(platforms[x].cycletype >> 1)) /* 2 */ platforms[x].type = 0;
      if (!odd(platforms[x].type >> 6)) /* 64 */ platforms[x].type = 0;
//      if (platforms[x].effecttype == SHOOTABLE) platforms[x].length = 1;
    }
    if (numtexts) for (z=0;z<numtexts;z++)
       {
	fread(texts[z],sizeof(char),STRMAXLEN,stream);
	fread(&textsx[z],sizeof(textsx[z]),1,stream);
	fread(&textsy[z],sizeof(textsy[z]),1,stream);
	fread(&textsc[z],sizeof(textsc[z]),1,stream);
   fread(&texton[z],sizeof(char),1,stream);
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
      numplayers=5;
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
  else numplayers = 4;
}

void initplats()
{
  FILE *stream;
  int x;

  if (numlevels == 255)
  {
  if ((stream = fopen("levels.dat","r")) == NULL) printf("Level table nonexistant\n");
  else
  {
    fscanf(stream,"%d",&numlevels);
    if (numlevels > 100) numlevels = 100;
    for (x=0;x<numlevels;x++)
      fscanf(stream,"%s",levelnames[x]);
    fclose(stream);
    curlevel = -1;
   }
  }
  if (!redo) {
    curlevel++;
    if (curlevel == numlevels) curlevel = 0; }
  loadlevel(levelnames[curlevel]);
}

void showlevelinfo()
{
  int x,y;

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
  drawicon(players[winner].birdtype,100,120);
  drawiconback(players[winner].birdtype,179,120);
  writetextxy2("O = Options",110,106);
  writetextxy2("Q = quit, R = Replay",110,113);
  setVisiblePage(!curpage);
}

void loadpair(char wing[16], int snum) {
  int leng,tempy;
  char str[16];

  leng = strlen(wing);
  strcpy(str,wing);
  strcat(str,"1.stn");
  for (tempy=snum;tempy<snum+2;tempy++) {
      loadsprite(str,tempy);
	   str[leng]++;
    }
}

void loadstuffs(char pre[16],char wing[16], int snum) {
  char str[16];
  int tempy,leng;

  leng = strlen(pre);
  strcpy(str,pre);
  strcat(str,"1.stn");
  for (tempy=snum;tempy<snum+5;tempy++) {
      loadsprite(str,tempy);
	   str[leng]++;
    }
  leng = strlen(wing);
  strcpy(str,wing);
  strcat(str,"1.stn");
  for (tempy=snum+5;tempy<snum+7;tempy++) {
      loadsprite(str,tempy);
	   str[leng]++;
    }
}

int sqr(int a)
{
  return (a * a);
}

void main(int argc)
{
  int x=5,y=4,n,q,t;
  char p,z,snum,pnum,znum,winner,ch;

  printf("Hello");
  initmode();
  fontfore = hiliteoff;
  loadfont();
  loadpal("pal.col",pal);
  updatepal();
  imageseg = FP_SEG(image);
  imageoff = FP_OFF(image);
  asm cld;
  loadsprite("blank.stn",0);
  loadsprite("rider1.stn",1);
  loadsprite("goblet.stn",2);
  loadstuffs("par","parw",3);
  loadstuffs("hors","horsew",10);
  loadstuffs("buff","buffw",17);
  loadstuffs("moth","mothw",24);
  loadstuffs("bana","banaw",31);
  loadstuffs("cat","catw",38);
  loadstuffs("bike","bikew",45);
  loadstuffs("dino","dinow",52);
  loadstuffs("fish","fishw",59);
  loadstuffs("classic","classiw",66);
  loadpair("plat",PLATSPRITE);
  loadpair("light",LIGHTSPRITE);
  loadpair("mine",MINESPRITE);
  loadpair("unharm",UNHARMSPRITE);
  loadpair("blow",BLOWSPRITE);
  loadpair("conf",CONFSPRITE);
  loadsprite("off.stn",OFFSPRITE);
  loadpair("splatt",SPLATSPRITE);
  loadpair("fire",FIRESPRITE);
  loadsprite("fire3.stn",FIRESPRITE+2);
  loadsprite("fire4.stn",FIRESPRITE+3);
  loadpair("force",FORCELEFT);
  loadsprite("force3.stn",FORCEUP);
  loadsprite("force4.stn",FORCEUP+1);
  loadsprite("forcec.stn",FORCECORNER);
  loadpair("convl",CONVLEFT);
  loadpair("convm",CONVMID);
  loadpair("convr",CONVRIGHT);
  loadpair("butt",BUTTON);
  basesproff = FP_OFF(spritedata[0]);
  basesprseg = FP_SEG(spritedata[0]);
  fontfore = 55;
  if (argc == 1) {
    loadimage("logo.cut");
    setActivePage(2);
    writetextxy("SuperJoust v1.0 Copyright (C), 1994, Ben Chadwick",0,201);
    writetextxy("all rights reserved.",0,211);
    writetextxy2("DO NOT DISTRIBUTE!! Press any key to continue.",0,221);
    displayimage();
    setVisiblePage(curpage);
    while (!kbhit());
  }
  loadpanel();
  drawpan();
  loadicon("parrot.dvl",0);
  loadicon("horse.dvl",1);
  loadicon("buffalo.dvl",2);
  loadicon("moth.dvl",3);
  loadicon("banana.dvl",4);
  loadicon("cat.dvl",5);
  loadicon("bike.dvl",6);
  loadicon("dino.dvl",7);
  loadicon("fish.dvl",8);
  loadicon("classic.dvl",9);
  loadicon("rip.dvl",RIPICON);
  initplayall();
  starthere:;
  gameend = 0;
  optionsmenu();
  if (gameend) goto theend;
  starthere2:;
  initplayers();
  findnumbers();
  gameover = 0;
  initplayers();
  redrawpanel();
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
  initplats();
  initsprites();
  rounds++;
  keypress = 0;
  autowinner = -1;
  animate();
  showlevelinfo();
  while ((keypress != 1) && (gameover < gameplayers) && (computerplayers != livingplayers))  {
  keychek();
  for (p=0;p<numplayers;p++) {
   snum = p*3+1;
   if ((players[p].playeron) || (players[p].isgoal)) {
   pnum = p*3+3;
   if ((players[p].compu) && (!players[p].stunned)) {
	 if (!random(30)) players[p].rundir = !players[p].rundir;
	 else
	 if (goal) {
	     if (righter(goalx,sprite[pnum].x)) players[p].rundir = 1;
	     else players[p].rundir = 0;
	   }
	 if (!random(25)) players[p].heightaim = (10-sqr(1+random(3)))*10;
	 else
	 if ((goal) && (!random(10))) players[p].heightaim = goaly;
	 else
	 if (!random(45)) players[p].heightaim = sprite[((p+1)%4)*3+1].y-5;
	 if ((!players[p].upkeyon) && (sprite[pnum].y > players[p].heightaim))
		 players[p].ukey = 1;
	 else players[p].ukey = 0;
	 if (players[p].ukey) players[p].upkeyon = UPPRESS;
	 players[p].lkey =
	 players[p].rkey = 0;
	 if ((!random(5)) && (!players[p].rundir)) players[p].lkey = 1; else
	 if ((!random(5)) && (players[p].rundir)) players[p].rkey = 1;
	 if ((!random(30)) && (!players[p].skey) && (specon)) {
	   players[p].skey = 1;
	   players[p].stime = specrate[players[p].specnum];
	   specialize(p);
	 }
      }
   if (players[p].armored) players[p].armored--;
   if (players[p].invis) {
       players[p].invis--;
       if (!players[p].invis) redrawpanel();
     }
   if (players[p].unharm) players[p].unharm--;
   if (players[p].stunned) {
                  players[p].ukey =
		  players[p].lkey =
                  players[p].rkey = 0;
                  players[p].stunned--;
                  if (sprite[snum].y == GROUNDLEVEL) players[p].xveloc = 0;
		}
   if (players[p].skey) {
                 if (players[p].stime == specrate[players[p].specnum])
                    specialize(p);
                 players[p].stime--;
                 if (!players[p].stime) players[p].skey = 0;
               }
    if (players[p].ukey)
      if (players[p].upkeyon == UPPRESS)
	  players[p].yveloc -= accely[players[p].birdtype];
    if ((players[p].ukey) || ((players[p].rkey || players[p].lkey) &&
       ((!players[p].plat) && (sprite[snum].y != GROUNDLEVEL))))
	 sprite[pnum].n = 9+(7*players[p].birdtype);
    else sprite[pnum].n = 8+(7*players[p].birdtype);
    if (players[p].upkeyon) players[p].upkeyon--;
    if (platson) players[p].wasonplat = players[p].plat;
    if (platson) platformscan(p);
   if (players[p].lkey)
                {
                  if ((!players[p].xveloc) || ((!players[p].plat) &&
			(sprite[snum].y < GROUNDLEVEL)))
                    players[p].dir = 0;
		  if ((players[p].xveloc > 0) &&
                      (players[p].xveloc-accelx[players[p].birdtype] < 0))
		  players[p].xveloc = 0; else
                  players[p].xveloc-= accelx[players[p].birdtype];
                } else
   if (players[p].rkey)
       {
                  if ((!players[p].xveloc) || ((!players[p].plat) &&
                        (sprite[snum].y < GROUNDLEVEL)))
                  players[p].dir = 1;
		  if ((players[p].xveloc < 0) &&
		      (players[p].xveloc+accelx[players[p].birdtype] > 0))
                  players[p].xveloc = 0; else
                  players[p].xveloc+= accelx[players[p].birdtype];
		}
   else if ((!players[p].rkey) && (!players[p].lkey) && (!players[p].ukey))
     sprite[pnum].n = 8+(7*players[p].birdtype);
  }
  }
  for (p=0;p<numplayers;p++)
  {
   if ((players[p].playeron) || (players[p].isgoal))
   {
   snum = p*3+1;
   if (((sprite[snum].y != GROUNDLEVEL) && (!players[p].plat)) || (players[p].yveloc < 0))
     sprite[snum].y += players[p].yveloc;
   if (sprite[snum].y > GROUNDLEVEL) sprite[snum].y = GROUNDLEVEL;

   if (sprite[snum].y <= ROOFLEVEL) {
	  sprite[snum].y = ROOFLEVEL+3;
	  players[p].yveloc *= -1;
     }
   if ((sprite[snum].y == GROUNDLEVEL)
       && (players[p].yveloc > 0)) {
	 if (players[p].yveloc < 5) players[p].yveloc=0;
	 else players[p].yveloc=(-1*(players[p].yveloc) / 4);
       }
    else if (!players[p].plat)
	if (((!halfgrav) && (curpage)) ||
	     ((halfgrav) && (!(gameturn & 3))))
	     players[p].yveloc+=gravity;
   if ((players[p].wasonplat) && (!players[p].plat))
     if (platforms[players[p].wasonplat+1].effecttype == 4)
       players[p].xveloc += platforms[players[p].wasonplat+1].xveloc;
   sprite[snum].x += players[p].xveloc;
   if ((players[p].xveloc != 0) && ((sprite[snum].y == GROUNDLEVEL) || (players[p].plat)))
      sprite[snum].n =  y+(7*players[p].birdtype);
   if (players[p].plat) {
	  sprite[snum].x += platforms[players[p].plat-1].xveloc;
	  if (players[p].yveloc > 0) players[p].yveloc = 0;
	}
   if (players[p].yveloc > 24) players[p].yveloc = 24; else
   if (players[p].yveloc < -1*maxspeedy[players[p].birdtype]) players[p].yveloc = -1*maxspeedy[players[p].birdtype];
   if ((players[p].xveloc) > maxspeedx[players[p].birdtype]) players[p].xveloc = maxspeedx[players[p].birdtype]; else
   if ((players[p].xveloc) < -1*maxspeedx[players[p].birdtype]) players[p].xveloc = -1*maxspeedx[players[p].birdtype];
   if (sprite[snum].x < 0) sprite[snum].x += REALWIDTH; else
   if (sprite[snum].x > 319) sprite[snum].x -= REALWIDTH;
   sprite[snum+1].x =
   sprite[snum+2].x = sprite[snum].x;
   sprite[snum+1].y =
   sprite[snum+2].y = sprite[snum].y;
   }
   }
   for (p=0;p<numplayers;p++) if (players[p].playeron) {
   snum = p*3+1;
   if (players[p].shot) {
		 for (n=0;n<MAXMINES;n++) {
		  players[p].minex[n] += (players[p].shotdir[n] * SHOTSPEED);
		  if ((players[p].minex[n] < 0) || (players[p].minex[n] >= REALWIDTH))
		    players[p].mineon[n] = 0;
		  else
		  for (z=0;z<numplayers;z++) {
		     if ((players[z].playeron) || (players[z].isgoal))
		     if (z != p)
		     if (players[p].mineon[n]) {
		     znum = z*3+1;
		     if ((players[p].miney[n] > sprite[znum].y) &&
			(players[p].miney[n] < sprite[znum].y+MAXSPRITEY))
		     if ((inrange(players[p].minex[n],sprite[znum].x,
				sprite[znum].x+MAXSPRITEX)) ||
		     ((players[p].minex[n] < sprite[znum].x) &&
		     (players[p].minex[n]-(players[p].shotdir[n]*SHOTSPEED) >= sprite[znum].x)) ||
		     ((players[p].minex[n] > sprite[znum].x) &&
		     (players[p].minex[n]-(players[p].shotdir[n]*SHOTSPEED) <= sprite[znum].x)))
		     if ((!players[z].unharm) && (!players[z].armored)) {
				animate();
				players[p].mineon[n] = 0;
				drawnorm3(SPLATSPRITE+curpage,players[p].minex[n],players[p].miney[n],6);
				if (players[z].isgoal)
				   players[z].isgoal = 0;
				else kill(z);
				setVisiblePage(!curpage);
			   }
		     }
	     }
   }
   }
  if (players[p].mine){
	      for (q=0;q<platformnum;q++)
	       if (platforms[q].type & 3)
	       for (t=0;t<MAXMINES;t++)
		if (players[z].mineon[t])
		if ((platforms[q].effecttype == SHOOTABLE) ||
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
			   if (platforms[q].effecttype == SHOOTABLE) {
			      players[z].mineon[t] =
			      platforms[q].type = 0;
			      drawnorm3(BLOWSPRITE+curpage,players[p].minex[t],players[p].miney[t],10);
			   }
			   else
			   {
			     players[z].mineon[t] = 0;
			     drawnorm3(BLOWSPRITE+curpage,players[p].minex[t],players[p].miney[t],10);
			   }
			 }
		  for (n=0;n<MAXMINES;n++) {
		  for (z=0;z<numplayers;z++) {
		     if ((players[z].playeron) || (players[z].isgoal))
		     if (z != p)
		     if (players[p].mineon[n]) {
		     znum = z*3+1;
		     if ((players[p].minex[n]+2 > sprite[znum].x) &&
			 (players[p].minex[n]+2 < sprite[znum].x+MAXSPRITEX)&&
			 (players[p].miney[n]+2 > sprite[znum].y) &&
			 (players[p].miney[n]+2 < sprite[znum].y+MAXSPRITEY)){
			  if ((!players[z].unharm) && (!players[z].armored)){
				animate();
				players[p].mineon[n] = 0;
				drawnorm3(BLOWSPRITE+curpage,sprite[znum].x+((players[p].shotdir[n]+1)*8)-2,players[p].miney[n],10);
				if (players[z].isgoal) players[z].isgoal = 0;
				else kill(z);
			   }
			   else
			  if (players[z].armored)
			     players[p].mineon[n] = 0;
			}
		   }
		}
	    }
	 }
  }
  for (p=0;p<numplayers;p++) {
      if ((players[p].playeron) && ((players[p].plat) ||
		   (sprite[p*3+1].y == GROUNDLEVEL)))
      if ((players[p].xveloc>0) && (!(x & 4))) players[p].xveloc--; else
      if ((players[p].xveloc<0) && (!(x & 4))) players[p].xveloc++;
  }
  x++;
  gameturn++;
  y = (x % 5)+3;
  keychek();
  if (platson) reviseplats();
  keychek();
  if (gameover != oldgameover) {
      livingplayers = 0;
      computerplayers = 0;
      for (p=0;p<numplayers;p++)
       if (players[p].playeron)	{
	 livingplayers++;
	 if (players[p].compu) computerplayers++;
	}
      redrawpanel();
   }
  oldgameover = gameover;
  if (gamedelay) delay(gamedelay);
  animate();
  }
  redrawpanel();
  animate();
  if (autowinner == -1) {
    winner = numplayers+2;
    for (p=0;p<numplayers;p++)
      if (players[p].playeron) winner = p;
  }
  else winner = autowinner;
  clearkeybuf();
  players[winner].wins++;
  winnerbox(winner);
  clearkeybuf();
  ch = getch();
  clearkeybuf();
  if ((ch == 'q') || (ch == 'Q')) goto theend;
  if ((ch == 'o') || (ch == 'O')) goto starthere;
  redo = 0;
  if ((ch == 'r') || (ch == 'R')) redo = 1;
  gameover = 0;
  goto starthere2;
  theend:;
  normmode();
}
