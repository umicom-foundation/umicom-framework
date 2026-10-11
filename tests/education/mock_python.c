/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/mock_python.c
 * PURPOSE: Simulate a Python executable in a deterministic CTest fixture.
 *          Tests never require a real MkDocs installation or web access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <direct.h>
# define MakeDirectoryLocal(path) _mkdir(path)
#else
# define MakeDirectoryLocal(path) mkdir(path, 0755)
#endif
/* The original test-double accepted ANY -c expression, masking the Windows
 * argument-splitting bug. Retain it as evidence; the active implementation
 * verifies the exact argument and count delivered across the process ABI. */
#if 0
int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
        return getenv("UMICOM_EDUCATION_TEST_NO_RENDERER") != NULL ? 1 : 0;
    /* The previous mock accepted argc > 9 and ignored --config-file. That
     * could hide space splitting in the build argument vector. Preserve the
     * earlier tolerant behaviour for engineering review only. */
#if 0
    if (argc < 9 || strcmp(argv[1], "-m") != 0 ||
        strcmp(argv[2], "mkdocs") != 0 || strcmp(argv[3], "build") != 0)
        return 2;
    if (getenv("UMICOM_EDUCATION_TEST_BUILD_FAIL") != NULL) return 3;
    const char *outputDirectory = NULL;
    for (int i = 4; i + 1 < argc; ++i)
        if (strcmp(argv[i], "--site-dir") == 0) outputDirectory = argv[++i];
    if (outputDirectory == NULL) return 2;
#endif
    if (argc != 9 || strcmp(argv[1], "-m") != 0 ||
        strcmp(argv[2], "mkdocs") != 0 ||
        strcmp(argv[3], "build") != 0 ||
        strcmp(argv[4], "--strict") != 0 ||
        strcmp(argv[5], "--config-file") != 0 ||
        strcmp(argv[7], "--site-dir") != 0)
    {
        fprintf(stderr, "[FAIL] MkDocs argv boundaries changed (%d arguments).\n", argc);
        for (int i = 1; i < argc; ++i)
            fprintf(stderr, "  argv[%d]=<%s>\n", i, argv[i]);
        return 2;
    }
    FILE *configuration = fopen(argv[6], "rb");
    if (configuration == NULL)
    {
        fprintf(stderr, "[FAIL] Mock cannot read the exact --config-file argument: <%s>.\n", argv[6]);
        return 2;
    }
    if (fclose(configuration) != 0) return 2;
    if (getenv("UMICOM_EDUCATION_TEST_BUILD_FAIL") != NULL) return 3;
    if (getenv("UMICOM_EDUCATION_TEST_NO_SITE_OUTPUT") != NULL) return 0;
    const char *outputDirectory = argv[8];
    if (MakeDirectoryLocal(outputDirectory) != 0 && errno != EEXIST) return 1;
    char indexPage[4096];
    int result = snprintf(indexPage, sizeof(indexPage), "%s/index.html", outputDirectory);
    if (result <= 0 || (size_t)result >= sizeof(indexPage)) return 2;
    FILE *output = fopen(indexPage, "wb");
    if (output == NULL) return 1;
    const char page[] = "<!doctype html><title>Test page</title>";
    if (fwrite(page, 1U, sizeof(page) - 1U, output) != sizeof(page) - 1U) { fclose(output); return 1; }
    return fclose(output) == 0 ? 0 : 1;
}

#endif

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-c") == 0)
    {
        const char expected[] = "__import__('mkdocs');__import__('material')";
        if (argc != 3 || strcmp(argv[2], expected) != 0)
        {
            fprintf(stderr, "[FAIL] Renderer probe crossed the process boundary incorrectly (%d arguments).\n",
                    argc);
            for (int i = 1; i < argc; ++i)
                fprintf(stderr, "  argv[%d]=<%s>\n", i, argv[i]);
            return 4;
        }
        return getenv("UMICOM_EDUCATION_TEST_NO_RENDERER") != NULL ? 1 : 0;
    }
    /* The previous mock accepted argc > 9 and ignored --config-file. That
     * could hide space splitting in the build argument vector. Preserve the
     * earlier tolerant behaviour for engineering review only. */
#if 0
    if (argc < 9 || strcmp(argv[1], "-m") != 0 ||
        strcmp(argv[2], "mkdocs") != 0 || strcmp(argv[3], "build") != 0)
        return 2;
    if (getenv("UMICOM_EDUCATION_TEST_BUILD_FAIL") != NULL) return 3;
    const char *outputDirectory = NULL;
    for (int i = 4; i + 1 < argc; ++i)
        if (strcmp(argv[i], "--site-dir") == 0) outputDirectory = argv[++i];
    if (outputDirectory == NULL) return 2;
#endif
    if (argc != 9 || strcmp(argv[1], "-m") != 0 ||
        strcmp(argv[2], "mkdocs") != 0 ||
        strcmp(argv[3], "build") != 0 ||
        strcmp(argv[4], "--strict") != 0 ||
        strcmp(argv[5], "--config-file") != 0 ||
        strcmp(argv[7], "--site-dir") != 0)
    {
        fprintf(stderr, "[FAIL] MkDocs argv boundaries changed (%d arguments).\n", argc);
        for (int i = 1; i < argc; ++i)
            fprintf(stderr, "  argv[%d]=<%s>\n", i, argv[i]);
        return 2;
    }
    FILE *configuration = fopen(argv[6], "rb");
    if (configuration == NULL)
    {
        fprintf(stderr, "[FAIL] Mock cannot read the exact --config-file argument: <%s>.\n", argv[6]);
        return 2;
    }
    if (fclose(configuration) != 0) return 2;
    if (getenv("UMICOM_EDUCATION_TEST_BUILD_FAIL") != NULL) return 3;
    if (getenv("UMICOM_EDUCATION_TEST_NO_SITE_OUTPUT") != NULL) return 0;
    const char *outputDirectory = argv[8];
    if (MakeDirectoryLocal(outputDirectory) != 0 && errno != EEXIST) return 1;
    char indexPage[4096];
    int result = snprintf(indexPage, sizeof(indexPage), "%s/index.html", outputDirectory);
    if (result <= 0 || (size_t)result >= sizeof(indexPage)) return 2;
    FILE *output = fopen(indexPage, "wb");
    if (output == NULL) return 1;
    const char page[] = "<!doctype html><title>Test page</title>";
    if (fwrite(page, 1U, sizeof(page) - 1U, output) != sizeof(page) - 1U)
    {
        (void)fclose(output);
        return 1;
    }
    return fclose(output) == 0 ? 0 : 1;
}
