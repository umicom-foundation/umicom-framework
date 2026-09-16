/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/numeric_parsing/main.c
 *
 * PURPOSE:
 *   Parsing numbers without accepting junk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Strict decimal 0..100: no sign, spaces, suffix or empty input. */
static bool ParseScore(const char *text, unsigned *out)
{
    if (text==NULL || out==NULL || text[0]=='\0') return false;
    for (const char *cursor=text; *cursor!='\0'; ++cursor)
        if (*cursor<'0' || *cursor>'9') return false;
    errno=0;
    char *end=NULL;
    unsigned long value=strtoul(text,&end,10);
    if (errno==ERANGE || end==text || *end!='\0' || value>100UL) return false;
    *out=(unsigned)value; return true;
}
int main(void)
{
    unsigned score=88U;
    const char *const rejected[]={"","-1","+1"," 5","5x","101","99999999999999999999999999999999"};
    for (size_t index=0U; index<sizeof(rejected)/sizeof(rejected[0]); ++index)
        if (ParseScore(rejected[index],&score) || score!=88U) return EXIT_FAILURE;
    if (!ParseScore("0",&score) || score!=0U || !ParseScore("100",&score) || score!=100U)
        return EXIT_FAILURE;
    puts("accepted 0 and 100; signs, spaces, overflow and suffixes rejected");
    puts("PASS: numeric_parsing");
    return EXIT_SUCCESS;
}
