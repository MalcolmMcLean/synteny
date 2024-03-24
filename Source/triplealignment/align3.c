char **align3(char *s1, char *s2, char *s3)
{
  
}

typedef struct
{
  char *s1;
  char *s2;
  char *s3;
  char *a12;
  char *a21;
  char *a13;
  char *a31;
  char *a23;
  char *a32;
  int s1i;
  int s2i;
  int s3i;
  int a12i;
  int a21i;
  int a13i;
  int a31i;
  int a23i;
  int a32i;

  char *out1;
  char *out2;
  char *out3;
} STATUS;

STATUS *createstatus()
{
}

void killstatus(STATUS *s)
{
}

int status_step(STATUS *st)
{
  int gap;
  gap = st_gapstate(st);

  switch(gap)
  {
  case 0:
    st_advance(st, 7);
    break;
  case 1:
    st_advance(st, 6);
    break;
  case 2:
    st_advance(st, 5);
    break;
  case 3:
    st_advance(st, 4);
    break;
  case 4:
    st_advance(st, 3);
    break;
  case 5:
    st_advance(st, 2);
    break;
  case 6:
    st_advance(st, 1);
    break;
  case 7:
    st_advance(st, 7);
    break; 
  }
  return 0;
}

void st_advance(STATUS *st, int fields)
{
  int advance12 = 0;
  int advance13 = 0;
  int advance32 = 0;

  if(fields & 0x01)
  {
    st->out1[st->outi] = st->s1[st->s1i]
  }
  else
  {
    if(st->a12[st->a12i] == '?' || st->s13[st->a13i] == '?')
      st->out1[st->outi] = '?';
    else
      st->out1[st->outi] = '-'; 
  }
  if(fields & 0x02)
  {
    st->out2[st->outi] = st->s2[st->s2i];
  }
  else
  {
    if(st->a21[st->a21i] == '?' || st->s23[st->s23i] == '?')
      st->out2[st->outi] = '?';
    else
      st->out[st->outi] = '-';
  }

  if(fields & 0x04)
  {
    st->out3[st->outi] = st->s3(st->s3i];
  }
  else
  {
    if(st->a31[st->a31i] == '?' || st->st32[st->s32i] == '?')
      st->out3[st->outi] = '?';
    else
      st->out3[st->outi] = '-';
  }

  if(fields & 0x01 )
  {
    if(is_letter(st->a12[st->a12i]))
      advance12 = 1;
    if(is_letter(st->a13[st->a13i]))
      advance13 = 1;  
  }
  if(fields & 0x02)
  {
    if(is_letter[st->a21[st->a21i]))
      advance12 = 1;
    if(is_letter[st->a23[st->a23i]))
      advance23 = 1;
  }
  if(fields & 0x04)
  {
    if(is_letter[st->a31[st->a31i]))
      advance13 =1;
    if(is_letter[st->a32[st->a32i]))
      advance 23 = 1;
  }

  if(advance12)
  {
    st->a12i++;
    st->a21i++;
  }
  if(advance13)
  {
    st->a13i++;
    st->a31i++;
  }
  if(advance23)
  {
    st->a23i++;
    st->a32i++;
  }
  st->outi++;
}


int st_gapstate(STATUS *st)
{
  int gap1, gap2, gap3;

  gap1 = is_letter(st->a12[st->a12i]) && is_letter(st->a21[st->a21i]);
  gap2 = is_letter(st->a13[st->a13i]) && is_letter(st->s31[st->s31i]);
  gap3 = is_letter(st->a23[st->a32i]) && is_letter(st->s32[st->a32i]);

  return (gap3 << 2) | (gap2 << 1) | gap1;
}

char *lastalignment(char *s1, char *s2, char *s3, char *a12, char *a21,
		    char *a13, char *a31, char *a23, char *a32)
{
  int p1, p2, p3;
  int pa12, pa13, pa23;
  
  gapstate = gapstate(a12[pa12], a21[pa12], a13[pa13], a31[pa31], a23[pa23], a32[pa23]);
  switch(gapstate)
  {
  case 0:  /* no gaps */
    a[0][p] = a12[pa12]; 
    a[1][p] = a21[pa12];
    a[2][p] = a13[pa13];
    pa12++;
    pa13++;
    pa23++;
    break;
  case 1: 
  case 2:
  case 3: /* gap in alignment 12 and 13, but not in 23 */
    do
    {
      a1[p] = '-';
      a2[p] = a23[p]
    }
    while(gapstate == 3);  
  case 4:
  case 5:
  case 6: 
    break;
  case 7: /*all gaps */
    a[0][p] = a12[pa12]; 
    a[1][p] = a21[pa12];
    a[2][p] = a13[pa13];
    pa12++;
    pa13++;
    pa23++;
    break;
  }
  p++;
 
   
}  

int gapstate(char a12, char a21, char a13, char a31, char a23, char a32)
{
  int gap1;
  int gap2;
  int gap3;
  
  gap1 = is_letter(a12) && is_letter(a21) ? 0 : 1;
  gap2 = is_letter(a13) && is_letter(a31) ? 0 : 1;  
  gap3 = is_letter(a23) && is_letter(a32) ? 0 : 1;

  return (gap3 << 2) | (gap2 << 1) | (gap1);
}

static int is_letter(int ch)
{
  return ch >= 'A' && ch <= 'z'; 
}
