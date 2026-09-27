/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/notes_project/test_notes.c
 *
 * PURPOSE:
 *   Check the Notes contract without disabling assertions in a Release build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "notes.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    static const struct {const char *text;size_t count;} examples[] = {
        {"",0U},{"Prepare the workshop note",4U},{"  one\ttwo\r\nthree  ",3U},{"\xc2\xa3" "50 allocation",2U}
    };
    for (size_t i=0U;i<sizeof examples/sizeof examples[0];++i) {
        size_t count=999U;
        if (NotesWordCount(examples[i].text,strlen(examples[i].text),&count)!=0 || count!=examples[i].count) {
            fprintf(stderr,"Notes case %zu: expected %zu words, got %zu\n",i,examples[i].count,count);return 1;
        }
    }
    size_t sentinel=17U;
    if(NotesWordCount(NULL,0U,&sentinel)!=1 || sentinel!=17U || NotesWordCount("a\0b",3U,&sentinel)!=1 || sentinel!=17U)return 1;
    puts("Notes word-count checks passed.");return 0;
}
