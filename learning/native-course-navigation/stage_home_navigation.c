/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-course-navigation/stage_home_navigation.c
 * PURPOSE: Produce complete, reviewable HTML files with accessible links to
 *          the continuation catalogues; NEVER overwrite published originals.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_home_link.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#define MakeDir(path) _mkdir(path)
#else
#define MakeDir(path) mkdir(path, 0755)
#endif

/* Root arguments identify trusted local repository and caller-owned staging
 * directories. Never accept a destination inside the educational source tree
 * by default; the operator compares the staged FULL files before merging. */
static int Join(char *buffer, size_t capacity, const char *root, const char *leaf)
{
    const int n = snprintf(buffer, capacity, "%s/%s", root, leaf);
    return n > 0 && (size_t)n < capacity;
}
static int Directory(const char *directory)
{
    struct stat statInfo;
    if (stat(directory, &statInfo) == 0) {
#if defined(_WIN32)
        return (statInfo.st_mode & _S_IFMT) == _S_IFDIR;
#else
        return S_ISDIR(statInfo.st_mode);
#endif
    }
    return MakeDir(directory) == 0;
}
static int Stage(const char *repo, const char *destination, const char *sourceRelative,
                 const char *targetRelative, const char *href, const char *label)
{
    char sourcePath[4096], targetPath[4096];
    char *input = NULL, *output = NULL;
    FILE *file = NULL;
    long fileSize = 0L;
    size_t written = 0U, size;
    int success = 0;
    if (!Join(sourcePath, sizeof(sourcePath), repo, sourceRelative) ||
        !Join(targetPath, sizeof(targetPath), destination, targetRelative)) goto cleanup;
    file = fopen(sourcePath, "rb");
    if (file == NULL || fseek(file, 0L, SEEK_END) != 0) goto cleanup;
    fileSize = ftell(file);
    if (fileSize <= 0L || (unsigned long)fileSize > UMI_EDUCATION_HOME_MAX_SOURCE_BYTES ||
        fseek(file, 0L, SEEK_SET) != 0) goto cleanup;
    size = (size_t)fileSize;
    input = malloc(size);
    output = malloc(size + 8192U);
    if (input == NULL || output == NULL || fread(input, 1U, size, file) != size) goto cleanup;
    if (fclose(file) != 0) {file = NULL; goto cleanup;}
    file = NULL;
    if (UmiEducationCourseHomeLinkCompose(input, size, href, label,
            output, size + 8192U, &written) != UMI_STATUS_OK) goto cleanup;

    /* x is the standard exclusive-create file-mode modifier. Never truncate
     * another generated file; a previous review must be moved deliberately. */
    file = fopen(targetPath, "wbx");
    if (file == NULL) {fprintf(stderr, "Refusing existing/unwritable output: %s\n", targetPath); goto cleanup;}
    size_t stored = fwrite(output, 1U, written, file);
    int closeStatus = fclose(file);
    file = NULL;
    if (stored != written || closeStatus != 0) {
        /* The exclusive create succeeded, so only OUR incomplete staging
         * file may be discarded. An existing file was never opened. */
        (void)remove(targetPath);
        fprintf(stderr, "Failed to publish complete staged output: %s\n", targetPath);
        goto cleanup;
    }
    printf("Staged complete HTML: %s (%zu bytes)\n", targetPath, written);
    success = 1;
cleanup:
    if (file != NULL) fclose(file);
    free(output);
    free(input);
    return success;
}
int main(int argc, char **argv)
{
    char llm[4096], maths[4096];
    if (argc != 5 || strcmp(argv[1], "--repo") != 0 ||
        strcmp(argv[3], "--output") != 0) {
        fprintf(stderr,"Usage: umicom-native-course-navigation --repo REPO --output EMPTY_STAGING_DIRECTORY\n");
        return 2;
    }
    if (!Join(llm, sizeof(llm), argv[4], "llm-from-scratch") ||
        !Join(maths, sizeof(maths), argv[4], "maths-for-ai")) return 2;
    if (!Directory(argv[4]) || !Directory(llm) || !Directory(maths)) {
        fputs("Cannot prepare reviewed-output directories.\n",stderr);
        return 2;
    }
    /* Source pages remain unchanged. The three review files are complete
     * replacements that may be merged individually after Beyond Compare. */
    int good = Stage(argv[2], argv[4], "content/courses/llm-from-scratch/index.html",
                      "llm-from-scratch/index.html", "advanced-lessons.html",
                      "Advanced LLM lessons and complete learning pathway");
    good &= Stage(argv[2], argv[4], "content/courses/llm-from-scratch/course-map.html",
                      "llm-from-scratch/course-map.html", "advanced-lessons.html",
                      "Browse the additional LLM lesson catalogue");
    good &= Stage(argv[2], argv[4], "content/courses/maths-for-ai/index.html",
                      "maths-for-ai/index.html", "advanced-units.html",
                      "Advanced Mathematics for AI units");
    if (!good) {
        fputs("Some staged pages were not created. Inspect outputs before merging.\n",stderr);
        return 1;
    }
    puts("Original HTML preserved. Compare the three staged complete documents with the published sources.");
    return 0;
}
