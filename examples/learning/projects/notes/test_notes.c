/* Umicom Framework teaching project: notes / test_notes.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "notes.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do {if (!(condition)) {fprintf(stderr,"Line %d: %s\n",__LINE__,#condition);return 1;}} while (0)
int main(void)
{
    NoteBook book;NotesInit(&book);
    CHECK(NotesFind(&book,"missing")==NULL);
    CHECK(NotesSet(&book,"meeting","Original")==0);
    CHECK(book.count==1U);
    CHECK(NotesSet(&book,"meeting","Changed")==0 && book.count==1U);
    CHECK(strcmp(NotesFind(&book,"meeting"),"Changed")==0);
    char tooLong[NOTE_TEXT_CAPACITY+1U];memset(tooLong,'a',sizeof tooLong-1U);tooLong[sizeof tooLong-1U]='\0';
    CHECK(NotesSet(&book,"meeting",tooLong)==2);
    CHECK(strcmp(NotesFind(&book,"meeting"),"Changed")==0);
    CHECK(NotesSet(&book,"","Empty key")==1 && book.count==1U);
    CHECK(NotesSet(&book,"copy",NotesFind(&book,"meeting"))==0);
    CHECK(strcmp(NotesFind(&book,"copy"),"Changed")==0);
    for (size_t i=book.count;i<NOTE_CAPACITY;++i) {
        char key[16];int n=snprintf(key,sizeof key,"note-%zu",i);
        CHECK(n>0 && (size_t)n<sizeof key);CHECK(NotesSet(&book,key,"Text")==0);
    }
    CHECK(NotesSet(&book,"overflow","Text")==2 && book.count==NOTE_CAPACITY);
    CHECK(NotesSet(NULL,"key","text")==1);
    puts("Notes contracts passed: empty, insert, replace, ownership and capacity.");
    return 0;
}
