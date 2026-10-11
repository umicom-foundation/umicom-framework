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

/* The previous assert-based test is retained for review. Its first failed
 * renderer assertion aborted on Windows with 0xc0000409 and concealed the
 * precise process-argument failure. The active test below prints a location
 * and returns a normal failure status, making CTest diagnostics actionable. */
#if 0
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

#endif

#define EDUCATION_CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

static int CopyExecutableChecked(const char *source, const char *destination)
{
#if defined(_WIN32)
    if (CopyFileA(source, destination, FALSE) == 0) {
        fprintf(stderr, "Unable to copy mock Python to %s (error %lu).\n",
                destination, (unsigned long)GetLastError());
        return 0;
    }
    return 1;
#else
    FILE *from = fopen(source, "rb");
    FILE *to = fopen(destination, "wb");
    if (from == NULL || to == NULL) {
        if (from != NULL) (void)fclose(from);
        if (to != NULL) (void)fclose(to);
        return 0;
    }
    unsigned char bytes[16384];
    int valid = 1;
    while (!feof(from)) {
        size_t count = fread(bytes, 1U, sizeof(bytes), from);
        if (count == 0U) break;
        if (fwrite(bytes, 1U, count, to) != count) { valid = 0; break; }
    }
    if (ferror(from)) valid = 0;
    if (fclose(from) != 0) valid = 0;
    if (fclose(to) != 0) valid = 0;
    return valid && chmod(destination, 0755) == 0;
#endif
}

static int SetEnvironmentChecked(const char *name, const char *value)
{
#if defined(_WIN32)
    return _putenv_s(name, value) == 0;
#else
    return setenv(name, value, 1) == 0;
#endif
}

static int ClearEnvironmentChecked(const char *name)
{
#if defined(_WIN32)
    return _putenv_s(name, "") == 0;
#else
    return unsetenv(name) == 0;
#endif
}

static int MakeFolderChecked(const char *folder)
{
    if (Mkdir(folder) == 0) return 1;
    if (errno == EEXIST) return 1;
    fprintf(stderr, "Cannot create test directory: %s (errno %d).\n", folder, errno);
    return 0;
}

static int FileExists(const char *name)
{
    FILE *file = fopen(name, "rb");
    if (file == NULL) return 0;
    return fclose(file) == 0;
}

int main(int argc, char **argv)
{
    EDUCATION_CHECK(argc == 3);
    char configuration[UMICOM_EDUCATION_PATH_CAPACITY];
    char page[UMICOM_EDUCATION_PATH_CAPACITY];
    char environment[UMICOM_EDUCATION_PATH_CAPACITY];
    char interpreter[UMICOM_EDUCATION_PATH_CAPACITY];
    char spacedFolder[UMICOM_EDUCATION_PATH_CAPACITY];
    char spacedInterpreter[UMICOM_EDUCATION_PATH_CAPACITY];
    EDUCATION_CHECK(MakeFolderChecked(argv[1]));
    EDUCATION_CHECK(snprintf(configuration, sizeof(configuration), "%s/mkdocs.yml", argv[1]) > 0);
    FILE *file = fopen(configuration, "wb");
    EDUCATION_CHECK(file != NULL);
    EDUCATION_CHECK(fputs("site_name: Renderer test\n", file) >= 0);
    EDUCATION_CHECK(fclose(file) == 0);
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_TEST_BUILD_FAIL"));
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_TEST_NO_RENDERER"));
    EDUCATION_CHECK(UmicomEducationCheckRenderer(argv[1], argv[2]) == 0);
    EDUCATION_CHECK(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 0);

    /* A repository-local .venv must override unrelated Python on PATH. */
    EDUCATION_CHECK(snprintf(environment, sizeof(environment), "%s/.venv", argv[1]) > 0);
    EDUCATION_CHECK(MakeFolderChecked(environment));
#if defined(_WIN32)
    EDUCATION_CHECK(snprintf(interpreter, sizeof(interpreter), "%s/.venv/Scripts", argv[1]) > 0);
    EDUCATION_CHECK(MakeFolderChecked(interpreter));
    EDUCATION_CHECK(snprintf(interpreter, sizeof(interpreter), "%s/.venv/Scripts/python.exe", argv[1]) > 0);
#else
    EDUCATION_CHECK(snprintf(interpreter, sizeof(interpreter), "%s/.venv/bin", argv[1]) > 0);
    EDUCATION_CHECK(MakeFolderChecked(interpreter));
    EDUCATION_CHECK(snprintf(interpreter, sizeof(interpreter), "%s/.venv/bin/python", argv[1]) > 0);
#endif
    EDUCATION_CHECK(CopyExecutableChecked(argv[2], interpreter));
    EDUCATION_CHECK(SetEnvironmentChecked("UMICOM_EDUCATION_PYTHON", "intentionally-invalid-python-on-path"));
    EDUCATION_CHECK(UmicomEducationBuildSite(argv[1]) == 0);
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_PYTHON"));

    EDUCATION_CHECK(snprintf(page, sizeof(page), "%s/site/index.html", argv[1]) > 0);
    EDUCATION_CHECK(FileExists(page));
    EDUCATION_CHECK(SetEnvironmentChecked("UMICOM_EDUCATION_TEST_BUILD_FAIL", "1"));
    EDUCATION_CHECK(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 1);
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_TEST_BUILD_FAIL"));
    EDUCATION_CHECK(SetEnvironmentChecked("UMICOM_EDUCATION_TEST_NO_RENDERER", "1"));
    EDUCATION_CHECK(UmicomEducationBuildSiteWithPython(argv[1], argv[2]) == 1);
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_TEST_NO_RENDERER"));
    EDUCATION_CHECK(UmicomEducationCheckRenderer(argv[1], "not-an-executable-education-fixture-404") == 1);

    /* Paths containing spaces are allowed; arguments still stay separate. */
    EDUCATION_CHECK(snprintf(spacedFolder, sizeof(spacedFolder), "%s/renderer fixture with spaces", argv[1]) > 0);
    EDUCATION_CHECK(MakeFolderChecked(spacedFolder));
#if defined(_WIN32)
    EDUCATION_CHECK(snprintf(spacedInterpreter, sizeof(spacedInterpreter), "%s/python test.exe", spacedFolder) > 0);
#else
    EDUCATION_CHECK(snprintf(spacedInterpreter, sizeof(spacedInterpreter), "%s/python test", spacedFolder) > 0);
#endif
    EDUCATION_CHECK(CopyExecutableChecked(argv[2], spacedInterpreter));
    EDUCATION_CHECK(UmicomEducationCheckRenderer(argv[1], spacedInterpreter) == 0);

    /* The original regression checked only the probe. Build in the same
     * spaced directory so both --config-file and --site-dir must survive
     * Win32 argv translation as intact, separate arguments. */
    EDUCATION_CHECK(snprintf(configuration, sizeof(configuration),
                             "%s/mkdocs.yml", spacedFolder) > 0);
    file = fopen(configuration, "wb");
    EDUCATION_CHECK(file != NULL);
    EDUCATION_CHECK(fputs("site_name: Spaced renderer fixture\n", file) >= 0);
    EDUCATION_CHECK(fclose(file) == 0);
    EDUCATION_CHECK(UmicomEducationBuildSiteWithPython(spacedFolder, spacedInterpreter) == 0);
    EDUCATION_CHECK(snprintf(page, sizeof(page), "%s/site/index.html", spacedFolder) > 0);
    EDUCATION_CHECK(FileExists(page));

    /* A success exit code is not enough. Test the negative postcondition in
     * an isolated folder, where the old rendered page cannot mask failure. */
    char noOutputFolder[UMICOM_EDUCATION_PATH_CAPACITY];
    EDUCATION_CHECK(snprintf(noOutputFolder, sizeof(noOutputFolder),
                             "%s/no-output-case", argv[1]) > 0);
    EDUCATION_CHECK(MakeFolderChecked(noOutputFolder));
    EDUCATION_CHECK(snprintf(configuration, sizeof(configuration),
                             "%s/mkdocs.yml", noOutputFolder) > 0);
    file = fopen(configuration, "wb");
    EDUCATION_CHECK(file != NULL);
    EDUCATION_CHECK(fputs("site_name: Empty child output\n", file) >= 0);
    EDUCATION_CHECK(fclose(file) == 0);
    EDUCATION_CHECK(snprintf(page, sizeof(page), "%s/site/index.html", noOutputFolder) > 0);
    /* Only the disposable CTest fixture may be cleaned. Do not touch the
     * learner's real website or an existing repository build directory. */
    if (FileExists(page)) EDUCATION_CHECK(remove(page) == 0);
    EDUCATION_CHECK(SetEnvironmentChecked("UMICOM_EDUCATION_TEST_NO_SITE_OUTPUT", "1"));
    EDUCATION_CHECK(UmicomEducationBuildSiteWithPython(noOutputFolder, argv[2]) == 1);
    EDUCATION_CHECK(ClearEnvironmentChecked("UMICOM_EDUCATION_TEST_NO_SITE_OUTPUT"));

    puts("[OK] Renderer argv boundaries, spaced paths, local venv and failure evidence.");
    return 0;
}
