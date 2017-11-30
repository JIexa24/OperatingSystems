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
static int symA[2] = {4294967295,4294967295};

void printField(char* field, int XY)
{
  int i;
  mt_clrscr();
  mt_gotoXY(0,0);
  printf("I have %s\n", XY == 1 ? "X" : "O");
  for (i = 0; i < 9; i++) {
    if (field[i] == 'a') {
      bc_printbigchar(symA, (i % 3) * 9 + 1, (i / 3) * 9 + 2, clr_red, clr_default);
    } else if (field[i] == 'X') {
      bc_printbigchar(symX, (i % 3) * 9 + 1, (i / 3) * 9 + 2, clr_red, clr_default);
    } else if (field[i] == 'O') {
      bc_printbigchar(symO, (i % 3) * 9 + 1, (i / 3) * 9 + 2, clr_red, clr_default);
    }
  }
  mt_gotoXY(10,1);
}
