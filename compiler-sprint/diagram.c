#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NUMBERUP(ROW, LO, HI) do {                              \
        int i;                                                  \
        int hi = (HI);                                          \
        int therow = (ROW);                                     \
        for (i = (LO); i <= hi; i++) {                          \
                if (!blotted(therow, i)) {                      \
                        if (numberable(therow, i, low_clue)     \
                            && low_clue <= high_clue)           \
                                blank(therow, i, low_clue++);   \
                        else if (numberable(therow, i, ANY))    \
                                return;                         \
                }                                               \
        }                                                       \
} while(0)

int ALLBLOT = 0;  /* permit a first row of all blots */
#define MAXN 25
#define WIDTH (MAXN + 2)

typedef enum square {UNKNOWN=0, ANY=253, BLANK=254, BLOT=255} Square;
Square grid[WIDTH][WIDTH];

static inline void blot   (int row, int col) 
{
        grid[row][col] = BLOT; 
}

static inline bool blotted(int row, int col) 

{
        return grid[row][col] == BLOT; 
}

static inline void blank  (int row, int col, int blank_or_number) 
{
        grid[row][col] = blank_or_number;
}


#define MAXCLUES 253
  
typedef char Flag;
Flag across[MAXCLUES], down[MAXCLUES];

#define NELEMS(A) (sizeof(A)/sizeof((A)[0]))

static int shortwords(int row, int col);


int N; 
int firstrow, firstcol;   /* if nonzero, the first blank */

#define symmetric(I) (N + 1 - (I))
unsigned K = 0;         /* show progress every K iterations */
int MINWORD = 3;        /* shortest numbered word */
static void numbergrid(int, int, int, int);
static int numberable(int, int, Square);
int main(int, char **);
static void parse(FILE *);
static int squares_remaining(int, int);
static void show(void);
static char *ascii_glyph(Square);     
static void ascii_newline(void);
static char *tex_glyph(Square);     
static void tex_newline(void);

char * (*glyph)(Square s) = ascii_glyph;
void (*newline)(void) = ascii_newline;

static void numbergrid(int row, int col, int low_clue, int high_clue) 
{
        if (row == 2 && low_clue == 1 && !ALLBLOT) return;
        {
                static unsigned long count = 0;
	        if (K > 0 && ++count >= K) {
                        printf("At %d, %d:\n", row, col);
                        show(); 
                        count = 0;
                }
        }
        if (high_clue - low_clue + 1 > squares_remaining(row, col)) {
                return;
        }
        if (col > N) {
                numbergrid(row + 1, 1, low_clue, high_clue);
        } else {
                int srow, scol, snum;
                srow = symmetric(row);
                scol = symmetric(col);
                snum = srow + 1;
                if (row > srow) {
                        NUMBERUP(row, 1, N);
                        if (low_clue == high_clue + 1) show();
                } else if (row == symmetric(row) && col > symmetric(col)) {
                        /*printf("Numbergrid up:\n"); show();*/
                        NUMBERUP(row, col, N);
                        NUMBERUP(snum, 1, col - 2);
                        if (low_clue == high_clue + 1) show();
                } else {
                        int newhi;
                        if (across[low_clue] 
                            && (row != firstrow || col != firstcol) 
                            && !across[grid[row][col-1]]
                            && !down[grid[row-1][col]] /* no 1-letter words */
                            && (newhi = high_clue,
                                blot(srow, scol), 
                                blotted(snum, scol) 
                                ? 1   /* not blank; no numbergrid issues */
                                : blotted(snum, scol - 1) 
                                ? across[newhi] && down[newhi] /* OK to number */
                                ? (grid[snum][scol] = newhi--, 1)
                                : 0  /* must number, but newhi won't work here */
                                : !across[newhi] && down[newhi] /* OK to number */
                                ? (grid[snum][scol] = newhi--, 1)
                                : 0  /* must number, but newhi won't work here */
                                    )
                            && (MINWORD <= 2 || !shortwords(row, col)) 
                                ) {
                                blot(row, col);
                                numbergrid(row, col + 1, low_clue, newhi);
                                {
                                        if (!blotted(snum, scol))
                                        	blank(snum, scol, BLANK);
                                }
                        }
                        if (row > firstrow || col >= firstcol) {
                                if ((newhi = high_clue,
                                     blank(srow, scol, BLANK),
                                     blotted(snum, scol) 
                                     ? 1   /* not blank; no numbergrid issues */
                                     : blotted(snum, scol - 1) 
                                     ? across[newhi] && !down[newhi] /* OK to number */
                                     ? (blank(snum, scol, newhi--), 1)
                                     : 0  /* must number, but newhi won't work here */
                                     : 1 /* no blot to left, therefore no number needed */
                                            )) {
                                        if (numberable(row, col, low_clue)
                                            && low_clue <= high_clue) {
                                                blank(row, col, low_clue);
                                                numbergrid(row,
                                                           col + 1,
                                                           low_clue + 1,
                                                           newhi);
                                        } else if (!numberable(row, col, ANY)) {
                                                blank(row, col, BLANK);
                                                numbergrid(row,
                                                           col + 1,
                                                           low_clue,
                                                           newhi);
                                        }
                                        { if (!blotted(snum, scol))
                                                        blank(snum,
                                                              scol,
                                                              BLANK); }
                                }
                        }
                        grid[row][col] = grid[srow][scol] = UNKNOWN;
                }
        }
}
static int numberable(int row, int col, Square n) 
{
        int l = blotted(row, col - 1);
        int u = blotted(row - 1, col);
        if (!l && !u) return 0;
        else if (n == ANY) return 1;
        else {
                if (across[n]) 
                {if (!l || col == N) return 0;}
                else
                {if (l) return 0;}
                if (down[n]) 
                {if (!u || row == N) return 0;}
                else
                {if (u) return 0;}
                return 1;
        }
}
static int shortwords(int row, int col) 
{
        int i;
        if (!blotted(row, col - 1)) {
                for (i = col - 1; !blotted(row, i); i--);
                if (col - i - 1 < MINWORD) return 1;
        }
        if (!blotted(row - 1, col)) {
                for (i = row - 1; !blotted(i, col); i--);
                if (row - i - 1 < MINWORD) return 1;
        }
        return 0;
}
int main(int argc, char *argv[])
{
        int i;
        FILE *in;
        if (argc > 1 && !strcmp(argv[1], "-t")) {
                glyph = tex_glyph;
                newline = tex_newline;
                argv[1] = argv[0];
                argv++;
                argc--;
        }
        assert(((void)"Usage: diagram [-t] N [file]", argc >= 2));
        { char *Kvar;
                Kvar = getenv("K");
                if (Kvar != NULL) K = atoi(Kvar);
                        }
        { char *MINWORDvar;
                MINWORDvar = getenv("MINWORD");
                if (MINWORDvar != NULL) MINWORD = atoi(MINWORDvar);
        }
        N = atoi(argv[1]);
        for (i = 0; i <= N + 1; i++) /* sentinels */
                grid[0][i]
                        = grid[i][0]
                        = grid[N + 1][i]
                        = grid[i][N + 1]
                        = BLOT;
        { char *s = getenv("ALLBLOT");
                if (s != NULL) ALLBLOT = 1;
        }
        if (argc > 2)
                for (i = 2; i < argc; i++) {
                        in = fopen(argv[i], "r");
                        if (!in) {
                                fprintf(stderr, "Couldn't open %s\n", argv[i]);
                                exit(1);
                        }
                        parse(in);
                }
        else
                parse(stdin);
        { unsigned hi;
                for (hi = 1;
                     hi < NELEMS(across) && (across[hi] || down[hi]);
                     hi++);
                if (!across[hi] && !down[hi]) hi--;
                numbergrid(1, 1, 1, hi);
        }
        return 0;
}

static void parse(FILE *in) 
{
        char buf[255];
        Flag *direction = NULL;  
        while (fscanf(in, "%s", &buf[0]) == 1)
                if (!strcmp(buf, "across")) 
                        direction = across;
                else if (!strcmp(buf, "down"))
                        direction = down;
                else if (!strcmp(buf, "first")) {
                        if (fscanf(in, "%d,%d", &firstrow, &firstcol) != 2)
                        {fprintf(stderr, "Malformed `first' line\n"); exit(1); }
                } else {
                        char *last;
                        long n;
                        n = strtol(buf, &last, 0);
                        if (*last != 0) {fprintf(stderr, "Bad digit in %s\n", buf); exit(1);}
                        assert(direction != NULL);
                        direction[n] = 1;
                }
        if (!feof(in)) {fprintf(stderr, "Did not reach end of file\n"); exit(1); }
}

static int squares_remaining(int row, int col)
{
        int rowsquares = N * (N - 2 * row);
        int n = (row == symmetric(row) ? 1 : 2) * (N - col + 1);
        return rowsquares > 0 ? n + rowsquares : n;
}

static void show(void) 
{
        int i, j;
        for (i = 1; i <= N; i++) {
                for(j = 1; j <= N; j++)
                        printf("%s", glyph(grid[i][j]));
                newline();
        }
        printf("\n");
}

static void ascii_newline(void)
{
        printf("\n");
}

static char *ascii_glyph(Square s) 
{
        static char buf[255];
        switch (s) {
        case UNKNOWN : return " ??";
        case ANY     : return " **";
        case BLANK   : return "   ";
        case BLOT    : return "###";
        default      :
                sprintf(buf, "%3d", s);
                return buf;
        }
}

static void tex_newline(void)
{
        printf("\\\\\n"); 
}

static char *tex_glyph(Square s) 
{
        static char buf[255];
        switch (s) {
        case UNKNOWN : return "\\?";
        case ANY     : return "\\sq{\\textsc{any}}";
        case BLANK   : return "\\b";
        case BLOT    : return "\\#";
        default      :
                sprintf(buf, "\\n{%d}", s);
                return buf;
        }
}
