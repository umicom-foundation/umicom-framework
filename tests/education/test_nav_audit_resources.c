/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_nav_audit_resources.c
 *
 * PURPOSE:
 *   Inject allocation failures and a small retained-storage budget into the real navigation auditor, verifying incomplete results and released ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#endif

static size_t allocationAttempt, failureAttempt = SIZE_MAX, liveAllocations;

static void *TestAllocate(size_t bytes)
{
    if (allocationAttempt++ == failureAttempt) return NULL;
    void *result = malloc(bytes);
    if (result != NULL) ++liveAllocations;
    return result;
}

static void *TestReallocate(void *pointer, size_t bytes)
{
    if (allocationAttempt++ == failureAttempt) return NULL;
    int wasNull = pointer == NULL;
    void *result = realloc(pointer, bytes);
    if (result != NULL && wasNull) ++liveAllocations;
    return result;
}

static void TestFree(void *pointer)
{
    if (pointer != NULL) --liveAllocations;
    free(pointer);
}

/* This test compiles the real service with private allocator substitutions;
 * production builds have no mutable allocator hooks or global test switches. */
#define malloc TestAllocate
#define realloc TestReallocate
#define free TestFree
#include "../../src/education/nav_audit.c"
#undef malloc
#undef realloc
#undef free

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "[FAIL] navigation resources:%d: %s\n", __LINE__, #value); \
    return 1; } } while (0)

static int TestDirectory(const char *directory)
{
#if defined(_WIN32)
    return _mkdir(directory) == 0 || errno == EEXIST;
#else
    return mkdir(directory, 0700) == 0 || errno == EEXIST;
#endif
}

static int IncompleteCase(const char *root, size_t failure)
{
    CHECK(liveAllocations == 0U);
    allocationAttempt = 0U;
    failureAttempt = failure;
    FILE *diagnostics = tmpfile();
    CHECK(diagnostics != NULL);
    UmiEducationNavReport report;
    CHECK(UmiEducationAuditNavigation(root, &report, diagnostics) == 2);
    CHECK(liveAllocations == 0U);
    CHECK(report.pageCount < 300U && report.structuralErrors == 0U);
    rewind(diagnostics);
    char line[1024];
    int incomplete = 0;
    while (fgets(line, (int)sizeof(line), diagnostics) != NULL)
        if (strstr(line, "Navigation audit (incomplete):") != NULL) incomplete = 1;
    CHECK(incomplete);
    CHECK(fclose(diagnostics) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    CHECK(TestDirectory(argv[1]));
    char config[4096], content[4096], page[4096];
    int written = snprintf(config, sizeof(config), "%s/mkdocs.yml", argv[1]);
    CHECK(written >= 0 && (size_t)written < sizeof(config));
    written = snprintf(content, sizeof(content), "%s/content", argv[1]);
    CHECK(written >= 0 && (size_t)written < sizeof(content));
    CHECK(TestDirectory(content));
    written = snprintf(page, sizeof(page), "%s/index.md", content);
    CHECK(written >= 0 && (size_t)written < sizeof(page));
    FILE *file = fopen(page, "wb");
    CHECK(file != NULL);
    CHECK(fputs("# Home\n", file) >= 0);
    CHECK(fclose(file) == 0);
    file = fopen(config, "wb");
    CHECK(file != NULL);
    CHECK(fputs("nav:\n", file) >= 0);
    for (size_t i = 0U; i < 300U; ++i)
        CHECK(fprintf(file, "  - Page %zu: index.md\n", i) > 0);
    CHECK(fclose(file) == 0);
#if defined(UMI_NAV_TEST_SMALL_BUDGET)
    CHECK(IncompleteCase(argv[1], SIZE_MAX) == 0);
#else
    const size_t failures[] = {0U, 1U, 2U, 64U, 65U, 66U, 130U};
    for (size_t i = 0U; i < sizeof(failures) / sizeof(failures[0]); ++i)
        CHECK(IncompleteCase(argv[1], failures[i]) == 0);
    failureAttempt = SIZE_MAX;
    allocationAttempt = 0U;
    UmiEducationNavReport report;
    CHECK(UmiEducationAuditNavigation(argv[1], &report, NULL) == 1);
    CHECK(report.pageCount == 300U && report.duplicateCount == 299U);
    CHECK(report.missingCount == 0U && report.structuralErrors == 0U);
    CHECK(liveAllocations == 0U);
#endif
    puts("[OK] Navigation allocation/budget failure stays incomplete and releases owned memory.");
    return 0;
}
