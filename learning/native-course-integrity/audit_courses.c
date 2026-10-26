/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-course-integrity/audit_courses.c
 * PURPOSE: Check actual HTML/Markdown lesson companions without modifying
 *          public curriculum source, assets or repository navigation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_companions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int Audit(const char *repo, UmiEducationCourseKind kind,
                 uint32_t first, uint32_t last)
{
    UmiEducationCourseCompanionReport r={0};
    UmiStatus status=UmiEducationCourseCompanionsAudit(repo,kind,first,last,&r);
    if(status!=UMI_STATUS_OK){
        fprintf(stderr,"Cannot read course directories; Framework status %d\n",(int)status);
        return 2;
    }
    printf("%s %u–%u: pairs=%zu; missing Markdown=%zu; missing HTML=%zu; "
           "both missing=%zu; slug mismatches=%zu; duplicates=%zu\n",
           kind==UMI_EDUCATION_COURSE_NATIVE_AI?"LLM":"Mathematics",
           first,last,r.completePairs,r.markdownMissing,r.htmlMissing,r.bothMissing,
           r.differentSlugs,r.duplicateNumberedPages);
    return r.markdownMissing||r.htmlMissing||r.bothMissing||
           r.differentSlugs||r.duplicateNumberedPages?1:0;
}
int main(int argc,char **argv)
{
    if(argc!=3 || strcmp(argv[1],"--repo")!=0){
        fputs("Usage: umicom-native-course-integrity --repo REPO\n",stderr);
        return 2;
    }
    int llm=Audit(argv[2],UMI_EDUCATION_COURSE_NATIVE_AI,95U,168U);
    int maths=Audit(argv[2],UMI_EDUCATION_COURSE_MATHEMATICS,21U,78U);
    return llm==2||maths==2?2:llm||maths?1:0;
}
