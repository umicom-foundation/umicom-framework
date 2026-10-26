/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_course_home_link.c
 * PURPOSE: Prove source preservation, idempotence, escape safety and refusal
 *          to publish an incomplete homepage navigation change.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_home_link.h"
#include <stdio.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) {fprintf(stderr,"course link failed line %d: %s\n",__LINE__,#test);return 1;}}while (0)
int main(void)
{
    const char before[] = "<!doctype html><html><head><title>Complete source</title></head><body>"
        "<main id=\"main\"><h1>Original lesson list</h1><a href=\"lessons/01.html\">one</a>"
        "</main><footer>preserved</footer></body></html>";
    char updated[2048] = {0}, repeated[2048] = {0};
    char small[8] = "RETAIN!";
    size_t n = 25U, m = 0U;
    CHECK(UmiEducationCourseHomeLinkCompose(before,strlen(before),
          "advanced-lessons.html", "More AI & <maths>",updated,sizeof(updated),&n)==UMI_STATUS_OK);
    CHECK(n==strlen(updated));
    CHECK(strstr(updated,"More AI &amp; &lt;maths&gt;") != NULL);
    CHECK(strstr(updated,"Original lesson list")!=NULL);
    CHECK(strstr(updated,"<footer>preserved</footer>")!=NULL);
    CHECK(strstr(updated,"href=\"advanced-lessons.html\"")!=NULL);
    CHECK(strstr(updated,"<title>Complete source</title>")!=NULL);
    CHECK(UmiEducationCourseHomeLinkCompose(updated,n,
        "advanced-lessons.html","More AI & <maths>",repeated,sizeof(repeated),&m)==UMI_STATUS_OK);
    CHECK(m==n && memcmp(updated,repeated,n+1U)==0);
    size_t sentinel = 99U;
    CHECK(UmiEducationCourseHomeLinkCompose(before,strlen(before),
          "advanced-lessons.html","label",small,sizeof(small),&sentinel)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(small,"RETAIN!",8U)==0 && sentinel==99U);
    CHECK(UmiEducationCourseHomeLinkCompose(before,strlen(before),
          "../outside.html","label",updated,sizeof(updated),&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationCourseHomeLinkCompose(before,strlen(before),
          "https://bad.example/file.html","label",updated,sizeof(updated),&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationCourseHomeLinkCompose(before,strlen(before),
          "safe.html","bad\nlabel",updated,sizeof(updated),&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationCourseHomeLinkCompose("<body>no main</body>",20U,
          "safe.html","label",updated,sizeof(updated),&sentinel)==UMI_STATUS_PARSE_ERROR);
    CHECK(sentinel == 99U);
    puts("Source-preserving HTML course link checks passed");
    return 0;
}
