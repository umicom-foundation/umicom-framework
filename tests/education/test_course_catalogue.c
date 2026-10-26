/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_course_catalogue.c
 * PURPOSE: Validate safe HTML escaping, ordered navigation and failure-atomic
 *          in-memory catalogue rendering using the real Framework source.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_catalogue.h"

#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "catalogue assertion on line %d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

int main(void)
{
    const UmiEducationCourseEntry entries[] = {
        {95U, "Review <source> & model identity", "lessons/95-admission.html"},
        {96U, "Write a portable \"checkpoint\"", "lessons/96-checkpoint.html"},
        {97U, "Keep a frozen model's identity", "lessons/97-frozen.html"}
    };
    char rendered[4096] = {0};
    char repeated[4096] = {0};
    char protected[8] = "PRESERV";
    size_t byteCount = 23U;
    size_t secondCount = 0U;
    CHECK(UmiEducationCourseCatalogueRender("LLM & AI", entries, 3U,
          "index.html", rendered, sizeof(rendered), &byteCount) == UMI_STATUS_OK);
    CHECK(byteCount == strlen(rendered));
    CHECK(strstr(rendered, "<!doctype html>") != NULL);
    CHECK(strstr(rendered, "LLM &amp; AI") != NULL);
    CHECK(strstr(rendered, "Review &lt;source&gt; &amp; model") != NULL);
    CHECK(strstr(rendered, "portable &quot;checkpoint&quot;") != NULL);
    CHECK(strstr(rendered, "aria-label=\"Course navigation\"") != NULL);
    CHECK(strstr(rendered, "href=\"lessons/95-admission.html\"") != NULL);
    CHECK(strstr(rendered, "<li value=\"97\">") != NULL);
    CHECK(strstr(rendered, "<script>") == NULL);
    CHECK(UmiEducationCourseCatalogueRender("LLM & AI", entries, 3U,
          "index.html", repeated, sizeof(repeated), &secondCount) == UMI_STATUS_OK);
    CHECK(byteCount == secondCount);
    CHECK(memcmp(rendered, repeated, byteCount + 1U) == 0);

    CHECK(UmiEducationCourseCatalogueRender("LLM & AI", entries, 3U,
          "index.html", protected, sizeof(protected), &secondCount) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(protected, "PRESERV", sizeof(protected)) == 0);
    CHECK(secondCount == byteCount);

    UmiEducationCourseEntry bad[3];
    memcpy(bad, entries, sizeof(bad));
    bad[1].lessonNumber = 95U;
    CHECK(UmiEducationCourseCatalogueRender("LLM", bad, 3U,
          "index.html", protected, sizeof(protected), &secondCount) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(protected, "PRESERV", sizeof(protected)) == 0);
    bad[1] = entries[1];
    bad[2].href = "../outside.html";
    CHECK(UmiEducationCourseCatalogueRender("LLM", bad, 3U,
          "index.html", rendered, sizeof(rendered), &byteCount) ==
          UMI_STATUS_INVALID_ARGUMENT);
    bad[2].href = "javascript:alert.html";
    CHECK(UmiEducationCourseCatalogueRender("LLM", bad, 3U,
          "index.html", rendered, sizeof(rendered), &byteCount) ==
          UMI_STATUS_INVALID_ARGUMENT);
    bad[2].href = "lessons/97-frozen.md";
    CHECK(UmiEducationCourseCatalogueRender("LLM", bad, 3U,
          "index.html", rendered, sizeof(rendered), &byteCount) ==
          UMI_STATUS_INVALID_ARGUMENT);
    bad[2].href = entries[2].href;
    bad[2].title = "Invalid\nheading";
    CHECK(UmiEducationCourseCatalogueRender("LLM", bad, 3U,
          "index.html", rendered, sizeof(rendered), &byteCount) ==
          UMI_STATUS_INVALID_ARGUMENT);
    puts("Native HTML catalogue: escaped, deterministic, safe and failure-atomic");
    return 0;
}
