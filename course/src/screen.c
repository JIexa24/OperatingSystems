#include "../include/screen.h"
#include <stdio.h>
/*static char symX[64] =
{
1, 0, 0, 0, 0, 0, 0, 1,
0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0, 0,
0, 1, 0, 0, 0, 0, 1, 0,
1, 0, 0, 0, 0, 0, 0, 1
};*/
/*static char symO[64] =
{
0, 1, 1, 1, 1, 1, 1, 0,
1, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 1,
0, 1, 1, 1, 1, 1, 1, 0,
};*/

static int symX[2] = {405029505,2168595480};
static int symO[2] = {2172748158,2122416513};
static int symA[2] = {0,0};

void printField(char* field, int XY, int position)
{
  int i;
  enum colors fgcolor = clr_red;
  enum colors bgcolor = clr_default;
//  mt_clrscr();
  mt_gotoXY(0,0);
  printf("I have %s\n", XY == 1 ? "X" : "O");
  printBox();
  for (i = 0; i < 9; i++) {
    if (i == position){
     if (XY == 1) bgcolor = clr_green;
     if (XY == 0) bgcolor = clr_cyan;
    } else bgcolor = clr_default;
    if (field[i] == 'a') {
      bc_printbigchar(symA, (i % 3) * 9 + 2, (i / 3) * 9 + 3, fgcolor, bgcolor);
    } else if (field[i] == 'X') {
      bc_printbigchar(symX, (i % 3) * 9 + 2, (i / 3) * 9 + 3, fgcolor, bgcolor);
    } else if (field[i] == 'O') {
      bc_printbigchar(symO, (i % 3) * 9 + 2, (i / 3) * 9 + 3, fgcolor, bgcolor);
    }
  }
  mt_gotoXY(10,1);
}
void printBox()
{
  bc_box(1,2,10,11);
  bc_box(10,2,19,11);
  bc_box(19,2,28,11);

  bc_box(1,11,10,20);
  bc_box(10,11,19,20);
  bc_box(19,11,28,20);

  bc_box(1,20,10,29);
  bc_box(10,20,19,29);
  bc_box(19,20,28,29);
}
