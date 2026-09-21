/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/library_link/test_close_progress.c
 * PURPOSE: Exercise copied close progress through the actual document library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close_session.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiDocumentCloseProgress progress = {0};
    progress.phase = UMI_DOCUMENT_CLOSE_READY;
    progress.total = 3U; progress.closed = 1U; progress.already_closed = 1U; progress.remaining = 1U;
    memcpy(progress.current.display_name, "notes.c", sizeof("notes.c"));
    UmiDocumentCloseProgress before;
    memcpy(&before, &progress, sizeof(before));
    char text[768] = "Preserve this message";
    const char *name = argv[1];
    if (!strcmp(name,"null")) {
        CHECK(UmiDocumentCloseProgressValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseProgressRequireComplete(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name,"dirty")) progress.current.dirty = 2;
    else if (!strcmp(name,"path")) progress.current.has_path = -1;
    else if (!strcmp(name,"readonly")) progress.current.read_only = 7;
    else if (!strcmp(name,"counts")) progress.closed = SIZE_MAX;
    else if (!strcmp(name,"name")) memset(progress.current.display_name,'x',sizeof(progress.current.display_name));
    else if (!strcmp(name,"phase")) progress.phase=(UmiDocumentClosePhase)99;
    else if (!strcmp(name,"status")) progress.last_status=(UmiStatus)99;
    else if (!strcmp(name,"empty-ready")) { progress.total=0U;progress.closed=0U;progress.already_closed=0U;progress.remaining=0U; }
    else if (!strcmp(name,"incomplete")) {
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentCloseProgressValidate(&progress)==UMI_STATUS_OK);
        CHECK(memcmp(&progress,&before,sizeof(progress))==0);return 0;
    } else if (!strcmp(name,"cancelled")) {
        progress.phase=UMI_DOCUMENT_CLOSE_CANCELLED;progress.last_status=UMI_STATUS_CANCELLED;
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_CANCELLED);return 0;
    } else if (!strcmp(name,"failed")) {
        progress.phase=UMI_DOCUMENT_CLOSE_FAILED;progress.last_status=UMI_STATUS_IO_ERROR;
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_IO_ERROR);return 0;
    } else if (!strcmp(name,"host-failure")) {
        progress.phase=UMI_DOCUMENT_CLOSE_FAILED;progress.last_status=UMI_STATUS_UNAVAILABLE;
        progress.closed=2U;progress.remaining=0U;
        CHECK(UmiDocumentCloseProgressValidate(&progress)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_UNAVAILABLE);return 0;
    } else if (!strcmp(name,"complete")) {
        progress.phase=UMI_DOCUMENT_CLOSE_COMPLETE;progress.closed=2U;progress.remaining=0U;
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseProgressFormat(&progress,text,sizeof(text))==UMI_STATUS_OK);
        CHECK(strstr(text,"2 closed, 1 already closed, 0 remaining")!=NULL);return 0;
    } else if (!strcmp(name,"capacity")) {
        CHECK(UmiDocumentCloseProgressFormat(&progress,text,1U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text,"Preserve this message")==0);
        CHECK(memcmp(&progress,&before,sizeof(progress))==0);return 0;
    } else { fprintf(stderr,"Unknown case: %s\n",name);return 1; }
    if (strcmp(name,"null")) {
        memcpy(&before,&progress,sizeof(before));
        CHECK(UmiDocumentCloseProgressValidate(&progress)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseProgressRequireComplete(&progress)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseProgressFormat(&progress,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&progress,&before,sizeof(progress))==0);
    }
    CHECK(strcmp(text,"Preserve this message")==0);
    return 0;
}
