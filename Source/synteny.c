#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct match
{
  int pos;
  int score;
  int bad;
  struct match *next;
} MATCH;

int align_r(char *seq1, int len1, char *seq2, int len2, char **a1, char **a2, int patlen);

MATCH *getmatches(char *seq1, int len1, char *seq2, int len2, int patlen);
void killmatches(MATCH *m, int N);
int getgoodmatches(MATCH *m, int N, int **pos1, int **pos2, int patlen);
int *matchtolist(MATCH *m, int N, int *lenret);
int match1(char *seq1, int len1, char *seq2, int len2, char **a1, char **a2);
double probability(int Nfixed, int len1, int len2, int patlen);
char *repchar(char ch, int N);
char *catN(char *str1, const char *str2, int N);
char *cat(char *str1, char *str2);
char *strdupN(const char *str, int N);
double factorial(int N);
double plongestsequence(int seqlen, int N);
int *longestascendingsequences(int *x, int N, int *lenret, int *Nret);
int *longestascendingsequence(int *x, int N, int *lenret);



int align(char *seq1, char *seq2, char *a1, char *a2)
{
  char *align1, *align2;
  int answer;
  
  answer = align_r(seq1, strlen(seq1), seq2, strlen(seq2), &align1, &align2, 6);
  if(answer == 0)
  {
    strcpy(a1, align1);
    strcpy(a2, align2);
  }
  else
  {
	 *a1 = 0;
	 *a2 = 0;
  }
  free(align1);
  free(align2);
  
  return answer;
}

int align_r(char *seq1, int len1, char *seq2, int len2, char **a1, char **a2, int patlen)
{
  char *align1 = 0;
  char *align2 = 0; 
  char *swapptr = 0; 
  MATCH *m;
  int Nfixed;
  int i;
  char *spaces = 0;
  int *pos1, *pos2;

  if(patlen == 1)
    return match1(seq1, len1, seq2, len2, a1, a2); 
  
  if(len1 > len2)
  {
	int temp;
	swapptr = seq1;
	seq1 = seq2;
	seq2 = swapptr;
	temp = len1;
	len1 = len2;
	len2 = temp;
  }
  if(len1 == 0)
  {
	if(!swapptr)
	{
      *a1 = repchar('-', len2);
      *a2 = strdupN(seq2, len2);
    }
    else
    {
	  *a2 = repchar('-', len2);
	  *a1 = strdupN(seq2, len2);
    }
    return 0;
  }
  
  m = getmatches(seq1, len1, seq2, len2, patlen);
  Nfixed = getgoodmatches(m, len1-patlen+1, &pos1, &pos2, patlen);

  if(probability(Nfixed, len1, len2, patlen) > 0.05)
	  Nfixed = 0;
  
  if(Nfixed > 0 && pos1[0] != 0 && pos2[0] == 0)
  {
    align1 = strdupN(seq1, pos1[0]);
    align2 = repchar('-', pos1[0]);
  }
  else if(Nfixed > 0 && pos1[0] == 0 && pos2[0] != 0)
  {
    align1 = repchar('-', pos2[0]);
    align2 = strdupN(seq2, pos2[0]);
  }
  else if(Nfixed > 0 && pos1[0] > 0 && pos2[0] > 0)
  {
    align_r(seq1, pos1[0], seq2, pos2[0], &align1, &align2, patlen-1); 
  }
  else if(Nfixed > 0 && pos1[0] == 0 && pos2[0] == 0)
  {
	align1 = strdupN("", 0);
	align2 = strdupN("", 0);
  }
  
  if(Nfixed == 0)
  {
    align_r(seq1, len1, seq2, len2, &align1, &align2, patlen-1);
  }
  for(i=0;i<Nfixed;i++)
  {
	int Nrun = 0;
	int j;
	char *sub1, *sub2;
	
	for(j=i+1;j<Nfixed;j++)
	  if(pos1[j-1] == pos1[j] - 1 && pos2[j-1] == pos2[j]-1)
	    Nrun++;
	  else
	    break;
	      
    align1 = catN(align1, seq1 + pos1[i], patlen + Nrun);
    align2 = catN(align2, seq2 + pos2[i], patlen + Nrun);

	if( j < Nfixed)
	{
      align_r(seq1+pos1[j-1] + patlen,
		    pos1[j] - pos1[j-1] - patlen > 0 ? pos1[j] - pos1[j-1] - patlen: 0, 
			seq2+pos2[j-1] + patlen, 
			pos2[j] - pos2[j-1] - patlen > 0 ? pos2[j] - pos2[j-1] - patlen : 0, 
			&sub1, 
			&sub2, 
			patlen-1);
      align1 = cat(align1, sub1);
      align2 = cat(align2, sub2); 
	}
    i += Nrun;
  }
  
  
  if(Nfixed > 0 && pos1[Nfixed-1] == len1-patlen && pos2[Nfixed-1] != len2 - patlen)
  {
	spaces = repchar('-', len2 - pos2[Nfixed-1] - patlen);
	align1 = cat(align1, spaces);
	align2 = catN(align2, seq2 + pos2[Nfixed-1]+patlen, len2 - pos2[Nfixed-1]-patlen);
  }
  else if(Nfixed > 0 && pos1[Nfixed-1] != len1-patlen && pos2[Nfixed-1] == len2 - patlen)
  {
    align1 = catN(align1, seq1 + pos1[Nfixed-1]+patlen, len1 - pos1[Nfixed-1]-patlen);
    spaces = repchar('-', len1 - pos1[Nfixed-1] - patlen);
    align2 = cat(align2, spaces);	  
  }
  else if(Nfixed > 0)
  {
	  char *sub1, *sub2;
 
	  align_r(seq1 + pos1[Nfixed-1] + patlen,
		      len1 - pos1[Nfixed-1] - patlen,
			  seq2 + pos2[Nfixed-1] + patlen,
			  len2 - pos2[Nfixed-1] - patlen,
			  &sub1, 
			  &sub2,
			  patlen-1);
	  align1 = cat(align1, sub1);
	  align2 = cat(align2, sub2);
  }
 
 
  if(swapptr)
  {
    *a2 = align1;
    *a1 = align2;
  }
  else
  {
    *a1 = align1;
    *a2 = align2;
  }
  
  killmatches(m, len1 - patlen + 1);
  free(pos1);
  free(pos2);
  return 0;
  error_exit:
    return -1;
  
}

MATCH *getmatches(char *seq1, int len1, char *seq2, int len2, int patlen)
{
  int i;
  char *ptr;
  char *sub;
  MATCH *answer;
  MATCH *prev;
  
  if(len1 < patlen || len2 < patlen)
  {
	  return 0;
  }
  answer = malloc((len1-patlen+1) * sizeof(MATCH));
  
  
  for(i=0;i<len1-patlen+1;i++)
  {
	 sub = strdupN(seq1 + i, patlen);
     ptr = strstr(seq2, sub);
     if(!ptr || ptr - seq2 >= len2 - patlen)
     {
       answer[i].pos = -1;
       answer[i].next = 0;
     }
     else
     {
       answer[i].pos = ptr - seq2;
       answer[i].next =0;
       ptr++;
       prev = &answer[i];
       while( (ptr = strstr(ptr, sub)) && ptr - seq2 <= len2 - patlen )
       {
         prev->next = malloc(sizeof(MATCH));
         prev->next->pos = ptr - seq2;
         prev->next->next = 0;
         prev = prev->next;
         ptr++;
       }
     }
     free(sub);
  }
  return answer;
}

void killmatches(MATCH *m, int N)
{
  int i;
  MATCH *prev, *next;
  
  if(m)
  {
    for(i=0;i<N;i++)
    {
      prev = m[i].next;
      while(prev)
      {
        next = prev->next;
        free(prev);
        prev = next;
      }
    }
  }
}

int getgoodmatches(MATCH *m, int N, int **pos1, int **pos2, int patlen)
{
  int *list;
  int Nlist;
  int Nascending;
  int i, j;
  MATCH *ptr;
  int *ascending;
  int *p1, *p2;
  int k = 0;
  int lastk;

  if(!m)
  {  
	 *pos1 = 0;
     *pos2 = 0;
	  return 0;
  }
  list = matchtolist(m, N, &Nlist);
  ascending = longestascendingsequence(list, Nlist, &Nascending);

  p1 = malloc(Nascending * sizeof(int));
  p2 = malloc(Nascending * sizeof(int));
  j = 0;
  for(i = 0; i < N; i++)
  {
	for(ptr = &m[i]; ptr;ptr=ptr->next)
	{
		if(ptr->pos == ascending[j])
		{
			if(k == 0 || 
				(p1[k-1] == i-1 && p2[k-1] == ptr->pos-1) ||
				(p1[lastk] <= i - patlen && p2[lastk] < ptr->pos - patlen))
			{
			  p1[k] = i;
			  p2[k] = ptr->pos;
			  
			  if(k == 0 || p1[k-1] != p1[k]-1 || p2[k-1] != p2[k]-1)
				  lastk = k;
			  k++;
			}
			j++;
			break;
		}
	}
  }
  *pos1 = p1;
  *pos2 = p2;

  free(list);
  free(ascending);

  return k;
}

int *matchtolist(MATCH *m, int N, int *lenret)
{
	int Nm =0;
	MATCH *ptr;
	int i, j, jj, k;
	int *answer;
	int temp;
	
	for(i=0;i<N;i++)
	  for(ptr = &m[i];ptr; ptr=ptr->next)
	    if(ptr->pos != -1)
	      Nm++;
    answer = malloc(Nm * sizeof(int));
    j = 0;
    for(i=0;i<N;i++)
    {
	  k = j;
      for(ptr = &m[i]; ptr; ptr=ptr->next)
        if(ptr->pos != -1)
          answer[j++] = ptr->pos;
	  
      for(jj=k;jj<k+(j-k)/2;jj++)
      {
	    temp = answer[jj];
	    answer[jj] = answer[j-jj+k-1];
	    answer[j-jj+k-1] = temp;
      }
    }
  
    *lenret = j;
  
  return answer;
}


int match1(char *seq1, int len1, char *seq2, int len2, char **a1, char **a2)
{
  char *align1, *align2;
  
  if(len1 == len2 && len1 <= 6)
  {
    align1 = strdupN(seq1, len1);
    align2 = strdupN(seq2, len2);
  }
  else
  {
    align1 = malloc(len1 + len2 + 1);
    memcpy(align1, seq1, len1);
    memset(align1 + len1, '?', len2);
    align1[len1+len2] = 0;
    align2 = malloc(len1 + len2 + 1);
    memset(align2, '?', len1);
    memcpy(align2+len1, seq2, len2);
    align2[len1+len2] = 0;
  }
  *a1 = align1;
  *a2 = align2;
  
  return 0;
}

double probability(int Nfixed, int len1, int len2, int patlen)
{
  double Ntries;
  double pmatch;

  if(Nfixed < 6)
	  return 1.0;
  Ntries = (len1-patlen+1) * (len2-patlen+1);
  pmatch = 1.0 /  pow(4, patlen);
  return plongestsequence(Nfixed, (int) (Ntries * pmatch) );
}

char *repchar(char ch, int N)
{
	char *answer;

  answer = malloc(N+1);
  if(answer)
  {
    memset(answer, ch, N);
    answer[N] = 0;
  }
  return answer;
}

char *catN(char *str1, const char *str2, int N)
{
  int len1;
  char *answer;
  
  len1 = strlen(str1);
  answer = malloc(len1 + N + 1);
  if(answer)
  {
	  strcpy(answer, str1);
	  memcpy(answer+len1, str2, N);
	  answer[len1+N] = 0;
  }
  free(str1);
  return answer;
}

char *cat(char *str1, char *str2)
{
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  char *answer = malloc(len1 + len2 + 1);
  
  if(answer)
  {
	memcpy(answer, str1, len1);
	strcpy(answer+len1, str2);
  }
  free(str1);
  free(str2);
  return answer;
}

char *strdupN(const char *str, int N)
{
  char *answer;
  
  answer = malloc(N+1);
  if(!answer)
    return 0;
  memcpy(answer, str, N);
  answer[N] = 0;
  
  return answer;
}

double factorial(int N)
{
  double answer = 1.0;
  int i;
  
  for(i=1;i<=N;i++)
    answer *= i;
    
  return answer;
}

static int oldsig[100] = {0, 0, 2, 3, 3, 3, 4, 4, 5, 6, 6, 6, 6, 6, 7, 7, 8, 9, 9,
9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 
13, 13, 13, 13, 14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 
16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 
20, 21, 21, 22, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24 };


static int sig[1000] = {0, 0, 2, 3, 3, 3, 4, 4, 5, 6, 6, 6, 6, 6, 7, 7, 8, 9, 9,
 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12,
13, 13, 13, 13, 13, 14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
16, 16, 16, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20,
20, 20, 21, 21, 22, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26,
26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
27, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30,
30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 34, 35, 35,
35, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 39, 39, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42,
42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46,
46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
50, 50, 50, 51, 51, 51, 51, 51, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54,
56, 56, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
59, 60, 60, 60, 60, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 63,
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 67, 67, 67, 67,
67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,
67, 68, 68, 68, 68, 68, 68, 68, 68, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 70,
70, 70, 70, 70, 70, 70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 72, 72, 72,
72, 72, 72, 73, 73, 73, 73, 74, 74, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 77,
77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
77, 77, 77, 77, 77, 78, 78, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 79, 79,
80, 81, 85, 85, 85, 85, 85, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,
87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 88, 88, 88, 88, 88, 88, 88,
88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 91,
91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 92, 92, 92,
92, 92, 92, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 94, 94, 94, 94, 94,
94, 94, 94, 94, 95, 95, 96, 97, 97, 97, 97, 98, 98, 100, 100, 100, 100, 100, 100, 
100, 100, 100, 100, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101,
101, 104, 104, 104, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 107,
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
107, 109, 109, 109, 109, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 111, 
111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 
113, 113, 113, 113, 113, 115, 115, 115, 119, 119, 119, 119, 119, 119, 119, 119, 
119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 
119, 120, 120, 120, 120, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 
122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 
122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 
122, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 125, 125, 125, 125, 125, 
125, 125, 125, 125, 125, 125, 125, 125, 125, 127, 127, 127, 127, 127, 127, 127, 
127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, };


double plongestsequence(int seqlen, int N)
{
  if(seqlen > N)
	 return 0;
  if(seqlen < 3)
	  return 1.0;
  if(N < 1000)
	return seqlen >= sig[N]/5 ? 0.01 : 1.0;
  else
	return seqlen >= N ? 0.01 : 1.0;
}


int *longestascendingsequence(int *x, int N, int *lenret)
{
	int *p;
	int *m;
	int L = -1;
	int i, j;
	int bestj;
	int *answer = 0;
	
	p = malloc(N * sizeof(int));
	m = malloc(N * sizeof(int));
	
	if(!m || !p)
	  goto out_of_memory;

	for(i=0;i<N;i++)
	{
	  bestj = -1;
	  for(j=0;j<=L;j++)
	    if(x[m[j]] < x[i])
	      bestj = j;
	 if(bestj != -1)
	    p[i] = m[bestj];
	 if(bestj == L || x[i] < x[m[bestj+1]])
	   m[bestj+1] = i;
	 L = L > bestj + 1 ? L : bestj+1;     
    }
	L++;
    answer = malloc(L * sizeof(int));
    if(lenret)
      *lenret = L;
    j = m[L-1];
    while(L--)
    {
      answer[L] = x[j];
      if(L)
        j = p[j];
    }
	free(m);
	free(p);
	return answer;
	
	out_of_memory:
	  free(m);
	  free(p);
	  free(answer);
	  if(lenret)
	    *lenret = 0;
	  return 0;
}

#define uniform() (rand()/(RAND_MAX+1.0))

void mem_swap(void *ptr1, void *ptr2, int N)
{
	unsigned char *p1 = ptr1;
	unsigned char *p2 = ptr2;
	unsigned char temp;
	int i;

	for(i=0;i<N;i++)
	{
		temp = p1[i];
		p1[i] = p2[i];
		p2[i] = temp;
	}
}

void shuffle(void *ptr, int width, int N)
{
	int target;
	int i;

	for(i=0;i<N-1;i++)
	{
      target = (int) (uniform() * (N-i)) + i;
	  mem_swap((unsigned char *)ptr + i*width, (unsigned char *)ptr + target*width, width); 
	}
}

char *randseq(int N, double gc)
{
	char *answer = malloc(N+1+100);
	int i;
	double p;

	if(!answer)
		return 0;
	memset(answer, 'X', N+1+100);
	for(i=0;i<N;i++)
	{
		p = uniform();
		if(p < gc)
		  answer[i] = p < gc/2.0 ? 'C' : 'G';
		else
		  answer[i] = (p < 1.0 - (1.0-gc)/2.0) ? 'A' : 'T';
	}
	answer[N] = 0;

	return answer;
}

int compints(const void *e1, const void *e2)
{
	const int *i1 = e1;
	const int *i2 = e2;

	return *i1 - *i2;
}

char *insert(char *seq, int N, int len)
{
  char *answer = malloc(strlen(seq) + N * len + 1);
  int t;
  int i, ii;

  strcpy(answer, seq);
  for(i=0;i<N;i++)
  {
	t = (int) (uniform() * strlen(answer));
    memmove(&answer[t+len], &answer[t], strlen(answer) - t + 1);
	for(ii=t;ii<t+len;ii++)
      answer[ii] = "ACGT"[rand() % 4];
  }
  
  return answer;
}

void pointmutate(char *seq, double p)
{
	int i;

	for(i=0;seq[i];i++)
	  if(uniform() < p)
		  seq[i] = "ACGT"[rand() % 4];

}

#include "fasta.h"

int fasta_record(FILE *fp, char *header, char *seq)
{
  int i;
  int len;
  
  if(header[0] == '>')
    fprintf(fp, "%s\n", header);
  else fprintf(fp, ">%s\n", header);
  len = strlen(seq);
  for(i=0;i<len;i++)
  {
    fputc(seq[i], fp);
    if( (i % 79) == 78)
      fputc('\n', fp);
  }
  if( (len % 79) != 0)
    fputc('\n', fp);
  return ferror(fp);
}


char *generatetestfasta()
{
  char *fname = "test.fasta";
  FILE *fp;
  char *seq1;
  char *seq2;
  char *mutseq;

  fp = fopen("test.fasta", "w");
  seq1 = randseq( 100 + (rand() % 100), 0.5);    
  seq2 = strdupN(seq1, strlen(seq1));
  mutseq = insert(seq1, 2, 5);
  pointmutate(mutseq, 0.1);
  fasta_record(fp, "seq1", seq1);
  fasta_record(fp, "seq2", mutseq);
  fclose(fp); 
  
}
void usage(void)
{
	printf("synteny_align : align two sequences using synteny method\n");
	printf("usage synteny_align <input.fasta>\n");
	printf("Fasta file must contain two sequences\n");
	printf("output to stdout\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int i;
	MATCH *m;
	MATCH *ptr;
	int N;
	int Nmatches;
	int *pos1, *pos2;
	int len;
	
	char *seq1;
	char *seq2;
	char *mutseq;
	int len1, len2;
	char *a1, *a2;
	
	int *x;
	int array[1000];
	int *seq;

	FASTA *fa;
	int err;

        if(argc == 1)
	{
          generatetestfasta();
	  return 0;
        }


	if(argc == 2)
	{
		fa = loadfasta(argv[1], &err);
		if(fasta_getNsequences(fa) != 2)
		{
			fprintf(stderr, "Fasta file must contain 2 sequences\n");
			exit(EXIT_FAILURE);
		}
		len1 = fasta_getlength(fa, 0);
		len2 = fasta_getlength(fa, 1);
		seq1 = malloc(len1 + 1);
		seq2 = malloc(len2 + 1);
		fasta_getsequence(fa, 0, seq1);
		fasta_getsequence(fa, 1, seq2);
	}
	a1 = malloc(len1+len2+1);
	a2 = malloc(len1+len2+1);

	align(seq1, seq2, a1, a2);
	fasta_record(stdout, fa->fs[0].header, a1);
	fasta_record(stdout, fa->fs[1].header, a2);
	free(a1);
	free(a2);
	free(seq1);
	free(seq2);
	return 0;
	/*

	x = malloc(10000 * sizeof(int));
	for(i=0;i<1000;i++)
	  array[i] = i+1;  

	printf("static int sig[1000] = {0, 0, "); 



	for(i=2;i<1000;i++)
	{
		int ii;
		for(ii=0;ii<1000;ii++)
		{
		  shuffle(array, i, sizeof(int));
		  seq = longestascendingsequence(array, i, &x[ii]);
		  free(seq);
		}
		qsort(x, 10000, sizeof(int), compints);
		printf("%d, ", x[9900]);
	}
	printf("};\n");
	getchar();
	*/

	/*
	for(i=0;i<100;i++)
	{
	seq1 = randseq( 100 + (rand() % 100), 0.5);
	//seq2 = randseq( 100 + (rand() % 100), 0.5);
	seq2 = strdupN(seq1, strlen(seq1));
	mutseq = insert(seq1, 2, 5);
	pointmutate(mutseq, 0.1);

	align(seq1, mutseq, a1, a2);
	printf("%s\n", a1);
	printf("%s\n", a2);

	free(seq1);
	free(seq2);
	free(mutseq);
	}
	getchar();
	return 0;
	*/
}
