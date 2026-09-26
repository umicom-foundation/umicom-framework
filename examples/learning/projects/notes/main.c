/* Umicom Framework teaching project: notes / main.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "notes.h"
#include <stdio.h>
int main(void)
{
    NoteBook book;
    NotesInit(&book);
    if (NotesSet(&book,"meeting","Bring a notebook")!=0) return 1;
    const char *text=NotesFind(&book,"meeting");
    if (text==NULL) return 1;
    printf("meeting: %s\n",text);
    return 0;
}
