#ifndef fasta_h
#define fasta_h

#define FASTA_UNKNOWN 0
#define FASTA_DNA 1
#define FASTA_RNA 2
#define FASTA_PROTEIN 3
#define FASTA_XDNA 4
#define FASTA_XRNA 5
#define FASTA_XPROTEIN 6

typedef struct
{
  char *header;       /* the FASTA header */
  char *comment;      /* comments (start with semicolon) */
  char *seq;          /* the sequence, includes gaps */
  int seqlen;         /* length of sequence (excluding gaps) */
  int gappedlen;      /* length of the sequence, including gaps */
  int type;           /* DNA/RNA/PROTEIN/UNKNOWN */
} FASTASEQ;

typedef struct
{
  int N;             /* number of sequences */
  FASTASEQ *fs;      /* the sequences */
} FASTA;

FASTA *loadfasta(char *fname, int *err);
FASTA *floadfasta(FILE *fp, int *err);
void killfasta(FASTA *fa);
int fasta_getNsequences(FASTA *fa);
void fasta_getsequence(FASTA *fa, int index, char *out);
void fasta_getgappedsequence(FASTA *fa, int index, char *out);
int fasta_getlength(FASTA *fa, int index);
int fasta_getgappedlength(FASTA *fa, int index);
int fasta_gettype(FASTA *fs, int index);

#endif
