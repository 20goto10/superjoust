#include <stdio.h>
#include <dos.h>

#define LKEY 0
#define RKEY 1
#define UKEY 2
#define SKEY 3

unsigned char keys[4][4];

void loadkeys()
{
  FILE *stream;
  int a,b;

  if ((stream = fopen("keys.dat","rb")) == NULL) printf("Creating new key file.\n");
  else
  {
    for (a=0;a<4;a++)
    for (b=0;b<4;b++)
      fread(&keys[a][b],1,1,stream);
    fclose(stream);
  }
}

void savekeys()
{
  FILE *stream;
  int a,b;

  if ((stream = fopen("keys.dat","wb")) == NULL) printf("Creating new key file.\n");
  else
  {
    for (a=0;a<4;a++)
    for (b=0;b<4;b++)
      fwrite(&keys[a][b],1,1,stream);
    fclose(stream);
  }
}

char scankey()
{
  int a;
  char c;

  c = inportb(0x60);
  while ((inportb(0x60)|128) == (c|128));
  return inportb(0x60);
}

void saykey(char input)
{
  switch (input) {
    case 2 : printf("1"); break;
    case 3 : printf("2"); break;
    case 4 : printf("3"); break;
    case 5 : printf("4"); break;
    case 6 : printf("5"); break;
    case 7 : printf("6"); break;
    case 8 : printf("7"); break;
    case 9 : printf("8"); break;
    case 10: printf("9"); break;
    case 11: printf("0"); break;
    case 12: printf("-"); break;
    case 13: printf("="); break;

    case 14: printf("[BACKSPACE]"); break;
    case 15: printf("[TAB]"); break;

    case 16: printf("q"); break;
    case 17: printf("w"); break;
    case 18: printf("e"); break;
    case 19: printf("r"); break;
    case 20: printf("t"); break;
    case 21: printf("y"); break;
    case 22: printf("u"); break;
    case 23: printf("i"); break;
    case 24: printf("o"); break;
    case 25: printf("p"); break;
    case 26: printf("["); break;
    case 27: printf("]"); break;

    case 28: printf("[ENTER]"); break;
    case 29: printf("[CTRL]"); break;

    case 30: printf("a"); break;
    case 31: printf("s"); break;
    case 32: printf("d"); break;
    case 33: printf("f"); break;
    case 34: printf("g"); break;
    case 35: printf("h"); break;
    case 36: printf("j"); break;
    case 37: printf("k"); break;
    case 38: printf("l"); break;
    case 39: printf(";"); break;
    case 40: printf("\")"); break;

    case 41: printf("`"); break;
    case 42: printf("[LEFT SHIFT]"); break;
    case 43: printf("\\"); break;

    case 44: printf("z"); break;
    case 45: printf("x"); break;
    case 46: printf("c"); break;
    case 47: printf("v"); break;
    case 48: printf("b"); break;
    case 49: printf("n"); break;
    case 50: printf("m"); break;
    case 51: printf(","); break;
    case 52: printf("."); break;
    case 53: printf("/"); break;

    case 54: printf("[RIGHT SHIFT]"); break;

    case 55: printf("*"); break;

    case 56: printf("[ALT]"); break;
    case 57: printf("[SPACE]"); break;

    case 58: printf("[CAPS LOCK]"); break;

    case 59: printf("[F1]"); break;
    case 60: printf("[F2]"); break;
    case 61: printf("[F3]"); break;
    case 62: printf("[F4]"); break;
    case 63: printf("[F5]"); break;
    case 64: printf("[F6]"); break;
    case 65: printf("[F7]"); break;
    case 66: printf("[F8]"); break;
    case 67: printf("[F9]"); break;
    case 68: printf("[F10]"); break;

    case 69: printf("[NUM LOCK]"); break;
    case 70: printf("[SCROLL LOCK]"); break;

    case 71: printf("[HOME]"); break;
    case 72: printf("[UP]"); break;
    case 73: printf("[PgUp]"); break;

    case 74: printf("-"); break;
    case 75: printf("[LEFT]"); break;
    case 76: printf("[CENTER]"); break;
    case 77: printf("[RIGHT]"); break;

    case 78: printf("+"); break;

    case 79: printf("[END]"); break;
    case 80: printf("[DOWN]"); break;
    case 81: printf("[PgDn]"); break;

    case 82: printf("[INSERT]"); break;
    case 83: printf("[DELETE]"); break;
    case 87: printf("[F11]"); break;
    case 88: printf("[F12]"); break;
   }
   printf("\n");
}

void main()
{
  int player,foo;

  loadkeys();
  printf("SuperJoust Key Configurator v1.0\n");
  printf("By Ben Chadwick... as usual...\n");
  printf("------------------------------\n");
  getnum:
  printf("Player to change [1-4], 0 to save, 9 to quit > \n");
  scanf("%d",&player);
  foo = 0;
  switch (player) {
    case 1 :
    case 2 :
    case 3 :
    case 4 : printf("Editting player %d's keys.\n",player);
	     player--;
	     printf("Enter key for going left: \n");
	     do {
	     keys[player][LKEY] = scankey();
	     if (keys[player][LKEY] == 1) printf("Sorry, `Escape' cannot be used!\n");
	     }
	     while (keys[player][LKEY] == 1);
	     saykey(keys[player][LKEY]);
	     printf("Enter key for going right: \n");
	     do {
	     keys[player][RKEY] = scankey();
	     if (keys[player][RKEY] == 1) printf("Sorry, `Escape' cannot be used!\n");
	     }
	     while (keys[player][RKEY] == 1);
	     saykey(keys[player][RKEY]);
	     printf("Enter key for flapping: \n");
	     do {
	     keys[player][UKEY] = scankey();
	     if (keys[player][UKEY] == 1) printf("Sorry, `Escape' cannot be used!\n");
	     }
	     while (keys[player][UKEY] == 1);
	     saykey(keys[player][UKEY]);
	     printf("Enter key for using special ability: \n");
	     do {
	     keys[player][SKEY] = scankey();
	     if (keys[player][SKEY] == 1) printf("Sorry, `Escape' cannot be used!\n");
	     }
	     while (keys[player][SKEY] == 1);
	     saykey(keys[player][SKEY]);
	     printf("Done.\n");
	     *(int far *) MK_FP(0x40,0x1A) = *(int far *) MK_FP(0x40,0x1C);
	     break;
    case 0 : savekeys(); printf("Saved.\n"); break;
    case 9 : foo = 2; break;
    default : foo = 1; break;
  }
  if (foo == 1) printf("Enter one of 1,2,3,4,0, or 9!\n");
  if (foo != 2) goto getnum;
  printf("Have fun!\n");
}