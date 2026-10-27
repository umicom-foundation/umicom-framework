/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_course_release.c
 *
 * PURPOSE:
 *   Native C23 course gate acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_release.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <direct.h>
# define UMI_MKDIR(path) _mkdir(path)
#else
# define UMI_MKDIR(path) mkdir(path, 0700)
#endif

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d rejected: %s\n", __FILE__, __LINE__, #expr); \
    return 1; } } while (0)

static int Join(char *out, size_t cap, const char *a, const char *b)
{
    int n = snprintf(out, cap, "%s/%s", a, b);
    return n > 0 && (size_t)n < cap;
}

static int Write(const char *name, const char *content)
{
    FILE *f = fopen(name, "wb");
    if (f == NULL) return 0;
    size_t n = strlen(content);
    int ok = fwrite(content, 1U, n, f) == n;
    if (fclose(f) != 0) return 0;
    return ok;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *root = argv[1];
    const char *parts[] = {
        "", "content", "content/lessons", "content/lessons/10-native-ai",
        "content/courses", "content/courses/llm-from-scratch",
        "content/courses/llm-from-scratch/lessons"
    };
    for (size_t i = 0U; i < sizeof(parts)/sizeof(parts[0]); ++i) {
        char dir[4096];
        CHECK(Join(dir, sizeof(dir), root, parts[i]));
        (void)UMI_MKDIR(dir);
    }
    char md[4096], html[4096];
    CHECK(Join(md, sizeof(md), root,
        "content/lessons/10-native-ai/281-native-measurement.md"));
    CHECK(Join(html, sizeof(html), root,
        "content/courses/llm-from-scratch/lessons/281-native-measurement.html"));
    CHECK(Write(md, "# Lesson 281\nNative benchmark with source attribution.\n"));
    CHECK(Write(html, "<html><a href=\"../../../lessons/10-native-ai/281-native-measurement.md\">Source</a></html>\n"));

    UmiEducationCourseReleaseReport report = {0};
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        281U, 281U, &report) == UMI_STATUS_OK);
    CHECK(UmiEducationCourseReleaseReady(&report));
    CHECK(report.htmlPagesRead == 1U && report.markdownPagesRead == 1U);
    CHECK(report.localLinksChecked == 1U && report.missingLocalLinks == 0U);

    CHECK(Write(html, "<a href=\"missing-page.html\">Missing</a>"));
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        281U, 281U, &report) == UMI_STATUS_OK);
    CHECK(!UmiEducationCourseReleaseReady(&report));
    CHECK(report.missingLocalLinks == 1U);

    CHECK(Write(html, "<a href=\"../../../../../../escape.html\">Unsafe</a>"));
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        281U, 281U, &report) == UMI_STATUS_OK);
    CHECK(!UmiEducationCourseReleaseReady(&report));
    CHECK(report.unsafeLocalLinks == 1U);

    CHECK(Write(html, "<p>Engineering follow-up (separate Framework batch)</p>"));
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        281U, 281U, &report) == UMI_STATUS_OK);
    CHECK(!UmiEducationCourseReleaseReady(&report));
    CHECK(report.internalEditorialMarkers == 1U);

    CHECK(Write(html, "<html>Reviewed learner content</html>"));
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        281U, 281U, &report) == UMI_STATUS_OK);
    CHECK(UmiEducationCourseReleaseReady(&report));
    UmiEducationCourseReleaseReport unchanged = report;
    CHECK(UmiEducationCourseReleaseAudit(root, UMI_EDUCATION_COURSE_NATIVE_AI,
        0U, 281U, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &unchanged, sizeof(report)) == 0);
    puts("Native publication preflight: companions, paths and editorial gate pass");
    return 0;
}
