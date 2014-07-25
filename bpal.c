/* 
Converted to Xcode by Doug Clayton
bpal.c -- reverse and add palindromes 

   usage: >bpal
       user prompted for:
       -starting number
       -range
       -number of iterations

   output to console and to the file palin.txt
   palin.txt is overwritten each run
   
   
   
*/
import Cocoa


let  BASE: int = 10
let START_SIZE: int 1000 /* max 999 digits for starting number */
enum {NEG, POS};

typedef struct bigint {
     int sign;
     int size;
     int initflag; 
     char *number;
} bigint;

void  bigInit(bigint *a);
void  bigAlloc(bigint *a, int bytes);
void  bigAssign(bigint *a, bigint b); 
void  str2big(bigint *a, char *string);
void  bigPrint(bigint a);
char* dig2str(int *digits, int ndigits);
void  strip_zeroes(char *s);
void  bigAdd(bigint *sum, bigint addend, bigint augend);
void  int2big(bigint *a, int n);
char* reverse(char *s);

int main(void)
{
    bigint num, numb, revnum, sum;
    bigint revsum, one, end, brange;
    char start[START_SIZE];
    int limit, iter, range, idx;
    int count, hi_iter, lo_iter, fileflag;
    FILE *output;

    bigInit(&num);
    bigInit(&numb);
    bigInit(&revnum);
    bigInit(&sum);
    bigInit(&revsum);
    bigInit(&one);
    bigInit(&end);
    bigInit(&brange);
       
    printf("enter starting number:\n");
    scanf("%s", &start);
    printf("enter range:\n");
    scanf("%d", &range);
    printf("enter number of iterations:\n");
    scanf("%d", &limit);

    str2big(&one, "1");
    str2big(&numb, start);
    str2big(&end, start);
    int2big(&brange, range);
    bigAdd(&end, end, brange);
    count = 0;
    hi_iter = 0;
    lo_iter = 10000;
    fileflag = 1;

    if((output = fopen("palin.txt", "w"))==NULL)
    {
        fileflag = 0;
    }
  
    /* reverse, add and check for palindromicity */
    printf("\n");
    for(idx = 0; idx < range; idx++)
    {
      iter = 0;
      bigAssign(&num, numb);
      while(iter++ < limit)
      {
        revnum.number = reverse(num.number);
        bigAdd(&sum, num, revnum);
        free(revnum.number);
        revsum.number = reverse(sum.number);
        if(strcmp(revsum.number, sum.number)==0) 
        {
            count++;
            bigPrint(numb);
            bigPrint(sum);
            printf("%d iterations\n\n", iter);
            if(fileflag)
            {
                fprintf(output, "%s\n", numb.number);
                fprintf(output, "%s\n", sum.number);
                fprintf(output, "%d iterations\n\n", iter);
            }
            bigAdd(&numb, numb, one);
            if(iter > hi_iter) hi_iter = iter;
            if(iter < lo_iter) lo_iter = iter;
            break;
        }
        free(revsum.number);
        bigAssign(&num, sum);
        if(iter == limit)
        {
            bigAdd(&numb, numb, one);
            break;
        }
      }
    }
    if(fileflag)
    {
        fprintf(output,"%d of %d numbers in the range\n", count, range);
        fprintf(output,"%s to %s\n", start, end.number);
        fprintf(output,"formed palindromes in %d or fewer iterations\n",limit);
        fprintf(output,"high iterations: %d\n", hi_iter);
        fprintf(output,"low iterations: %d\n", lo_iter);
        fclose(output);
    }
    printf("%d of %d numbers in the range\n", count, range);
    printf("%s to %s\n", start, end.number);
    printf("formed palindromes in %d or fewer iterations\n", limit);
    printf("high iterations: %d\n", hi_iter);
    printf("low iterations: %d\n", lo_iter);

    if(!fileflag)
    {
        fprintf(stderr, "\noutput file could not be opened\n");
        fprintf(stderr, "output to console only\n");
    }

    return 0;
}

/* reverse the sum */
char* reverse(char *s)
{
    int length, idx, jdx, last;
    char digit;
    char* newstr;

    length = strlen(s) + 1; 
    if((newstr = malloc(length))==NULL)
    {
        fprintf(stderr, "malloc failed in reverse()\n");
        exit(EXIT_FAILURE);
    }
    jdx = length - 2;
    last = length - 1;
    for(idx = 0; idx < length; ++idx, --jdx)
    {
        digit = s[jdx];
        newstr[idx] = digit;
    }
    newstr[last] = '\0'; 
  
    return newstr;
}

/* ----- big integer arithmetic routines --------------- */

/* initialize a bigint to zero */
void bigInit(bigint *a)
{
    if((a->number = malloc(2))==NULL)
    {
        fprintf(stderr, "malloc failed in bigInit()\n");
        exit(EXIT_FAILURE);
    }
    a->sign = POS;
    a->size = 1;
    a->initflag = 1; 
    a->number[0] = '0';
    a->number[1] = '\0';
    return;
}

/* allocate memory for a bigint */
void bigAlloc(bigint *a, int bytes)
{
    if(a->initflag != 1)
    {
        bigInit(a);
    }

    free(a->number);
    if((a->number = malloc(bytes * sizeof(*a->number)))==NULL)
    {
        fprintf(stderr, "bigAlloc failed\n");
        exit(EXIT_FAILURE);
    }
    return;
} 

/* bigint assignment: a = b */
void bigAssign(bigint *a, bigint b) 
{
    bigAlloc(a, b.size+1);
    a->sign = b.sign;
    a->size = b.size;
    strcpy(a->number, b.number);
    return;
}

/* convert a string to a bigint */
void str2big(bigint *a, char *string)
{
    int len, idx;
    
    len = strlen(string);
    if(string[0] == '-' || string[0] == '+') 
    {
        for(idx = 1; idx < len; idx++)
        {
            if(!isdigit(string[idx]))
            {
                fprintf(stderr, "Error: non-numeric input to str2big()\n");
                exit(EXIT_FAILURE);
            }
        }
        if(string[0]=='-') a->sign = NEG;
        if(string[0]=='+') a->sign = POS;
        bigAlloc(a, len);
        assert((sizeof(a->number)*len) >= strlen(string)+1);
        strcpy(a->number, string+1);
        a->size = strlen(a->number);
        return;
    }
    else /* a string with no leading sign is assumed to be positive */
    {
        for(idx = 0; idx < len; idx++)
        {
            if(!isdigit(string[idx]))
            {
                fprintf(stderr, "Error: non-numeric input to str2big()\n");
                exit(EXIT_FAILURE);
            }
        }
        a->sign = POS;
        bigAlloc(a, len+1); 
        assert((sizeof(a->number)*len+1) >= strlen(string)+1);
        strcpy(a->number, string);
        a->size = strlen(a->number);
        return;
    }
}

/* print a bigint */
void bigPrint(bigint a)
{
    if(a.sign == POS)
    {
        printf("%s\n", a.number);
    }
    else if(a.sign == NEG)
    {
        printf("-%s\n", a.number);
    }
    else
    {
        printf("?%s\n", a.number);
    }
    return;
}

/* convert array of single digit ints to a string */
/* called by bigAdd() */
char* dig2str(int *digits, int ndigits)
{
    char* bigstring;
    int idx = 0;

    if((bigstring = calloc(ndigits + 1, sizeof(char)))==NULL)
    {
        fprintf(stderr, "calloc failed in dig2str()\n");
        exit(EXIT_FAILURE);
    }
    while(idx < ndigits)
    {
        bigstring[idx++] = '0' + *digits++;
    }
    bigstring[idx] = '\0';

    return bigstring;
} 

/* strip leading zeroes */
/* called by bigAdd */
void strip_zeroes(char *s)
{
    char *stripped;
    int idx, jdx, zero_count, len;
    
    zero_count = 0;
    idx = 0;
    len = strlen(s) + 1;

    while(s[idx] == '0')
    {
        zero_count++;
        idx++;
    }
    if((stripped = malloc((len-zero_count) * sizeof(*stripped)))==NULL)
    {
        fprintf(stderr, "malloc failed in strip_zeroes\n");
        exit(EXIT_FAILURE);
    }
    for(idx = zero_count, jdx = 0; idx < len; idx++, jdx++)
    {
        stripped[jdx] = s[idx];
    }
    if((s = realloc(s, (len - zero_count) * sizeof(*s)))==NULL)
    {
        fprintf(stderr, "realloc failed in strip_zeroes()\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(s, stripped);
    free(stripped);
    return;
}

/* convert an int to a bigint */
void int2big(bigint *a, int n)
{
    int ndigits;
    
    if(n < 0)
    {
        a->sign = NEG;
        n *= -1; /* positive input to log10() */
    }
    else
    {
        a->sign = POS;
    }

    if(n == 0)
    {
        bigAlloc(a, 2);
        a->number[0] = '0';
        a->number[1] = '\0';
    }
    else
    {
        ndigits = (int)(log10(n)) + 1;
        bigAlloc(a, ndigits+1);
        sprintf(a->number, "%d", n);
    }
    a->size = strlen(a->number);
    return;
}

/* addition */
void bigAdd(bigint *sum, bigint addend, bigint augend)
{
    int idx, jdx, digits;
    int len_add, len_aug;
    int lsd_add, lsd_aug, lsd_sum;
    int sum_dig, carry;
    int *add_digits, *aug_digits, *sum_digits;
    char *bsum;

    len_add = strlen(addend.number);
    len_aug = strlen(augend.number);
    
    /* make both operand arrays the
       length of the longest operand */
    if(len_add == len_aug)
    {
        digits = len_add;
    }
    else if(len_add > len_aug)
    {
        digits = len_add;
    }
    else
    {
        digits = len_aug;
    }

    add_digits = calloc(digits, sizeof(*add_digits));
    aug_digits = calloc(digits, sizeof(*aug_digits));
    sum_digits = calloc((digits + 1), sizeof(*sum_digits));
    if(add_digits==NULL || aug_digits==NULL || sum_digits==NULL)
    {
        fprintf(stderr, "calloc failed in bigAdd()\n");
        exit(EXIT_FAILURE);
    }

    lsd_add = digits-1;
    lsd_aug = digits-1;
    lsd_sum = digits;
  
    /* load operand arrays */
    /* shorter operand will be padded with zeroes */
    if(len_add < digits) /* padded */
    {
        int op_dig_ctr = lsd_add;
        for(idx = len_add - 1; idx >= 0; --idx)
        {
            add_digits[op_dig_ctr--] = addend.number[idx] - '0';
        }
    }
    else /* not padded */
    {  
        for(idx = lsd_add; idx >= 0; --idx)
        {
            add_digits[idx] = addend.number[idx] - '0';
        }
    }
  
    if(len_aug < digits) /* padded */
    {
        int op_dig_ctr = lsd_aug;
        for(idx = len_aug - 1; idx >= 0; --idx)
        {
            aug_digits[op_dig_ctr--] = augend.number[idx] - '0';
        }
    }
    else /* not padded */
    {
        for(idx = lsd_aug; idx >= 0; --idx)
        {
            aug_digits[idx] = augend.number[idx] - '0';
        }
    }
      
    /* do the addition */
    carry = 0;
    for(idx=lsd_sum, jdx=lsd_add; idx >=1; idx--, jdx--)
    {
        sum_dig = add_digits[jdx] + aug_digits[jdx] + carry;
        if(sum_dig < BASE)
        {
            sum_digits[idx] = sum_dig;
            carry = 0;
        }
        else
        {
            sum_digits[idx] = sum_dig % BASE;
            carry = 1;
        }
    }
    sum_digits[0] = carry;
  
    /* convert sum array to a string */
    bsum = dig2str(sum_digits, digits + 1);

    bigAlloc(sum, strlen(bsum)+1);
    strcpy(sum->number, bsum);
    strip_zeroes(sum->number);
    sum->size = strlen(sum->number);

    free(add_digits);
    free(aug_digits);
    free(sum_digits);
    
    return;
}
 