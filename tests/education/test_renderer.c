/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_renderer.c
 * PURPOSE: Test explicit interpreter selection, failing children and the
 *          required index.html postcondition without Python or MkDocs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <direct.h>
# include <windows.h>
# define Mkdir(path) _mkdir(path)
#else
# define Mkdir(path) mkdir(path, 0755)
#endif

static void CopyExecutable(const char *source, const char *destination)
{
#if defined(_WIN32)
    assert(CopyFileA(source, destination, FALSE) != 0);
#else
    FILE *from = fopen(source, "rb");
    FILE *to = fopen(destination, "wb");
    assert(from != NULL && to != NULL);
    unsigned char bytes[16384];
    while (!feof(from)) {
        size_t count = fread(bytes, 1U, sizeof(bytes), from);
        if (count == 0U) break;
        assert(fwrite(bytes, 1U, count, to) == count);
    }
    assert(ferror(from) == 0);
    assert(fclose(from) == 0);
    assert(fclose(to) == 0);
    assert(chmod(destination, 0755) == 0);
#endif
}
static void SetEnvironment(const char *name, const char *value)
{
#if defined(_WIN32)
    assert(_putenv_s(name, value) == 0);
#else
    assert(setenv(name, value, 1) == 0);
#endif
}
static void ClearEnvironment(const char *name)
{
#if defined(_WIN32)
    assert(_putenv_s(name, "") == 0);
#else
    assert(unsetenv(name) == 0);
#endif
}
int main(int argc, char **argv)
{
    assert(argc == 3);
    char configuration[UMICOM_EDUCATION_PATH_CAPACITY];
    char page[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(Mkdir(argv[1]) == 0 || errno == EEXIST);
    assert(snprintf(configuration, sizeof(configuration), "%s/mkdocs.yml", argv[1]) > 0);
    FILE *file = fopen(configuration, "wb");
    assert(file != NULL);
    assert(fputs("site_name: Renderer test\n", file) >= 0);
    assert(fclose(file) == 0);
    ClearEnvironment("UMICOM_EDUCATION_TEST_BUILD_FAIL");
    ClearEnvironment("UMICOM_EDUCATION_TEST_NO_RENDERER");
    assert(UmicomEducationCheckRenderer(argv[1], argv[2]) == 0);
    assert(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 0);
    /* The checkout-owned interpreter takes precedence over a mismatched PATH.
     * This reproduces the Windows MSYS2 Python conflict deterministically. */
    char environment[UMICOM_EDUCATION_PATH_CAPACITY];
    char interpreter[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(snprintf(environment, sizeof(environment), "%s/.venv", argv[1]) > 0);
    assert(Mkdir(environment) == 0 || errno == EEXIST);
#if defined(_WIN32)
    assert(snprintf(interpreter, sizeof(interpreter), "%s/.venv/Scripts", argv[1]) > 0);
    assert(Mkdir(interpreter) == 0 || errno == EEXIST);
    assert(snprintf(interpreter, sizeof(interpreter), "%s/.venv/Scripts/python.exe", argv[1]) > 0);
#else
    assert(snprintf(interpreter, sizeof(interpreter), "%s/.venv/bin", argv[1]) > 0);
    assert(Mkdir(interpreter) == 0 || errno == EEXIST);
    assert(snprintf(interpreter, sizeof(interpreter), "%s/.venv/bin/python", argv[1]) > 0);
#endif
    CopyExecutable(argv[2], interpreter);
    SetEnvironment("UMICOM_EDUCATION_PYTHON", "intentionally-invalid-python-on-path");
    assert(UmicomEducationBuildSite(argv[1]) == 0);
    ClearEnvironment("UMICOM_EDUCATION_PYTHON");
    assert(snprintf(page, sizeof(page), "%s/site/index.html", argv[1]) > 0);
    file = fopen(page, "rb");
    assert(file != NULL);
    assert(fclose(file) == 0);
    SetEnvironment("UMICOM_EDUCATION_TEST_BUILD_FAIL", "1");
    assert(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 1);
    ClearEnvironment("UMICOM_EDUCATION_TEST_BUILD_FAIL");
    SetEnvironment("UMICOM_EDUCATION_TEST_NO_RENDERER", "1");
    assert(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 1);
    ClearEnvironment("UMICOM_EDUCATION_TEST_NO_RENDERER");
    assert(UmicomEducationCheckRenderer(argv[1], "not-an-executable-education-fixture-404") == 1);
    puts("Interpreter discovery, child failure and website postcondition tests passed.");
    return 0;
}
