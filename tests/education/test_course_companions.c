/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_course_companions.c
 * PURPOSE: Read-only course reconciliation, missing companion detection,
 *          slug divergence and unchanged caller-owned outputs on rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_companions.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#define Mkdir(path) _mkdir(path)
#define Rmdir(path) _rmdir(path)
#else
#include <unistd.h>
#define Mkdir(path) mkdir(path,0755)
#define Rmdir(path) rmdir(path)
#endif
#define CHECK(ok) do {if(!(ok)){fprintf(stderr,"companion audit failed line %d: %s\n",__LINE__,#ok);return 1;}}while(0)
static int CreateFile(const char *path)
{
    FILE *out = fopen(path,"wb");
    if(!out) return 0;
    const char content[]="Reviewed educational fixture.\n";
    int result = fwrite(content,1U,sizeof(content)-1U,out)==sizeof(content)-1U;
    return fclose(out)==0 && result;
}
int main(void)
{
    const char *root="umicom-education-companion-fixture";
    char one[256],two[256],three[256],four[256],five[256],six[256];
    UmiEducationCourseCompanionReport report={0};
    CHECK(Mkdir(root)==0);
    snprintf(one,sizeof(one),"%s/content",root); CHECK(Mkdir(one)==0);
    snprintf(two,sizeof(two),"%s/content/lessons",root); CHECK(Mkdir(two)==0);
    snprintf(three,sizeof(three),"%s/content/lessons/10-native-ai",root); CHECK(Mkdir(three)==0);
    snprintf(four,sizeof(four),"%s/content/courses",root); CHECK(Mkdir(four)==0);
    snprintf(five,sizeof(five),"%s/content/courses/llm-from-scratch",root); CHECK(Mkdir(five)==0);
    snprintf(six,sizeof(six),"%s/content/courses/llm-from-scratch/lessons",root); CHECK(Mkdir(six)==0);
    char md[300],html[300],bad[300];
    snprintf(md,sizeof(md),"%s/161-example.md",three); CHECK(CreateFile(md));
    snprintf(html,sizeof(html),"%s/161-example.html",six); CHECK(CreateFile(html));
    snprintf(bad,sizeof(bad),"%s/162-different.html",six); CHECK(CreateFile(bad));
    UmiStatus status=UmiEducationCourseCompanionsAudit(root,UMI_EDUCATION_COURSE_NATIVE_AI,161U,163U,&report);
    CHECK(status==UMI_STATUS_OK && report.completePairs==1U &&
          report.markdownMissing==1U && report.bothMissing==1U && report.htmlMissing==0U);
    snprintf(md,sizeof(md),"%s/162-original.md",three); CHECK(CreateFile(md));
    status=UmiEducationCourseCompanionsAudit(root,UMI_EDUCATION_COURSE_NATIVE_AI,161U,163U,&report);
    CHECK(status==UMI_STATUS_OK && report.differentSlugs==1U && report.bothMissing==1U);
    remove(bad);
    snprintf(bad,sizeof(bad),"%s/162-original.html",six); CHECK(CreateFile(bad));
    status=UmiEducationCourseCompanionsAudit(root,UMI_EDUCATION_COURSE_NATIVE_AI,161U,163U,&report);
    CHECK(status==UMI_STATUS_OK && report.completePairs==2U && report.bothMissing==1U);
    UmiEducationCourseCompanionReport protectedValue=report;
    CHECK(UmiEducationCourseCompanionsAudit(root,UMI_EDUCATION_COURSE_NATIVE_AI,
          165U,161U,&report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&protectedValue,&report,sizeof(report))==0);
    CHECK(UmiEducationCourseCompanionsAudit("missing-directory",UMI_EDUCATION_COURSE_NATIVE_AI,
          161U,162U,&report)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&protectedValue,&report,sizeof(report))==0);
    remove(bad);
    snprintf(md,sizeof(md),"%s/162-original.md",three);remove(md);
    snprintf(md,sizeof(md),"%s/161-example.md",three);remove(md);
    remove(html);
    CHECK(Rmdir(six)==0); CHECK(Rmdir(five)==0); CHECK(Rmdir(four)==0);
    CHECK(Rmdir(three)==0); CHECK(Rmdir(two)==0); CHECK(Rmdir(one)==0);
    CHECK(Rmdir(root)==0);
    puts("Markdown/HTML companion audit passed");
    return 0;
}
