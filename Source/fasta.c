/*
  fasta.c

  code for loading a fasta format file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "fasta.h"

static void assigntypes(FASTA *fa);
static void fixuplengths(FASTA *fa);
static int seqprotein(char *str);
static int seqnucleic(char *str);
static void filterspaceanddigits(char *str);
static int strcount(char *str, int ch);
static char *readline(FILE *fp);

/*
  load a fasta file by name
  Params: fname - name of file to load
          err - return for error code (can be 0)
  Returns: loaded object, 0 on fail 
  Notes: errors 0 OK
                -1 out of memory
                -2 can't open file
                -3 parse error
         
 */
FASTA *loadfasta(char *fname, int *err)
{
  FILE *fp;
  int error;
  FASTA *answer;

  fp = fopen(fname, "r");
  if(!fp)
  {
    error = -2;
    printf("Not opened\n");
    goto error_exit; 
  }
  answer = floadfasta(fp, &error);
  if(!answer)
    goto error_exit;

  fclose(fp);
  if(err)
    *err = 0;
  return answer;

 error_exit:
  if(fp)
    fclose(fp);
  if(err)
    *err = error;
  return 0;
}

/*
  load fasta file from a opened stream
  Params: fp - pointer to open file
          err - return pointer for error
  Returns: loaded object

*/
FASTA *floadfasta(FILE *fp, int *err)
{
  FASTA *answer;
  FASTASEQ *temp;
  int error;
  char *line;
  int len;
  char *ptr;

  answer = malloc(sizeof(FASTA));
  if(!answer)
  {
    error = -1;
    goto error_exit;
  }
  answer->N = 0;
  answer ->fs = 0;

  while ((line = readline(fp)))
  {
    /* header */
    if(line[0] == '>')
    {
      temp = realloc(answer->fs, (answer->N + 1) * sizeof(FASTASEQ));
      if(!temp)
      {
        error = -1;
        goto error_exit;
      }
      answer->fs = temp;
      answer->fs[answer->N].header = line;
      answer->fs[answer->N].comment = 0;
      answer->fs[answer->N].seq = 0;
      answer->fs[answer->N].seqlen = 0;
      answer->fs[answer->N].gappedlen = 0;
      answer->N++;
    }
    else if(line[0] == ';')
    {
      if(answer->N)
      {
        if(answer->fs[answer->N-1].comment == 0)
          answer->fs[answer->N-1].comment = line;
        else
	{
          len = strlen(answer->fs[answer->N-1].comment);
          len += strlen(line);
          ptr = malloc(len + 3);
          if(!ptr)
	  {
            error = -1;
            goto error_exit;
	  }
          strcpy(ptr, answer->fs[answer->N-1].comment);
          strcat(ptr, "\n");
          strcat(ptr, line);
          strcat(ptr, "\n");
          free(line);
          free(answer->fs[answer->N-1].comment);
          answer->fs[answer->N-1].comment = ptr;
	}
      }
    }
    else
    {
      if(answer->N)
      {
        filterspaceanddigits(line);
        if(answer->fs[answer->N-1].seq == 0)
	{
          answer->fs[answer->N-1].seq = line;
	  answer->fs[answer->N-1].seqlen = strlen(line);
        }
        else
	{
          len = answer->fs[answer->N-1].seqlen;
          len += strlen(line);
          ptr = malloc(len+1);
          if(!ptr)
	  {
            error = -1;
            goto error_exit;
	  }
          strcpy(ptr, answer->fs[answer->N-1].seq);
          strcat(ptr, line);
          free(line);
          free(answer->fs[answer->N-1].seq);
          answer->fs[answer->N-1].seq = ptr;
          answer->fs[answer->N-1].seqlen = len;
	}
      }
      else
      {
        free(line);
        error = -3;
        goto error_exit;
      }
    }   
  } 
  if(!feof(fp))
  {
    error = -1;
    goto error_exit;
  } 

  assigntypes(answer);
  fixuplengths(answer);

  if(err)
    *err = 0;
  return answer;
  
 error_exit:
  if(answer)
    killfasta(answer);
  if(err)
    *err = error;
  return 0;
}

/*
  destroy a fasta file object
  Params: fa - the object to destroy
 */
void killfasta(FASTA *fa)
{
  int i;

  if(fa)
  {
    if(fa->fs)
    {
      for(i=0;i<fa->N;i++)
      {
        free(fa->fs[i].header);
	free(fa->fs[i].comment);
        free(fa->fs[i].seq);
      }
      free(fa->fs);
    }
    free(fa);
  }
}

/*
  get the number of sequences in the file
  Params: fa - the fasta object
  Returns: number of sequences

 */
int fasta_getNsequences(FASTA *fa)
{
  return fa->N;
}
/*
  retrieve an ungapped sequence from a fasta file
  Parmas: fa - the fasta object
          index - index of sequence to retrieve
          out - buffer to receive sequence
 */
void fasta_getsequence(FASTA *fa, int index, char *out)
{
  int i;

  assert(index >= 0 && index < fa->N);

  for(i=0;fa->fs[index].seq[i];i++)
  {
    if(fa->fs[index].seq[i] != '-')
      *out++ = fa->fs[index].seq[i];
  }
  /* knock off trailing * terminator */
  if(i > 0 && *(out-1) == '*')
    out--;
  *out = 0;
}

/*
  retrieve sequence from afasta file, preserving gaps (-)
  Params: fa - the fasta object
          index - index of sequence to retrieve
          out - buffer to receive sequence
 */
void fasta_getgappedsequence(FASTA *fa, int index, char *out)
{
  assert(index >= 0 && index < fa->N);
  strcpy(out, fa->fs[index].seq);
}

/*
  get the length of a sequence (excluding gaps)
  Params: fa - the fasta object
          index - index of sequence
  Returns: length of sequence
 */
int fasta_getlength(FASTA *fa, int index)
{
  assert(index >= 0 && index < fa->N);
  
  return fa->fs[index].seqlen;
}

/*
  get the length of a sequence, including gaps
  Params: fa - the fata object
          index - index of object   
  Returns: length of the gapped sequence
 */
int fasta_getgappedlength(FASTA *fa, int index)
{
  assert(index >= 0 && index < fa->N);

  return fa->fs[index].gappedlen;
}

/*
  get type of sequence
  Params: fa - the fasta object
          index - index of sequence
  Returns: type of sequence it is, DNA, RNA, Protein
  Notes: have to assign intelligently
 */
int fasta_gettype(FASTA *fa, int index)
{
  assert(index >= 0 && index < fa->N);

  return fa->fs[index].type;
}




/*
  intelligently work out type 
 */
static void assigntypes(FASTA *fa)
{
  int i;
  int ii;
  int protein;
  int nucleic;
  int dna;
  int rna;
  int extended;

  for(i=0;i<fa->N;i++)
  {
    protein = seqprotein(fa->fs[i].seq);
    nucleic = seqnucleic(fa->fs[i].seq);
    fa->fs[i].type = FASTA_UNKNOWN;
    if(!protein && !nucleic)
      fa->fs[i].type = 0;
    if(protein)
    {
      for(ii=0;ii<fa->fs[i].seqlen;ii++)
        if(strchr("BUZX*", fa->fs[i].seq[ii]))
	{
          fa->fs[i].type = FASTA_XPROTEIN; 
          break;
        }
      if(ii == fa->fs[i].seqlen)
        fa->fs[i].type = FASTA_PROTEIN;
     
      /* if are canonical with trailing *, it is normal protein */
      if(ii == fa->fs[i].seqlen -1 && fa->fs[i].seq[ii] == '*')
        fa->fs[i].type = FASTA_PROTEIN;
    }
    if(nucleic)
    {
      rna = 0;
      dna = 0;
      extended = 0;

      for(ii=0;ii<fa->fs[i].seqlen;ii++)
      {
        if(fa->fs[i].seq[ii] == 'U')
          rna = 1;
        if(fa->fs[i].seq[ii] == 'T')
          dna = 1;
        if(!strchr("ACGTU-", fa->fs[i].seq[ii]))
          extended = 1;
      }
      if(protein == 1 && dna == 1 && rna == 1)
      {
        /* it's a protein that looks like DNA/RNA */
      }
      /* if we are both a protein and an extende, flag as unknown */
      else if(protein == 1 && extended == 1)
        fa->fs[i].type = 0; 
      else if( dna == 1 && rna == 0 && extended == 0)
        fa->fs[i].type = FASTA_DNA;
      else if( rna == 1 && dna == 0 && extended == 0)
        fa->fs[i].type = FASTA_RNA;
      else if(dna == 1 && extended == 1 && rna == 0)
        fa->fs[i].type = FASTA_XDNA;
      else if(rna == 1 && extended == 1 && dna == 0)
        fa->fs[i].type = FASTA_XRNA;
      else
        fa->fs[i].type = 0; /* dna/rna mix */
    }
  }
}

/*
  adjust the legnths to account for gaps
  Params: fa - fasta object
 */
static void fixuplengths(FASTA *fa)
{
  int i;
  int gaps;

  for(i=0;i<fa->N;i++)
  {
    gaps = strcount(fa->fs[i].seq, '-');
    fa->fs[i].gappedlen = fa->fs[i].seqlen;
    fa->fs[i].seqlen -= gaps;
    /* knock off trailing '*' */
    if(fa->fs[i].seqlen > 0 && fa->fs[i].seq[fa->fs[i].seqlen-1] == '*')
      fa->fs[i].seqlen--;
  }
}

/*
  is the sequence a valid protein ?
 */
static int seqprotein(char *str)
{
  while(*str)
    if(!strchr("ABCDEFGHIKLMNPQRSTUVWYZX*-", *str++))
      return 0;
  return 1;
}

/*
  is the sequence a valid nucleic acid ?
 */
static int seqnucleic(char *str)
{
  while(*str)
    if(!strchr("ACGTURYKMSWBHVN-", *str++))
      return 0;
 
  return 1;
}

/*
  filter out whitespace and digits, 
   also put characters into upper case#
  Params: str - string to filter
 */
static void filterspaceanddigits(char *str)
{
  char *wptr;
  char *rptr;

  wptr = str;
  rptr = str;
  do
  {
    if(!isspace(*rptr) && !isdigit(*rptr))
    {
      *wptr++ = toupper(*rptr);
    }
  } while(*rptr++);

}

/*
  count the instances * character in string
  Params: str - the string
          ch - the character to test
  Returns: no times ch appears in str 
 */
static int strcount(char *str, int ch)
{
  int answer = 0;
  
  while(*str)
  {
    if(*str++ == ch)
      answer++;
  }

  return answer;
}

/*
  readline()  read  aline from an input file
Params:  pointer to an opened file
Returns: line read up to newline, 0 on out-of-memory or EOF.
Notes: trailing newline stripped. Return allocated with malloc().
*/
static char *readline(FILE *fp)
{
  char *buff;
  int nread = 0;
  int buffsize = 128;
  int ch;
  char *temp;

  buff = malloc(128);
  if(!buff)
    return 0;
  
  while((ch = fgetc(fp)) != '\n')
  {
    if(ch == EOF)
    {
      if(nread == 0)
	{
          free(buff);
          return 0;
	}
      break;
    }

    buff[nread] = (char) ch;
    nread++;
    if(nread == buffsize - 1)
    {
      temp = realloc(buff, buffsize + 64);
      if(!temp)
      {
        free(buff);
        return 0;
      }   
      buff = temp;
      buffsize += 64;
    }
  }

  buff[nread] = 0;
  return buff;
}


int fastamain(int argc, char **argv)
{
  int i;
  int ii;
  FASTA *fasta;
  char seq[2048];
  int error;

  for(i=1;i<argc;i++)
  {
    fasta = loadfasta(argv[i], &error);
    if(!fasta)
    {
      printf("Error %d\n", error);
      exit(EXIT_FAILURE);
    }
    printf("%d sequences\n", fasta->N);
    for(ii=0;ii<fasta->N;ii++)
    {
      switch(fasta->fs[ii].type)
      {
      case FASTA_UNKNOWN:
        printf("Unknown type ");
        break;
      case FASTA_DNA:
        printf("DNA ");
        break;
      case FASTA_RNA:
        printf("RNA ");
        break;
      case FASTA_PROTEIN:
        printf("Protein ");
        break;
      case FASTA_XDNA:
        printf("Extended DNA");
        break;
      case FASTA_XRNA:
        printf("Extended RNA");
        break;
      case FASTA_XPROTEIN:
        printf("Extended Protein");
        break;
      default:
        printf("Error in type ");
        break;
      }
      printf("length %d\n", fasta->fs[ii].seqlen);   
      if(fasta->fs[ii].seqlen < 2048)
      {
        fasta_getsequence(fasta, ii, seq);
        printf("%s\n", seq);
      }
    }
    killfasta(fasta);
  }

  return 0;
}
