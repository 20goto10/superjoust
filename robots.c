// Ben Chadwick's Techlab
// The Murderous Incestuous Cannibalistic Enigmatic robots (MICE)

#include <conio.h>
#include <ctype.h>
#include "modex.h"
#include "pal.h"
#include "font.h"
palette pal;

#define AGELIMIT 1000

#define CODELEN 40
#define WRAPWIDTH 304
unsigned int basesprseg,basesproff;

#define FLOOR 0
#define WALL 1
#define CHEESE 2
#define HILITE 3
#define ROBOFF 4

#define MAPX 10
#define MAPY 14

char map[MAPX][MAPY] =
         	{     2, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1,
			 1, 0, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 2  };

#define WAIT 0
#define IFWALL 1
#define IFMATE 2
#define IFFIGHT 3
#define IFCHEESE 4
#define TURNLEFT 5
#define TURNRIGHT 6
#define FORWARD 7
#define BACKWARD 8
#define SMELL 9
#define BURROW 10
#define SHAREFOOD 11
#define RESTART 12

unsigned long cycles=0;
char curpage=0;

#define MAXBOTS 400

struct robot_foo {
	char robotcode[CODELEN];  // code
	char xpos,ypos; // position x and y on map, screen = * Spritesize
	int codeptr;    // code pointer
	int dirptr;     // direction facing
	char gender;    // male or female
	char father;    // keeps the player from being overwritten when he dies
						 // if his code is still req'd
	char hunger;    // from bloated to starving
	int age;        // old=near death, young..
	char pregnant;  // if a female is carrying a child
	char kidsfather;// Who's the father
     char clan; // whether cats or rats
	char fight,mating; // If currently involved in one or the other
	char VAR_A,VAR_B,VAR_C,VAR_D; // variables
	char active;    // Is robot still alive, not yet born, or dead?
        char history[10]; // last 10 code pieces executed
} far robots[MAXBOTS];
int numrobots=8;
int monitor=0;

#define MAXSPRITEX 16
#define MAXSPRITEY 16

typedef char spritedata_t[MAXSPRITEX*MAXSPRITEY];
spritedata_t spritedata[20];

void box(int x1,int y1,int x2,int y2,int color) {
 int x;

 for (x=x1;x<=x2;x++) putpixel(x,y1,color);
 for (x=y1;x<=y2;x++) putpixel(x1,x,color);
 for (x=x1;x<=x2;x++) putpixel(x,y2,color);
 for (x=y1;x<=y2;x++) putpixel(x2,x,color);
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

void addtext(int bot, char stuff)
{
  int x;

  for (x=0;x<9;x++)
    robots[bot].history[x] = robots[bot].history[x+1];
  robots[bot].history[9] = stuff;
}

void drawnorm(int spritenum,int xoff,int yoff) {
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

void printcode()
{
  int x;

  box(MAPX*MAXSPRITEX+7,5,310,91,5);
  for (x=0;x<10;x++)
    switch (robots[monitor].history[x])
      {
        case WAIT : writetextxy("WAIT",MAPX*MAXSPRITEX+10,(x+1)*8); break;
	case TURNLEFT  : writetextxy("TURN LEFT",MAPX*MAXSPRITEX+10,(x+1)*8);
			  break;
	case TURNRIGHT : writetextxy("TURN RIGHT",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
	case BACKWARD : writetextxy("MOVE BACKWARD",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
	case IFWALL : writetextxy("IF WALL THEN...",MAPX*MAXSPRITEX+10,(x+1)*8);
				break;
        case BURROW : writetextxy("BURROWING",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
	case IFMATE :  writetextxy("IF MATE THEN...",MAPX*MAXSPRITEX+10,(x+1)*8);
				 break;
	case IFFIGHT : writetextxy("IF ENEMY THEN...",MAPX*MAXSPRITEX+10,(x+1)*8);
				 break;
	case IFCHEESE : writetextxy("IF CHEESE THEN...",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
	case SMELL : writetextxy("SNIFFING -- ",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
	case FORWARD  : writetextxy("MOVE FORWARD",MAPX*MAXSPRITEX+10,(x+1)*8);
			break;
       case SHAREFOOD : writetextxy("SHARING FOOD",MAPX*MAXSPRITEX+10,(x+1)*8);
                        break;
	case RESTART : writetextxy("RESTART",MAPX*MAXSPRITEX+10,(x+1)*8);
	break;
      }
}

int cheese_scan(int bot)
{
  int z;
  int value;

  value = 0;
  switch (robots[bot].dirptr) {
	 case 0 : for (z=robots[bot].xpos;z<MAPX;z++)
		if (map[z][robots[bot].ypos] == CHEESE) value = 1;
		else if (map[z][robots[bot].ypos] == 1) z = MAPX+1;
		  break;
	 case 1 : for (z=robots[bot].ypos;z<MAPY;z++)
		if (map[robots[bot].xpos][z] == CHEESE) value = 1;
		else if (map[robots[bot].xpos][z] == 1) z = MAPY+1;
		  break;
	 case 2 : for (z=robots[bot].xpos;z>=0;z--)
		if (map[z][robots[bot].ypos] == CHEESE) value = 1;
		else if (map[z][robots[bot].ypos] == 1) z = -1;
		  break;
	 case 3 : for (z=robots[bot].ypos;z>=0;z--)
		if (map[robots[bot].xpos][z] == CHEESE) value = 1;
		else if (map[robots[bot].xpos][z] == 1) z = -1;
		  break;
	 }
  return value;
}

int robotcheck(int bot)
{
  int x;

  for (x=0;x<numrobots;x++)
	 if (x != bot)
	 if ((robots[bot].xpos == robots[x].xpos) &&
	(robots[bot].ypos == robots[x].ypos) &&
	(robots[x].active))
	  return x+1;
  return 0;
}

int lifecheck()
{
  int x;

  for (x=0;x<numrobots;x++)
	 if (robots[x].active) return 1;
  return 0;
}

int accept(int xnew,int ynew)
{
  if ((xnew < MAPX) && (ynew < MAPY) && (xnew >= 0) && (ynew >= 0))
	{
	 if (map[xnew][ynew] == 1) return 0;
		else return 1;
	}
  else return 0;
}

int acceptorwall(int xnew,int ynew)
{
  if ((xnew < MAPX) && (ynew < MAPY) && (xnew >= 0) && (ynew >= 0))
          return 1;
  else return 0;
}

void sense_of_smell(int bot)
{
  if ((accept(robots[bot].xpos-1,robots[bot].ypos)) &&
	  (map[robots[bot].xpos-1][robots[bot].ypos] == 2)) robots[bot].dirptr = 2;
  else
  if ((accept(robots[bot].xpos+1,robots[bot].ypos)) &&
	  (map[robots[bot].xpos+1][robots[bot].ypos] == 2)) robots[bot].dirptr = 0;
  else
  if ((accept(robots[bot].xpos,robots[bot].ypos-1)) &&
	  (map[robots[bot].xpos][robots[bot].ypos-1] == 2)) robots[bot].dirptr = 3;
  else
  if ((accept(robots[bot].xpos,robots[bot].ypos+1)) &&
	  (map[robots[bot].xpos][robots[bot].ypos+1] == 2)) robots[bot].dirptr = 1;
}

int robot_in_front(int bot)
{
  int z;
  int value;

  value = 0;
  switch (robots[bot].dirptr) {
	 case 0 : for (z=robots[bot].xpos;z<MAPX;z++)
		if (map[z][robots[bot].ypos] >= 3)
		  {
			 value = map[z][robots[bot].ypos];
			 z = MAPX+1;
		  }
		else if (map[z][robots[bot].ypos] == 1) z = MAPX+1;
		  break;
	 case 1 : for (z=robots[bot].ypos;z<MAPY;z++)
		if (map[robots[bot].xpos][z] >= 3)
		  {
			 value = map[robots[bot].xpos][z];
			 z = MAPY + 1;
		  }
		else if (map[robots[bot].xpos][z] == 1) z = MAPY+1;
		  break;
	 case 2 : for (z=robots[bot].xpos;z>=0;z--)
		if (map[z][robots[bot].ypos] >= 3)
		  {
			 value = map[z][robots[bot].ypos];
			 z = -1;
		  }
		else if (map[z][robots[bot].ypos] == 1) z = -1;
		  break;
	 case 3 : for (z=robots[bot].ypos;z>=0;z--)
		if (map[robots[bot].xpos][z] >= 3)
		  {
			 value = map[robots[bot].xpos][z];
			 z = -1;
		  }
		else if (map[robots[bot].xpos][z] == 1) z = -1;
		  break;
	 }
  if (value)
  return robots[value-3].gender;
  else
  return 2;
}

int female(int a,int b)  // returns which of A or B is female
{
  if (robots[a].gender == 1) return a;
  else if (robots[b].gender == 1) return b;
  else return -1;
}

int male(int a,int b)    // returns which of A or B is male
{
  if (robots[a].gender == 0) return a;
  else if (robots[b].gender == 0) return b;
  else return -1;
}

int weaker(int a,int b)  // returns which of A or B is hungrier
{
  if (robots[a].hunger >= robots[b].hunger) return a;
  else return b;
}

int stronger(int a,int b)  // returns which of A or B is not hungrier
{
  if (robots[a].hunger <= robots[b].hunger) return a;
  else return b;
}

void sharefood(int bot)
{
  int a,newval;

  if (robots[bot].hunger < 50)
  for (a=0;a<numrobots;a++)
    {
        if ((robots[a].active) && (robots[a].xpos == robots[bot].xpos) &&
            (robots[a].ypos == robots[bot].ypos))
            if ((robots[a].hunger > 80) && (robots[bot].hunger < 50))
              {
                  robots[a].hunger -= 20;
                  robots[bot].hunger += 20;
              }
    }
}

void codeparse(int bot)
{
  int occupied,loop,r,temp,a,newnum,l,k;

  for (a=0;a<=numrobots;a++)
		  if ((!robots[a].active) && (!robots[a].father))
			 {
				newnum = a;
				if (newnum == numrobots) numrobots++;
				if (numrobots == MAXBOTS-1) numrobots = MAXBOTS-2;
				a = numrobots+1;
			 }
  if (robots[bot].pregnant == 1)
	 {
		robots[newnum].xpos = robots[bot].xpos;
		robots[newnum].ypos = robots[bot].ypos;
		robots[newnum].gender = random(2);
		robots[newnum].age = 0;
          robots[newnum].clan = robots[bot].clan;
          if (!random(400)) robots[newnum].clan = !robots[newnum].clan;
		for (loop=0;loop<CODELEN-2;loop++)
		{
	r = stronger(robots[bot].kidsfather,bot);
     l = weaker(robots[bot].kidsfather,bot);
     k = random(100);
// 60% strong
// 20% weaker
// 20% random
	if (k < 60) robots[newnum].robotcode[loop] = robots[r].robotcode[loop];
	else
	if (k < 80)
	robots[newnum].robotcode[loop] = robots[l].robotcode[loop];
	else
	robots[newnum].robotcode[loop] = random(RESTART);
		}
		robots[newnum].robotcode[loop] = RESTART;
		robots[newnum].robotcode[loop+1] = RESTART;
		robots[bot].pregnant = 0;
          robots[newnum].hunger = robots[bot].hunger >> 2;
		robots[robots[bot].kidsfather].father = 0;
          robots[newnum].active = 1;
          robots[newnum].codeptr = 0;
	 }
  else if (robots[bot].pregnant) robots[bot].pregnant--;
  map[robots[bot].xpos][robots[bot].ypos] = 0;
  robots[bot].fight = 0;
  robots[bot].mating = 0;
  occupied = robotcheck(bot);
  if (occupied)
  {
	 if ((robots[occupied-1].gender != robots[bot].gender)
         && (robots[occupied-1].clan == robots[bot].clan)
	&& (robots[occupied-1].age > 50)
	&& (robots[bot].age > 50))
		{
	if (!robots[female(occupied-1,bot)].pregnant)
	{
	  robots[female(occupied-1,bot)].pregnant = 60;
	  robots[female(occupied-1,bot)].kidsfather = male(occupied-1,bot);
	  robots[male(occupied-1,bot)].father = 1;
	}
	robots[bot].mating = 1;
	robots[occupied-1].mating = 1;
		}
	 else
	 if ((robots[occupied-1].clan != robots[bot].clan)
	&& (robots[occupied-1].age > 50)
	&& (robots[bot].age > 50))
		{
	robots[bot].fight = 1;
	robots[occupied-1].fight = 1;
	robots[weaker(bot,occupied-1)].active = 0;
		}
  }
  switch (robots[bot].robotcode[robots[bot].codeptr])
	 {
		 case TURNLEFT  :
			if (robots[bot].dirptr)
			  robots[bot].dirptr--; else
			  robots[bot].dirptr = 3; break;
		 case TURNRIGHT :
			if (robots[bot].dirptr < 3)
			robots[bot].dirptr++; else
			robots[bot].dirptr = 0; break;
		 case BACKWARD :
				 switch (robots[bot].dirptr) {
			  case 0 : if (accept(robots[bot].xpos-1,robots[bot].ypos))
				robots[bot].xpos--; break;
			  case 1 : if (accept(robots[bot].xpos,robots[bot].ypos-1))
				robots[bot].ypos--; break;
			  case 2 : if (accept(robots[bot].xpos+1,robots[bot].ypos))
				robots[bot].xpos++; break;
			  case 3 : if (accept(robots[bot].xpos,robots[bot].ypos+1))
				robots[bot].ypos++; break;
			}
			break;
	case IFWALL :
				switch (robots[bot].dirptr) {
			  case 0 : if (accept(robots[bot].xpos+1,robots[bot].ypos)) {
				robots[bot].codeptr++; }
				break;
			  case 1 : if (accept(robots[bot].xpos,robots[bot].ypos+1)){
				robots[bot].codeptr++; }
				break;
			  case 2 : if (accept(robots[bot].xpos-1,robots[bot].ypos)){
				robots[bot].codeptr++; }
				break;
			  case 3 : if (accept(robots[bot].xpos,robots[bot].ypos-1)){
				robots[bot].codeptr++; }
				break;
			}
			break;
    case BURROW :
				switch (robots[bot].dirptr) {
			  case 0 : if (acceptorwall(robots[bot].xpos+1,robots[bot].ypos))
                               map[robots[bot].xpos+1][robots[bot].ypos] = 0;
                          break;
			  case 1 : if (acceptorwall(robots[bot].xpos,robots[bot].ypos+1))
                               map[robots[bot].xpos][robots[bot].ypos+1] = 0;
                               break;
			  case 2 : if (acceptorwall(robots[bot].xpos-1,robots[bot].ypos))
                               map[robots[bot].xpos-1][robots[bot].ypos] = 0;
				break;
			  case 3 : if (acceptorwall(robots[bot].xpos,robots[bot].ypos-1))
                               map[robots[bot].xpos][robots[bot].ypos-1] = 0;
				break;
			}
			break;
	case IFMATE :
				 temp = robot_in_front(bot);
				 if ((temp == 2) || (temp == robots[bot].gender))
					robots[bot].codeptr++;
				 break;
	case IFFIGHT :
				 temp = robot_in_front(bot);
				 if ((temp == 2) || (temp != robots[bot].gender))
				 {
					robots[bot].codeptr++;
				 }
				 break;
	case IFCHEESE :
			if (!cheese_scan(bot)) {
					robots[bot].codeptr++;
			} break;
//	case GOTO : break;
	//addtext(bot,"GOTO"); robots[bot].codeptr =
	//		robots[bot].robotcode[robots[bot].codeptr+1]; break;
	case SMELL :	sense_of_smell(bot); break;
	case FORWARD  :
			switch (robots[bot].dirptr) {
			  case 0 : if (accept(robots[bot].xpos+1,robots[bot].ypos))
				robots[bot].xpos++; break;
			  case 1 : if (accept(robots[bot].xpos,robots[bot].ypos+1))
				robots[bot].ypos++; break;
			  case 2 : if (accept(robots[bot].xpos-1,robots[bot].ypos))
				robots[bot].xpos--; break;
			  case 3 : if (accept(robots[bot].xpos,robots[bot].ypos-1))
				robots[bot].ypos--; break;
			}
			break;
     case SHAREFOOD : sharefood(bot);
                      break;
	case RESTART :	robots[bot].codeptr = -1; break;
    }
  addtext(bot,robots[bot].codeptr);
  robots[bot].codeptr++;
  robots[bot].hunger++;
  robots[bot].age++;
  if (!random(100)) map[random(MAPX)][random(MAPY)] = WALL;
  if ((map[robots[bot].xpos][robots[bot].ypos] == CHEESE) &&
       (robots[bot].hunger > 10))
   {
	 robots[bot].hunger = 0;
	 map[random(MAPX-2)+1][random(MAPY-2)+1] = CHEESE;
	 map[robots[bot].xpos][robots[bot].ypos] = 0;
   }
  if (robots[bot].hunger >= 200)
	robots[bot].active = 0;
  if (robots[bot].age >= AGELIMIT)
	robots[bot].active = 0;
  if (robots[bot].active) map[robots[bot].xpos][robots[bot].ypos] = 3+bot;
	 else map[robots[bot].xpos][robots[bot].ypos] = CHEESE;
}

char decipher(char temp[80])
{
 if (!strcmpi(temp,"WAIT")) return 0;
 if (!strcmpi(temp,"IFWALL")) return 1;
 if (!strcmpi(temp,"IFMATE")) return 2;
 if (!strcmpi(temp,"IFFIGHT")) return 3;
 if (!strcmpi(temp,"IFCHEESE")) return 4;
 if (!strcmpi(temp,"TURNLEFT")) return 5;
 if (!strcmpi(temp,"TURNRIGHT")) return 6;
 if (!strcmpi(temp,"FORWARD")) return 7;
 if (!strcmpi(temp,"BACKWARD")) return 8;
 if (!strcmpi(temp,"SMELL")) return 9;
 if (!strcmpi(temp,"BURROW")) return 10;
 if (!strcmpi(temp,"SHAREFOOD")) return 11;
 if (!strcmpi(temp,"RESTART")) return 12;
 return 255;
}

void loadcode()
{
	FILE *stream;
	int a;
     char temp[80];

	if ((stream = fopen("code.cod","r")) == NULL) printf("Error.\n");
	else
	for (a=0;a<CODELEN;a++)
	 {
		fscanf(stream,"%s",&temp);
          robots[0].robotcode[a] = decipher(temp);
          if (!random(50)) robots[0].robotcode[a] = random(RESTART);
		robots[3].robotcode[a] = robots[0].robotcode[a];
          robots[5].robotcode[a] = robots[0].robotcode[a];
          robots[6].robotcode[a] = robots[0].robotcode[a];
	 }
	fclose(stream);

	if ((stream = fopen("code2.cod","r")) == NULL) printf("Error.\n");
	else
	for (a=0;a<CODELEN;a++)
	 {
          fscanf(stream,"%s",&temp);
          robots[1].robotcode[a] = decipher(temp);
          if (!random(50)) robots[1].robotcode[a] = random(RESTART);
		robots[2].robotcode[a] = robots[1].robotcode[a];
          robots[4].robotcode[a] = robots[1].robotcode[a];
          robots[7].robotcode[a] = robots[1].robotcode[a];
	 }
	fclose(stream);
}

void makeprogram()
{
  int l,bot;

 randomize();
 numrobots = 8;
 for (bot=0;bot<numrobots;bot++)
  {
  for (l=0;l<CODELEN;l++)
	{
	 if ((l) && (robots[bot].robotcode[l-1] >= 1) &&
	(robots[bot].robotcode[l-1] <= 4))
	  robots[bot].robotcode[l] = random(4)+5;
	 else
 	 robots[bot].robotcode[l] = random(RESTART);
	}
  robots[bot].robotcode[l] = RESTART;
  robots[bot].robotcode[l+1] = RESTART;
  robots[bot].gender = bot & 1;

  robots[bot].codeptr = 0;
  robots[bot].hunger = 0;
  map[robots[bot].xpos][robots[bot].ypos] = 3+bot;
  robots[bot].active = 1;
 }
 loadcode();
 robots[0].xpos = (MAPX>>1)+1;
 robots[0].ypos = 1;
 robots[1].xpos = (MAPX>>1);
 robots[1].ypos = 1;

 robots[2].xpos = MAPX-1;
 robots[2].ypos = MAPY-2;
 robots[3].xpos = 4;
 robots[3].ypos = MAPY-2;

 robots[0].clan = 0;
 robots[1].clan = 0;
 robots[2].clan = 1;
 robots[3].clan = 1;

 robots[4].xpos = MAPX-2;
 robots[4].ypos = 0;
 robots[5].xpos = 3;
 robots[5].ypos = 0;
 robots[6].xpos = MAPX-2;
 robots[6].ypos = MAPY-3;
 robots[7].xpos = 4;
 robots[7].ypos = MAPY-3;
 robots[4].clan = 0;
 robots[5].clan = 0;
 robots[6].clan = 1;
 robots[7].clan = 1;
}

void init()
{
  initmode();
  loadpal("pal.col",pal);
  loadfont("small.fnt");
  makeprogram();
  fontfore = 50;
  fontback = 0;
  fontbackon = 0;
  // setpal(pal);
  loadsprite("floor.stn",FLOOR);
  loadsprite("wall.stn",WALL);
  loadsprite("cheese.stn",CHEESE);
  loadsprite("hilite.stn",HILITE);
  loadsprite("robot1.stn",4);
  loadsprite("robot2.stn",5);
  loadsprite("robot3.stn",6);
  loadsprite("robot4.stn",7);
  loadsprite("frobot1.stn",8);
  loadsprite("frobot2.stn",9);
  loadsprite("frobot3.stn",10);
  loadsprite("frobot4.stn",11);
  loadsprite("grobot1.stn",12);
  loadsprite("grobot2.stn",13);
  loadsprite("grobot3.stn",14);
  loadsprite("grobot4.stn",15);
  loadsprite("drobot1.stn",16);
  loadsprite("drobot2.stn",17);
  loadsprite("drobot3.stn",18);
  loadsprite("drobot4.stn",19);
}

double density(int num)
{
  int x,count=0,t=0;

  for (x=0;x<=numrobots;x++)
   {
     if ((robots[x].active) && (robots[x].clan == num)) count++;
     if (robots[x].active) t++;
   }
   return (100 * (float) count / (float) (t));
}

double spacedensity(int num)
{
  int x,count=0;

  for (x=0;x<numrobots;x++)
   if ((robots[x].active) && (robots[x].clan == num)) count++;
   return ((float) count / (float) (MAPX*MAPY));
}

int numclan(int num)
{
  int x,count=0;

  for (x=0;x<numrobots;x++)
   if ((robots[x].active) && (robots[x].clan == num)) count++;

  return count;
}

void drawscreen()
{
  int x,y,bot,numrats,numbats;
  double dens;
  char tstr[10];

  basesprseg = FP_SEG(spritedata);
  basesproff = FP_OFF(spritedata);
  curpage = !curpage;
  setActivePage(curpage);
  clearPage(curpage,0);
  for (x=0;x<MAPX;x++)
    for (y=0;y<MAPY;y++)
      {
	if (map[x][y] == 1) drawtile(WALL,x*MAXSPRITEX,y*MAXSPRITEY);
	  else drawtile(FLOOR,x*MAXSPRITEX,y*MAXSPRITEY);
	if (map[x][y] == 2)
	  drawnorm(CHEESE,x*MAXSPRITEX,y*MAXSPRITEY);
      }
  for (bot=0;bot<numrobots;bot++)
   if (robots[bot].active)
    drawnorm(ROBOFF+(robots[bot].gender<<2)+
             robots[bot].dirptr+
             (robots[bot].clan<<3),
	robots[bot].xpos*MAXSPRITEX,robots[bot].ypos*MAXSPRITEY);
  if (robots[monitor].active)
  drawnorm(HILITE,robots[monitor].xpos*MAXSPRITEX,
		  robots[monitor].ypos*MAXSPRITEY);
  if (robots[monitor].gender) writetextxy("FEMALE",MAXSPRITEX*MAPX+20,101);
    else writetextxy("MALE",MAXSPRITEX*MAPX+20,101);
  if (robots[monitor].clan) writetextxy("BAT",MAXSPRITEX*MAPX+70,101);
    else writetextxy("RAT",MAXSPRITEX*MAPX+70,101);
  writetextxy("HUNGER %:",MAXSPRITEX*MAPX+20,111);
  writetextxy(itoa(robots[monitor].hunger>>1,tstr,10),MAXSPRITEX*MAPX+80,111);
  writetextxy("AGE:",MAXSPRITEX*MAPX+20,119);
  writetextxy(itoa(robots[monitor].age,tstr,10),MAXSPRITEX*MAPX+80,119);
  if (robots[monitor].pregnant)
  {
	 writetextxy("PREGNANT: ",MAXSPRITEX*MAPX+20,127);
    writetextxy(itoa(robots[monitor].pregnant,tstr,10),MAXSPRITEX*MAPX+80,127);
  }
  if (robots[monitor].fight)
    writetextxy("FIGHTING",MAXSPRITEX*MAPX+20,151);
  if (robots[monitor].mating)
    writetextxy("MATING",MAXSPRITEX*MAPX+20,135);
  if (!robots[monitor].active)
    writetextxy("DEAD",MAXSPRITEX*MAPX+20,143);
  writetextxy("Q TO ABORT",0,220);
  writetextxy("CYCLES:",0,230);
  writetextxy(itoa(cycles,tstr,10),60,230);
  writetextxy("RP%",100,230);
  dens = density(0);
  writetextxy(gcvt(dens,2,tstr),120,230);
  writetextxy("BP%",170,230);
  dens = density(1);
  writetextxy(gcvt(dens,2,tstr),190,230);
  writetextxy("RD%",100,220);
  dens = spacedensity(0);
  writetextxy(gcvt(dens,4,tstr),120,220);
  writetextxy("BD%",170,220);
  dens = spacedensity(1);
  writetextxy(gcvt(dens,4,tstr),190,220);
  numrats = numclan(0);
  numbats = numclan(1);
  writetextxy("R#",240,220);
  writetextxy(itoa(numrats,tstr,10),260,220);
  writetextxy("B#",240,230);
  writetextxy(itoa(numbats,tstr,10),260,230);
  writetextxy("TOT:",280,220);
  writetextxy(itoa(numrats+numbats,tstr,10),280,230);
  writetextxy("MONITORING ROBOT #",MAXSPRITEX*MAPX+10,200);
  writetextxy(itoa(monitor,tstr,10),MAXSPRITEX*MAPX+122,200);
  box(MAPX*MAXSPRITEX+7,5,310,91,5);
  printcode();
  box(MAPX*MAXSPRITEX+7,98,MAPX*MAXSPRITEX+16,135,5);
  if (robots[monitor].VAR_A) drawfont('A',MAXSPRITEX*MAPX+10,101);
  if (robots[monitor].VAR_B) drawfont('B',MAXSPRITEX*MAPX+10,109);
  if (robots[monitor].VAR_C) drawfont('C',MAXSPRITEX*MAPX+10,117);
  if (robots[monitor].VAR_D) drawfont('D',MAXSPRITEX*MAPX+10,125);
  setVisiblePage(curpage);
}

void misc()
{
  int cheesed=0;
  char keyhit=0,done=0,bot=0,key2;

  while ((!done) && (!cheesed))
   {
	 drawscreen();
    done = !lifecheck();
    for (bot=0;bot<numrobots;bot++)
	 if (robots[bot].active)
	codeparse(bot);
	 cycles++;
	 if (kbhit()) keyhit = getch(); else keyhit = 5;
	 switch (toupper(keyhit))
	{
	  case '+' : if (monitor < numrobots-1) monitor++; break;
	  case '-' : if (monitor) monitor--; break;
	  case 'A' : robots[monitor].VAR_A = !robots[monitor].VAR_A; break;
	  case 'B' : robots[monitor].VAR_B = !robots[monitor].VAR_B; break;
	  case 'C' : robots[monitor].VAR_C = !robots[monitor].VAR_C; break;
	  case 'D' : robots[monitor].VAR_D = !robots[monitor].VAR_D; break;
       case 'H' : robots[monitor].hunger = 0; break;
       case 'R' : robots[monitor].age = 0; break;
	  case 'P' : robots[monitor].pregnant = 2;
                  robots[monitor].kidsfather = 0;
                  robots[0].father = 1;
                  break;
       case 'K' : robots[monitor].active = 0; break;
       case 'E' : robots[monitor].active = 1; robots[monitor].hunger=0;
                  robots[monitor].age=50; break;
       case 'S' : robots[monitor].clan = !robots[monitor].clan; break;
	  case 0   : key2 = getch();
					 switch (key2)
						{
						  case 72 :
										switch (robots[monitor].dirptr) {
										case 0 : if (accept(robots[monitor].xpos+1,robots[monitor].ypos))
													robots[monitor].xpos++; break;
										case 1 : if (accept(robots[monitor].xpos,robots[monitor].ypos+1))
													robots[monitor].ypos++; break;
										case 2 : if (accept(robots[monitor].xpos-1,robots[monitor].ypos))
													robots[monitor].xpos--; break;
										case 3 : if (accept(robots[monitor].xpos,robots[monitor].ypos-1))
													robots[monitor].ypos--; break;
										}
										break;
						  case 75  :
										 if (robots[monitor].dirptr)
										 robots[monitor].dirptr--; else
										 robots[monitor].dirptr = 3; break;
						  case 77  :
										 if (robots[monitor].dirptr < 3)
										 robots[monitor].dirptr++; else
										 robots[monitor].dirptr = 0; break;
						  case 80 :
										 switch (robots[monitor].dirptr) {
											case 0 : if (accept(robots[monitor].xpos-1,robots[monitor].ypos))
														robots[monitor].xpos--; break;
											case 1 : if (accept(robots[monitor].xpos,robots[monitor].ypos-1))
														robots[monitor].ypos--; break;
											case 2 : if (accept(robots[monitor].xpos+1,robots[monitor].ypos))
														robots[monitor].xpos++; break;
											case 3 : if (accept(robots[monitor].xpos,robots[monitor].ypos+1))
														robots[monitor].ypos++; break;
									}
									break;
						  }
						  break;
	  case 'Q' : done=1; break;
	}
	}
  while (!kbhit());
}

void main()
{
  init();
  misc();
  normmode();
}
