/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/library.c
 * PURPOSE: Keep the offline guide catalogue in one native, read-only authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education_workspace/study.h"
#include "private.h"
#include "study_internal.h"
#include <string.h>
static const UmiEducationLibraryEntry LIBRARY[]={
#include "library.inc"
};
_Static_assert(sizeof LIBRARY/sizeof LIBRARY[0]<=UMI_EDUCATION_LIBRARY_LIMIT,"Library capacity");
size_t UmiEducationLibraryCount(void) {return sizeof LIBRARY/sizeof LIBRARY[0];}
const UmiEducationLibraryEntry *UmiEducationLibraryAt(size_t index)
{return index<UmiEducationLibraryCount()?&LIBRARY[index]:NULL;}
static bool GuideName(const char *name)
{
    size_t n=strlen(name);
    if (n<6U||strcmp(name+n-5U,".html")!=0) return false;
    for (size_t i=0U;i<n;++i) {
        char c=name[i];
        if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.')) return false;
    }
    return strstr(name,"..")==NULL;
}
UmiStatus UmiEducationLibraryValidate(void)
{
    for (size_t i=0U;i<UmiEducationLibraryCount();++i) {
        const UmiEducationLibraryEntry *e=&LIBRARY[i];
        if (!EwIdValid(e->id)||!EwTextValid(e->title,256U,false)||
            !EwTextValid(e->category,128U,false)||!EwTextValid(e->boundary,512U,false)||
            !EwTextValid(e->guideFile,128U,false)||!GuideName(e->guideFile)||
            !EwTextValid(e->examplePath,256U,false)||strncmp(e->examplePath,"examples/",9U)!=0||
            strstr(e->examplePath,"..")!=NULL||strpbrk(e->examplePath,"\\:\r\n\t")!=NULL)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j=0U;j<i;++j)
            if (strcmp(e->id,LIBRARY[j].id)==0||strcmp(e->guideFile,LIBRARY[j].guideFile)==0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiEducationLibrarySearch(const char *text,UmiEducationLibraryMatches *out)
{
    if (out==NULL||!EwTextValid(text,UMI_EDUCATION_SEARCH_CAPACITY,true))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiEducationLibraryValidate();
    if (status!=UMI_STATUS_OK) return status;
    UmiEducationLibraryMatches result={0};
    for (size_t i=0U;i<UmiEducationLibraryCount();++i) {
        const UmiEducationLibraryEntry *e=&LIBRARY[i];
        if (EwStudyContains(e->id,text)||EwStudyContains(e->title,text)||
            EwStudyContains(e->category,text)||EwStudyContains(e->boundary,text))
            result.indices[result.count++]=i;
    }
    *out=result;return UMI_STATUS_OK;
}
