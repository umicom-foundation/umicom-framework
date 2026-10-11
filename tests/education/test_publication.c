/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_publication.c
 * PURPOSE:
 *   Exercise deterministic publication manifests and refusal paths against
 *   disposable files, without creating a live website or publishing to Git.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/publication.h"
#include "umicom/education/site.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
# include <direct.h>
# include <process.h>
# define TestMkdir(path) _mkdir(path)
# define TestPid() ((unsigned long)_getpid())
#else
# include <unistd.h>
# define TestMkdir(path) mkdir(path, 0755)
# define TestPid() ((unsigned long)getpid())
#endif

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Publication check failed: %s at line %d\n", #condition, __LINE__); \
        return 1; \
    } \
} while (0)

static int WriteText(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL) return 0;
    size_t length = strlen(text);
    int okay = fwrite(text, 1U, length, file) == length;
    return fclose(file) == 0 && okay;
}

static int Contains(const char *fileName, const char *needle)
{
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) return 0;
    char text[8192];
    size_t count = fread(text, 1U, sizeof(text) - 1U, file);
    int okay = !ferror(file);
    if (fclose(file) != 0) okay = 0;
    text[count] = '\0';
    return okay && strstr(text, needle) != NULL;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    char root[UMICOM_EDUCATION_PATH_CAPACITY];
    int length = snprintf(root, sizeof(root), "%s/publication-%lu", argv[1], TestPid());
    CHECK(length > 0 && (size_t)length < sizeof(root));
    CHECK(TestMkdir(root) == 0);
    char site[UMICOM_EDUCATION_PATH_CAPACITY], subdir[UMICOM_EDUCATION_PATH_CAPACITY];
    char home[UMICOM_EDUCATION_PATH_CAPACITY], css[UMICOM_EDUCATION_PATH_CAPACITY];
    char second[UMICOM_EDUCATION_PATH_CAPACITY], manifest[UMICOM_EDUCATION_PATH_CAPACITY];
    CHECK(snprintf(site, sizeof(site), "%s/site", root) > 0);
    CHECK(snprintf(subdir, sizeof(subdir), "%s/styles", site) > 0);
    CHECK(snprintf(home, sizeof(home), "%s/index.html", site) > 0);
    CHECK(snprintf(css, sizeof(css), "%s/umicom.css", subdir) > 0);
    CHECK(snprintf(second, sizeof(second), "%s/extra.html", site) > 0);
    CHECK(snprintf(manifest, sizeof(manifest), "%s/output/native-publication/site.sha256", root) > 0);
    CHECK(TestMkdir(site) == 0);
    CHECK(TestMkdir(subdir) == 0);
    CHECK(WriteText(home, "abc"));
    CHECK(WriteText(css, "body{}"));

    /* Manifest creation is explicit. Read-only verification cannot create one. */
    CHECK(UmicomEducationPublicationVerify(root) != 0);
    CHECK(UmicomEducationPublicationManifest(root, 0) == 0);
    CHECK(Contains(manifest,
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad  index.html"));
    CHECK(UmicomEducationPublicationVerify(root) == 0);
    CHECK(UmicomEducationPublicationManifest(root, 0) == 0);

    CHECK(WriteText(home, "abcd"));
    CHECK(UmicomEducationPublicationVerify(root) != 0);
    CHECK(UmicomEducationPublicationManifest(root, 0) != 0);
    CHECK(Contains(manifest,
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad  index.html"));
    CHECK(UmicomEducationPublicationManifest(root, 1) == 0);
    CHECK(UmicomEducationPublicationVerify(root) == 0);

    CHECK(WriteText(second, "Additional page"));
    CHECK(UmicomEducationPublicationVerify(root) != 0);
    CHECK(UmicomEducationPublicationManifest(root, 1) == 0);
    CHECK(UmicomEducationPublicationVerify(root) == 0);

#if !defined(_WIN32)
    /* Linked directories and files may escape the selected site: refuse them. */
    char linked[UMICOM_EDUCATION_PATH_CAPACITY];
    CHECK(snprintf(linked, sizeof(linked), "%s/linked-file", site) > 0);
    CHECK(symlink(home, linked) == 0);
    CHECK(UmicomEducationPublicationManifest(root, 0) != 0);
    CHECK(UmicomEducationPublicationVerify(root) != 0);
    CHECK(unlink(linked) == 0);
#endif
    CHECK(UmicomEducationPublicationVerify(root) == 0);
    puts("Publication manifest preserves existing evidence and detects changes.");
    return 0;
}
