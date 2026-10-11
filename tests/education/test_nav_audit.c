/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_nav_audit.c
 * PURPOSE: Test a native, read-only navigation preflight with valid, missing,
 *          duplicate, unsafe and malformed target fixtures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/nav_audit.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#endif

static int MkDir(const char *path)
{
#if defined(_WIN32)
    return _mkdir(path) == 0 || errno == EEXIST;
#else
    return mkdir(path, 0700) == 0 || errno == EEXIST;
#endif
}

static int Join(char *out, size_t capacity, const char *root, const char *part)
{
    int written = snprintf(out, capacity, "%s/%s", root, part);
    return written >= 0 && (size_t)written < capacity;
}

static int Write(const char *path, const char *text)
{
    FILE *stream = fopen(path, "wb");
    if (stream == NULL) return 0;
    size_t length = strlen(text);
    int ok = fwrite(text, 1U, length, stream) == length;
    if (fclose(stream) != 0) ok = 0;
    return ok;
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "[FAIL] nav audit test:%d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    CHECK(MkDir(argv[1]));
    char content[4096], lessons[4096], config[4096], home[4096], chapter[4096];
    CHECK(Join(content, sizeof(content), argv[1], "content"));
    CHECK(Join(lessons, sizeof(lessons), content, "lessons"));
    CHECK(Join(config, sizeof(config), argv[1], "mkdocs.yml"));
    CHECK(Join(home, sizeof(home), content, "index.md"));
    CHECK(Join(chapter, sizeof(chapter), lessons, "01-bounded-c.md"));
    CHECK(MkDir(content) && MkDir(lessons));
    CHECK(Write(home, "# Home\n"));
    CHECK(Write(chapter, "# Bounded C23\n"));

    UmiEducationNavReport report;
    const char *valid = "site_name: Test\n"
        "docs_dir: content\nnav:\n"
        "  - Home: index.md\n"
        "  - Lessons:\n"
        "      - \"01. Memory-safe C23\": lessons/01-bounded-c.md\n";
    CHECK(Write(config, valid));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, stderr) == 0);
    CHECK(report.pageCount == 2U && report.missingCount == 0U &&
          report.unsafeCount == 0U && report.duplicateCount == 0U);

    CHECK(Write(config, "nav:\n  - Home: index.md\n  - Missing: lessons/not-here.md\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.missingCount == 1U && report.pageCount == 2U);

    CHECK(Write(config, "nav:\n  - Home: index.md\n  - Second: index.md\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.duplicateCount == 1U);

    CHECK(Write(config, "nav:\n  - Escape: ../README.md\n  - Good: index.md\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.unsafeCount == 1U && report.pageCount == 1U);

    CHECK(Write(config, "nav:\n  - Escape: C:\\secrets\\key.md\n  - Good: index.md\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.unsafeCount == 1U);

    CHECK(Write(config, "nav:\n  - Not closed: \"index.md\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.structuralErrors > 0U);

    CHECK(Write(config, "site_name: Empty\n"));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.structuralErrors == 1U);

    CHECK(Write(config, valid));
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 0);
    CHECK(UmiEducationAuditNavigation(NULL, &report, NULL) == 2);
    puts("[OK] Native C23 navigation audit: present, missing, duplicate, traversal, "
         "drive paths, quoting and empty navigation.");
    return 0;
}
