/* Native Umicom educational publishing preflight | Sammy Hegab | MIT */
#include "umicom/education/course_release.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int ParseNumber(const char *value, uint32_t *out)
{
    char *end = NULL;
    if (value == NULL || *value == '\0' || *value == '-') return 0;
    unsigned long n = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || n > UINT32_MAX || n == 0UL) return 0;
    *out = (uint32_t)n;
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 5) {
        fputs("Usage: umicom-native-course-release REPO ai|maths FIRST LAST\n", stderr);
        return 2;
    }
    UmiEducationCourseKind kind;
    if (strcmp(argv[2], "ai") == 0) kind = UMI_EDUCATION_COURSE_NATIVE_AI;
    else if (strcmp(argv[2], "maths") == 0) kind = UMI_EDUCATION_COURSE_MATHEMATICS;
    else { fputs("Course must be ai or maths\n", stderr); return 2; }
    uint32_t first, last;
    if (!ParseNumber(argv[3], &first) || !ParseNumber(argv[4], &last)) {
        fputs("FIRST and LAST must be positive integers\n", stderr);
        return 2;
    }
    UmiEducationCourseReleaseReport report = {0};
    UmiStatus status = UmiEducationCourseReleaseAudit(argv[1], kind,
                                                       first, last, &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Native course gate cannot inspect the repository: %d\n", (int)status);
        return 1;
    }
    printf("Companions complete: %zu\n", report.companions.completePairs);
    printf("HTML pages: %zu; Markdown pages: %zu\n",
           report.htmlPagesRead, report.markdownPagesRead);
    printf("Local references checked: %zu; missing: %zu; unsafe: %zu\n",
           report.localLinksChecked, report.missingLocalLinks, report.unsafeLocalLinks);
    printf("Internal editorial markers: %zu; unreadable pages: %zu\n",
           report.internalEditorialMarkers, report.unreadableOrOversizedFiles);
    if (!UmiEducationCourseReleaseReady(&report)) {
        fputs("Course preflight: FAILED\n", stderr);
        return 1;
    }
    puts("Course preflight: PASSED (source and local links; not full HTML rendering)");
    return 0;
}
