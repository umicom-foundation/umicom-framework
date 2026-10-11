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
int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
        return getenv("UMICOM_EDUCATION_TEST_NO_RENDERER") != NULL ? 1 : 0;
    if (argc < 9 || strcmp(argv[1], "-m") != 0 ||
        strcmp(argv[2], "mkdocs") != 0 || strcmp(argv[3], "build") != 0)
        return 2;
    if (getenv("UMICOM_EDUCATION_TEST_BUILD_FAIL") != NULL) return 3;
    const char *outputDirectory = NULL;
    for (int i = 4; i + 1 < argc; ++i)
        if (strcmp(argv[i], "--site-dir") == 0) outputDirectory = argv[++i];
    if (outputDirectory == NULL) return 2;
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
