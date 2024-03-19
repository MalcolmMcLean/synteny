#include <stdio.h>
#include <stdlib.h>


static char *As;
static char *Bs;
static char *Cs;
int alen;
int blen;
int clen;


static int ncode[7] = {7,1,2,4,3,5,6};
static int infinity = 32000;
int ***U;
int **Udone;
int MaxCost = 70;

int ukk(int ab, int ac, int cost);
int step(int PBest, int ab, int ac, int cost, int da, int db, int dc, int *prevcost);
void traceback(int ab, int ac, int cost);
int triple(int a, int b, int c);
void showtriple(int a, int b, int c);
int max7(int a, int b, int c, int d, int e, int f, int g);
int max2(int a, int b);

/*
char **ukkonenR(char *s1, char *s2, char *s3, int cost)
{
  Udone = malloc( (70+70+1) * sizeof(int *));
  for(i=0;i<70+70+1;i++)
    Udone[i] = malloc( (70+70+1) * sizeof(int));

  U = malloc( (70+70+1) * sizeof(int **));
  for(i=0;i<70+70+1;i++)
  {
    U[i] = malloc( (70+70+1) * sizeof(int *));
    for(ii=0;ii< 70+70+1;ii++)
    {
      U[i][ii] = malloc(71 * sizeof(int));
      for(iii=0;iii<71;iii++)
        U[i][ii][iii] = 0;
    }
  }

  
  for(ab=-MaxCost;ab<=MaxCost;ab++)
    for(ac=-MaxCost;ac<-MaxCost;ac++)
      Udone[ab+MaxCost][ac+MaxCost] = -3;

  As = s1;
  Bs = s2;
  Cs = s3;
  alen = strlen(As);
  blen = strlen(Bs);
  clen = strlen(Cs);

  i = 0;

  while(i < alen && triple(i,i,i) == 0)
    i++;

  U[0][0][0] = i -1;
  Udone[0][0] = 0;
    
  ab = alen - blen;
  ac = alen - clen;
  do
  {
      cost = max7(0, ab, ac, -ab, -ac, ab-ac, ac-ab) - 1;
      do
      {
        cost++;
        a = ukk(ab, ac, cost);
      }
    } while(a < alen) 
  }  

  traceback(ab, ac, cost);
}

*/

int ukk(int ab, int ac, int cost)
{
  int radius, nn, neighbour;
  int a, a2, b, c, da, db, dc, pc;
  //int prevcost;

  radius = max7(0, ab, ac, -ab, -ac, ab-ac, ac-ab);
  
  if(radius > cost)
    return -infinity;
  else if(cost <= Udone[MaxCost+ab][MaxCost+ac])
    return U[MaxCost+ab][MaxCost+ac][cost];
  else
  {
    a = -infinity;
    //    prevcost = -infinity;
    
    for(nn=0;nn<7;nn++)
    {
      neighbour = ncode[nn];
      da = neighbour / 4;
      db = (neighbour/2) % 2;
      dc = neighbour % 2;
      a2 = step(a, ab-da+db, ac-da+dc, cost-1, da, db, dc, &pc);
      if(a2 < a)
      {
        a = a2;
      }
    }
      
    b = a - ab;
    c = a - ac;
   
    while(a >= 0 && a < alen && triple(a+1, b+1, c+1) == 0)
    {
      a++;
      b++;
      c++;
    }

    U[MaxCost+ab][MaxCost+ac][cost] = a;
    Udone[MaxCost+ab][MaxCost+ac] = cost;
    
    return a;
  }
}


int step(int PBest, int ab, int ac, int cost, int da, int db, int dc, int *prevcost)
{
  int a, b, c, a2, b2,c2, a3;
  
  a3 =0;
  a = ukk(ab, ac, cost);
  b = a - ab;
  c = a - ac;
  *prevcost = cost;
  a2 = a+da; b2 = b+db; c2 = c+dc;
  switch(da+db+dc)
  {
  case 1: break;
  case 2: while(a2 >= PBest && a2 > a3 && a2 >= 0 && b2 >= 0 && c2 >= 0 &&
		triple(a2*da, b2*db, c2*dc) == 2)
      {
        a2--;
        b2--;
        c2--;
      }
    if(a2 == a3)
      *prevcost = cost -1;
    break;

  case 3:
    if( triple(a2, b2, c2) == 2)
      {
        a2 = a;
        a3 = ukk(ab, ac, cost-1) + 1;
        if(a3 > a2)
	{
          a2 = a3;
          *prevcost = cost-1;
	}
      }
    break;
  }
  return a2;
}

void traceback(int ab, int ac, int cost)
{
  int a, a2, b, c, da, db, dc, ab2, ac2;
  int prevcost, pc, choice, nn, neighbour, i;

  a = -infinity;
  choice = 0;

  for(nn=0;nn<7;nn++)
  {
    neighbour = ncode[nn];
    da = neighbour / 4;
    db = (neighbour / 2) % 2;
    dc = neighbour % 2;
    a2 = step(a, ab-da+db, ac-da+dc, cost-1, da, db, dc, &pc);
    if(a2 > a || (a2 >= a && 
		  (choice == 1 || choice == 2 || choice == 4) && 
		  (neighbour == 3 || neighbour == 5 || neighbour == 6)) )
    {
      a = a2;
      choice = neighbour;
      prevcost = pc;
    }
    a = max2(0, a);
    b = a - ab;
    c = a-ac;

    da = choice / 4;
    db = (choice / 2) % 2;
    dc = choice % 2;
    ab2 = ab-da+db;
    ac2 = ac-da+dc;
   
    if(a > 1 || b > 1 || c > 1)
      traceback(ab2, ac2, prevcost);
    if(a+b+c > 0)
    {
      showtriple(a*da, b*db, c*dc);
    }
    for(i=0;i<ukk(ab, ac, cost);i++)
      showtriple(a+i, b+i, c+i);
  }
}

void showU(int ***array)
{
}

int triple(int a, int b, int c)
{
  char ach, bch,  cch;
  int ans;

  ach = As[a];
  bch = Bs[b];
  cch = Cs[c];

  if(ach == bch)
  {
    if(ach == cch)
      ans = 0;
    else
      ans = 1;
  }
  else
  {
    if(ach == cch || bch == cch)
      ans = 1;
    else
      ans = 2;
  }
  return ans;
}

void showtriple(int a, int b, int c)
{
  printf("%c %c %c\n", 
   a >= 0 ? As[a] : '-', 
   b >= 0 ? Bs[b] : '-', 
   c >= 0 ? Cs[c] : '-');
}

int max7(int a, int b, int c, int d, int e, int f, int g)
{
  return max2(max2(max2(a, b), max2(c, d)), max2(max2(e, f), g)); 
}
 
int max2(int a, int b)
{
  return a > b ? a : b;
}

int main(void)
{
}
