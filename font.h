#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STRING_H
#include <string.h>
#endif

#define CR 13
#define BS 8

#define NAMELENGTH 110

int fontback=0;
int fontfore=1;
int fontbackon=0;
int highlite=2;
int unhighlite=1;

#define FONTX 5
#define FONTY 6

typedef char fontletter[6];
typedef fontletter fonttype[256];

fonttype font;

void loadfont(char *ch)
{
   FILE *stream;

   if ((stream = fopen(ch,"rb")) == NULL) printf("Error.",0,140);
   else
   {
     fread(font, 1, 1536, stream);
     fclose(stream);
   }
}

int odd(char num)
{
  return (num & 1);
}

void drawfont(int letter,int xoff,int yoff)
{
 int x,y;
 char color;

 for (y=0;y<FONTY;y++)
  for (x=0;x<=(FONTX-1);x++)
   {
    color = font[letter][y];
    if (odd(color >> (FONTX-(1+x)))) putpixel(xoff+x,yoff+y,fontfore);
    else if (fontbackon) putpixel(xoff+x,yoff+y,fontback);
   }
}

void highwritetextxy(char *string,int xoff,int yoff)
{
 int x,len,yoff2,truex=0;

 yoff2=yoff;
 strupr(string);
 len = strlen(string);
 for(x=0;x<len;x++)
 {
  if (string[x] == NULL) x = len+5; else
  if (string[x] == '<') fontfore = highlite; else
  if (string[x] == '>') fontfore = unhighlite; else
  {
    drawfont((int) (string[x]),xoff+(truex*(FONTX+1)),yoff2);
    truex++;
    if (xoff+truex*(FONTX+1)+FONTX >= width) { yoff2+=FONTY+1; truex=0;}
  }
 }
}

void highwritetextxy2(char *string,int xoff,int yoff)
{
 int x,len;

 strupr(string);
 len = strlen(string);
 if (len > 64) len = 64;
 for(x=0;x<len;x++)
 {
  if (string[x] == NULL) x = len+2; else
  if (string[x] == '<') fontfore = highlite; else
  if (string[x] == '>') fontfore = unhighlite; else
    drawfont((int) (string[x]),xoff+(x*FONTX),yoff);
 }
}

void writetextxy(char *string,int xoff,int yoff)
{
 int x,len,yoff2,truex=0;

 yoff2=yoff;
 strupr(string);
 len = strlen(string);
 for(x=0;x<len;x++)
 {
  if (string[x] == NULL) x = len+5;
  drawfont((int) (string[x]),xoff+(truex*(FONTX+1)),yoff2);
  truex++;
  if (xoff+truex*(FONTX+1)+FONTX >= width) { yoff2+=FONTY+1; truex=0;}
 }
}

void writetextxy2(char *string,int xoff,int yoff)
{
 int x,len;

 strupr(string);
 len = strlen(string);
 if (len > 64) len = 64;
 for(x=0;x<len;x++)
 {
  if (string[x] == NULL) x = len+2;
  drawfont((int) (string[x]),xoff+(x*FONTX),yoff);
 }
}

void mwritetextxy(char *string,int xoff,int yoff,int maxlen)
{
 int x,len,yoff2,truex=0,dowrite=1;

 yoff2=yoff;
 strupr(string);
 len = strlen(string);
 if (maxlen < len) len = maxlen;
 for(x=0;(dowrite) &&(x<len);x++)
 {
  if (string[x] == NULL) dowrite = 0;
  drawfont((int) (string[x]),xoff+(truex*(FONTX+1)),yoff2);
  truex++;
  if (xoff+truex*(FONTX+1)+FONTX >= width) { yoff2+=FONTY+1; truex=0;}
 }
}

void mwritetextxy2(char *string,int xoff,int yoff,int maxlen)
{
 int x,len,dowrite=1;

 strupr(string);
 len = strlen(string);
 if (len > 64) len = 64;
 if (maxlen < len) len = maxlen;
 for(x=0;(dowrite) && (x<len);x++)
 {
  if (string[x] == NULL) dowrite = 0;
  drawfont((int) (string[x]),xoff+(x*FONTX),yoff);
 }
}

char upcase(char foo)
{
 if (((int) foo >= (int) 'a') && ((int) foo <= (int) 'z'))
   foo = (char) ((int) foo - 32);
 return foo;
}

void readin(char *ch,int x,int y)
{
 int n=0;
 char foo;

 strnset(ch,' ',14);
 do
  {
    foo = getch();
    if ((foo != CR) && (foo != BS))
     {
      ch[n] = foo;
      drawfont(upcase(foo),n*7+x,y);
      n++;
     }
    if (foo == BS)
     {
       if (n) n--;
       ch[n] = 32;
       drawfont(32,n*7+x,y);
     }
  }
  while (foo != CR);
  ch[n] = NULL;
}

void readin2(char *ch,int x,int y)
{
 int n=0;
 char foo;

 strnset(ch,' ',14);
 do
  {
    foo = getch();
    if ((foo != CR) && (foo != BS))
     {
      ch[n] = foo;
      drawfont(upcase(foo),n*6+x,y);
      n++;
     }
    if (foo == BS)
     {
       if (n) n--;
       ch[n] = 32;
       drawfont(32,n*6+x,y);
     }
  }
  while (foo != CR);
  ch[n] = NULL;
}