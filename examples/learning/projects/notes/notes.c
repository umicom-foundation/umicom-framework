/* Umicom Framework teaching project: notes / notes.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "notes.h"
#include <string.h>
void NotesInit(NoteBook *book)
{
    if (book!=NULL) memset(book,0,sizeof *book);
}
const char *NotesFind(const NoteBook *book,const char *key)
{
    if (book==NULL || key==NULL || book->count>NOTE_CAPACITY) return NULL;
    for (size_t i=0U;i<book->count;++i)
        if (strcmp(book->entries[i].key,key)==0) return book->entries[i].text;
    return NULL;
}
int NotesSet(NoteBook *book,const char *key,const char *text)
{
    if (book==NULL || key==NULL || text==NULL || key[0]=='\0' || book->count>NOTE_CAPACITY) return 1;
    size_t keyLength=strlen(key),textLength=strlen(text),index=book->count;
    if (keyLength>=NOTE_KEY_CAPACITY || textLength>=NOTE_TEXT_CAPACITY) return 2;
    for (size_t i=0U;i<book->count;++i)
        if (strcmp(book->entries[i].key,key)==0) {index=i;break;}
    if (index==NOTE_CAPACITY) return 2;
    /* A temporary copy also permits input borrowed from the current notebook. */
    Note next={0};
    memcpy(next.key,key,keyLength+1U);memcpy(next.text,text,textLength+1U);
    book->entries[index]=next;
    if (index==book->count) ++book->count;
    return 0;
}
