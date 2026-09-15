/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_foundations_resources.c
 *
 * PURPOSE:
 *   Verify every lesson points at its own complete, readable HTML resource.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/foundations_curriculum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Paths in the legacy public catalogue are Applications-root-relative.
 * This test receives the canonical Framework checkout, so only the exact
 * fixed prefix is removed; no application-owned content is copied. */
int main(int argc, char **argv)
{
    const char prefix[] = "framework/";
    size_t checked = 0U;
    if (argc != 2 || argv[1][0] == '\0') {
        (void)fprintf(stderr, "Expected one Framework source directory.\n");
        return EXIT_FAILURE;
    }
    for (size_t index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        const UmiTeacherFoundationsLesson *lesson = umi_teacher_foundations_curriculum_at(index);
        char path[4096];
        char marker[256];
        long length;
        char *text;
        FILE *file;
        int written;
        if (lesson == NULL || strncmp(lesson->resource_path, prefix, sizeof(prefix) - 1U) != 0) {
            (void)fprintf(stderr, "Lesson %zu has no canonical Framework resource path.\n", index);
            return EXIT_FAILURE;
        }
        written = snprintf(path, sizeof(path), "%s/%s", argv[1],
                           lesson->resource_path + sizeof(prefix) - 1U);
        if (written < 0 || (size_t)written >= sizeof(path)) return EXIT_FAILURE;
        file = fopen(path, "rb");
        if (file == NULL) {
            (void)fprintf(stderr, "%s: missing lesson %s\n", lesson->id, path);
            return EXIT_FAILURE;
        }
        if (fseek(file, 0L, SEEK_END) != 0 || (length = ftell(file)) <= 0L ||
            length > 2L * 1024L * 1024L || fseek(file, 0L, SEEK_SET) != 0) {
            (void)fclose(file);
            return EXIT_FAILURE;
        }
        text = malloc((size_t)length + 1U);
        if (text == NULL) { (void)fclose(file); return EXIT_FAILURE; }
        if (fread(text, 1U, (size_t)length, file) != (size_t)length) {
            free(text); (void)fclose(file); return EXIT_FAILURE;
        }
        if (fclose(file) != 0) { free(text); return EXIT_FAILURE; }
        text[length] = '\0';
        written = snprintf(marker, sizeof(marker),
            "name=\"umicom-lesson-id\" content=\"%s\"", lesson->id);
        if (written < 0 || (size_t)written >= sizeof(marker) ||
            strstr(text, marker) == NULL || strstr(text, "</html>") == NULL) {
            (void)fprintf(stderr, "%s: wrong identity or incomplete HTML in %s\n", lesson->id, path);
            free(text);
            return EXIT_FAILURE;
        }
        free(text);
        ++checked;
    }
    if (checked == 0U) return EXIT_FAILURE;
    (void)printf("Verified %zu lesson resources.\n", checked);
    return EXIT_SUCCESS;
}
