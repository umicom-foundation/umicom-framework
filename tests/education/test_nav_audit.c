/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_nav_audit.c
 *
 * PURPOSE:
 *   Test native navigation preflight with valid, missing, duplicate, unsafe and malformed targets, including sites larger than the historical page limit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Scale fixtures reuse one real page. Repeated references exercise complete
 * counting and duplicate diagnostics without creating thousands of files. A
 * separate unique-target case checks ordering without duplicate shortcuts. */
static int ScaleCase(const char *root, const char *name)
{
    size_t pages = 2058U;
    int unique = 0, late = 0;
    if (strcmp(name, "boundary") == 0) pages = 2048U;
    else if (strcmp(name, "beyond") == 0) pages = 2049U;
    else if (strcmp(name, "large") == 0) pages = 8193U;
    else if (strcmp(name, "unique") == 0) { pages = 4097U; unique = 1; }
    else if (strcmp(name, "late-failures") == 0) late = 1;
    else { fprintf(stderr, "Unknown scale case: %s\n", name); return 1; }
    CHECK(MkDir(root));
    char content[4096], config[4096], home[4096];
    CHECK(Join(content, sizeof(content), root, "content"));
    CHECK(MkDir(content));
    CHECK(Join(config, sizeof(config), root, "mkdocs.yml"));
    CHECK(Join(home, sizeof(home), content, "index.md"));
    CHECK(Write(home, "# Home\n"));
    FILE *stream = fopen(config, "wb");
    CHECK(stream != NULL);
    CHECK(fputs("nav:\n", stream) >= 0);
    for (size_t i = 0U; i < pages; ++i) {
        if (unique)
            CHECK(fprintf(stream, "  - Page %zu: absent-%zu.md\n", i, pages-i) > 0);
        else
            CHECK(fprintf(stream, "  - Page %zu: index.md\n", i) > 0);
    }
    if (late)
        CHECK(fputs("  - Missing: not-here.md\n  - Repeated: index.md\n"
                    "  - Unsafe: ../outside.md\n  - Quoted: \"index.md\n", stream) >= 0);
    CHECK(fclose(stream) == 0);
    FILE *diagnostics = tmpfile();
    CHECK(diagnostics != NULL);
    UmiEducationNavReport report;
    CHECK(UmiEducationAuditNavigation(root, &report, diagnostics) == 1);
    CHECK(report.pageCount == pages + (late ? 2U : 0U));
    CHECK(report.duplicateCount == (unique ? 0U : pages - 1U + (late ? 1U : 0U)));
    CHECK(report.missingCount == (unique ? pages : (late ? 1U : 0U)));
    CHECK(report.unsafeCount == (late ? 1U : 0U));
    CHECK(report.structuralErrors == (late ? 1U : 0U));
    if (late) {
        rewind(diagnostics);
        char line[1024];
        int duplicateAtOriginalLine = 0;
        while (fgets(line, (int)sizeof(line), diagnostics) != NULL)
            if (strstr(line, "mkdocs.yml:2061: duplicate target: index.md") != NULL)
                duplicateAtOriginalLine = 1;
        CHECK(duplicateAtOriginalLine);
    }
    CHECK(fclose(diagnostics) == 0);
    printf("[OK] Complete navigation scale case: %s (%zu references)\n", name, report.pageCount);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 3) return ScaleCase(argv[1], argv[2]);
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
