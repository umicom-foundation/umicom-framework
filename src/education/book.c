/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/book.c
 *
 * PURPOSE:
 *   Catalogue the existing Bits2Banking volume manifests and assemble one
 *   reviewable Markdown manuscript, without changing the original chapters or
 *   requiring Python, PowerShell, Pandoc or a Markdown rendering dependency.
 *
 *   This is a deliberately bounded, read-only YAML SUBSET for the existing
 *   `title` and `chapters` fields. It is NOT a general-purpose YAML parser.
 *   Other volume metadata stays in the original manifests for BookForge.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
# include <direct.h>
# include <windows.h>
# include <process.h>
# include <io.h>
# define EducationMkdir(path) _mkdir(path)
#else
# include <dirent.h>
# include <unistd.h>
# define EducationMkdir(path) mkdir(path, 0755)
#endif

#define EDUCATION_BOOK_CHAPTERS 128U
#define EDUCATION_BOOK_LINE 4096U
#define EDUCATION_BOOK_CHAPTER_LIMIT (16U * 1024U * 1024U)
#define EDUCATION_BOOK_VOLUMES 128U

typedef struct EducationBook {
    char slug[128];
    char title[512];
    char chapters[EDUCATION_BOOK_CHAPTERS][256];
    size_t count;
} EducationBook;

static int BookJoin(char *output, size_t cap, const char *root, const char *part)
{
    int length = snprintf(output, cap, "%s/%s", root, part);
    return length >= 0 && (size_t)length < cap;
}

static int BookDirectory(const char *path)
{
#if defined(_WIN32)
    DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
           (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0;
#else
    struct stat metadata;
    return lstat(path, &metadata) == 0 && S_ISDIR(metadata.st_mode);
#endif
}

static int BookFile(const char *path)
{
#if defined(_WIN32)
    DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
           (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0;
#else
    struct stat metadata;
    return lstat(path, &metadata) == 0 && S_ISREG(metadata.st_mode);
#endif
}

static int BookCreateDirectory(const char *path)
{
    if (BookDirectory(path)) return 1;
    if (EducationMkdir(path) == 0) return 1;
    return errno == EEXIST && BookDirectory(path);
}

static int BookValidSlug(const char *slug)
{
    if (slug == NULL || *slug == '\0' || strlen(slug) > 100U) return 0;
    for (const unsigned char *p = (const unsigned char *)slug; *p != '\0'; ++p)
        if (!(isalnum(*p) || *p == '_' || *p == '-')) return 0;
    return slug[0] != '.';
}

static int BookValidChapter(const char *name)
{
    if (name == NULL) return 0;
    size_t length = strlen(name);
    if (length < 4U || length > 240U || strcmp(name + length - 3U, ".md") != 0)
        return 0;
    if (name[0] == '.') return 0;
    for (const unsigned char *p = (const unsigned char *)name; *p != '\0'; ++p) {
        /* No absolute paths, traversal, Windows alternate streams or drives.
         * Subdirectories can be introduced later with a rooted-path contract. */
        if (!(isalnum(*p) || *p == '_' || *p == '-' || *p == '.')) return 0;
    }
    return 1;
}

static char *BookTrim(char *text)
{
    while (*text == ' ' || *text == '\t') ++text;
    size_t length = strlen(text);
    while (length > 0U && (text[length - 1U] == ' ' || text[length - 1U] == '\t' ||
           text[length - 1U] == '\r' || text[length - 1U] == '\n'))
        text[--length] = '\0';
    return text;
}

static void BookStripQuotes(char *text)
{
    size_t length = strlen(text);
    if (length >= 2U && ((text[0] == '"' && text[length - 1U] == '"') ||
                         (text[0] == '\'' && text[length - 1U] == '\''))) {
        memmove(text, text + 1U, length - 2U);
        text[length - 2U] = '\0';
    }
}

static int BookLoadManifest(const char *repositoryRoot, const char *slug, EducationBook *book)
{
    char relative[300], manifest[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!BookValidSlug(slug) || book == NULL ||
        snprintf(relative, sizeof(relative), "dropzone/documents/%s/volume.yml", slug) < 0 ||
        !BookJoin(manifest, sizeof(manifest), repositoryRoot, relative)) return 2;
    FILE *stream = fopen(manifest, "rb");
    if (stream == NULL) {
        fprintf(stderr, "Missing book manifest: %s\n", manifest);
        return 2;
    }
    memset(book, 0, sizeof(*book));
    memcpy(book->slug, slug, strlen(slug) + 1U);
    char buffer[EDUCATION_BOOK_LINE];
    int readingChapters = 0, sawList = 0;
    size_t lineNo = 0U;
    int result = 0;
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        ++lineNo;
        size_t size = strlen(buffer);
        if (size == sizeof(buffer) - 1U && buffer[size - 1U] != '\n') {
            fprintf(stderr, "Manifest line too long at %s:%zu\n", manifest, lineNo);
            result = 2;
            break;
        }
        char *start = buffer;
        if (lineNo == 1U && (unsigned char)buffer[0] == 0xefU &&
            (unsigned char)buffer[1] == 0xbbU && (unsigned char)buffer[2] == 0xbfU)
            start += 3;
        /* Leave the original bytes untouched. This subset reads only one
         * top-level title and a block sequence of literal chapter filenames. */
        int indented = *start == ' ' || *start == '\t';
        char *entry = BookTrim(start);
        if (*entry == '\0' || *entry == '#') continue;
        if (!indented && strncmp(entry, "title:", 6U) == 0) {
            char *title = BookTrim(entry + 6U);
            BookStripQuotes(title);
            if (strlen(title) >= sizeof(book->title)) { result = 2; break; }
            memcpy(book->title, title, strlen(title) + 1U);
            readingChapters = 0;
        } else if (!indented && strcmp(entry, "chapters:") == 0) {
            readingChapters = 1;
            sawList = 1;
        } else if (!indented) {
            readingChapters = 0;
        } else if (readingChapters && entry[0] == '-' && entry[1] == ' ') {
            char *name = BookTrim(entry + 2U);
            BookStripQuotes(name);
            if (book->count >= EDUCATION_BOOK_CHAPTERS || !BookValidChapter(name)) {
                fprintf(stderr, "Invalid or excessive chapter at %s:%zu\n", manifest, lineNo);
                result = 2;
                break;
            }
            for (size_t k = 0U; k < book->count; ++k) {
                if (strcmp(book->chapters[k], name) == 0) {
                    fprintf(stderr, "Repeated chapter at %s:%zu\n", manifest, lineNo);
                    result = 2;
                    break;
                }
            }
            if (result != 0) break;
            memcpy(book->chapters[book->count++], name, strlen(name) + 1U);
        }
    }
    if (ferror(stream) != 0) result = 1;
    if (fclose(stream) != 0) result = 1;
    if (result == 0 && (!sawList || book->count == 0U || book->title[0] == '\0')) {
        fprintf(stderr, "Manifest lacks a title or chapter list: %s\n", manifest);
        return 2;
    }
    return result;
}

static int BookChapterPath(char *path, size_t capacity, const char *root,
                           const EducationBook *book, size_t number)
{
    char relative[450];
    int n = snprintf(relative, sizeof(relative), "dropzone/documents/%s/%s",
                     book->slug, book->chapters[number]);
    return n >= 0 && (size_t)n < sizeof(relative) && BookJoin(path, capacity, root, relative);
}

static int BookAllChaptersExist(const char *root, const EducationBook *book)
{
    char sourceDirectory[UMICOM_EDUCATION_PATH_CAPACITY];
    char relativeDirectory[300];
    if (snprintf(relativeDirectory, sizeof(relativeDirectory),
                 "dropzone/documents/%s", book->slug) < 0 ||
        !BookJoin(sourceDirectory, sizeof(sourceDirectory), root, relativeDirectory) ||
        !BookDirectory(sourceDirectory)) {
        fprintf(stderr, "Missing or linked source volume directory: %s\n", book->slug);
        return 0;
    }
    for (size_t i = 0U; i < book->count; ++i) {
        char path[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!BookChapterPath(path, sizeof(path), root, book, i) || !BookFile(path)) {
            fprintf(stderr, "Missing chapter: %s/%s\n", book->slug, book->chapters[i]);
            return 0;
        }
    }
    return 1;
}

static int BookCompareFiles(const char *leftPath, const char *rightPath)
{
    FILE *left = fopen(leftPath, "rb"), *right = fopen(rightPath, "rb");
    if (left == NULL || right == NULL) {
        if (left != NULL) fclose(left);
        if (right != NULL) fclose(right);
        return 0;
    }
    unsigned char l[8192], r[8192];
    int same = 1;
    do {
        size_t a = fread(l, 1U, sizeof(l), left);
        size_t b = fread(r, 1U, sizeof(r), right);
        if (a != b || memcmp(l, r, a) != 0) { same = 0; break; }
        if (a == 0U) { if (ferror(left) || ferror(right)) same = 0; break; }
    } while (1);
    fclose(left);
    fclose(right);
    return same;
}

/* Write a complete chapter with its original text intact except for an
 * optional UTF-8 BOM, which would otherwise appear in the middle of the book.
 * A bounded copy prevents one unexpected huge source from exhausting a small
 * educational workstation; the temporary output is discarded on failure. */
static int BookAppendChapter(FILE *destination, const char *path)
{
    FILE *source = fopen(path, "rb");
    if (source == NULL) return 0;
    unsigned char buffer[16384];
    size_t count = fread(buffer, 1U, 3U, source);
    size_t total = 0U;
    if (count != 3U || buffer[0] != 0xefU || buffer[1] != 0xbbU || buffer[2] != 0xbfU) {
        if (count != 0U && fwrite(buffer, 1U, count, destination) != count) { fclose(source); return 0; }
        total += count;
    }
    int passed = 1;
    while (!feof(source)) {
        count = fread(buffer, 1U, sizeof(buffer), source);
        if (count == 0U) break;
        if (count > EDUCATION_BOOK_CHAPTER_LIMIT - total ||
            fwrite(buffer, 1U, count, destination) != count) { passed = 0; break; }
        total += count;
    }
    if (ferror(source)) passed = 0;
    if (fclose(source) != 0) passed = 0;
    return passed;
}

static int BookReplace(const char *temporary, const char *destination)
{
#if defined(_WIN32)
    return MoveFileExA(temporary, destination,
                      MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(temporary, destination) == 0;
#endif
}

int UmicomEducationBookAssemble(const char *repositoryRoot, const char *volumeSlug, int refresh)
{
    if (repositoryRoot == NULL || !BookValidSlug(volumeSlug)) return 2;
    EducationBook book;
    int status = BookLoadManifest(repositoryRoot, volumeSlug, &book);
    if (status != 0) return status;
    if (!BookAllChaptersExist(repositoryRoot, &book)) return 2;
    char outputRoot[UMICOM_EDUCATION_PATH_CAPACITY];
    char outputDirectory[UMICOM_EDUCATION_PATH_CAPACITY];
    char outputName[150];
    char outputFile[UMICOM_EDUCATION_PATH_CAPACITY];
    char temporary[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!BookJoin(outputRoot, sizeof(outputRoot), repositoryRoot, "output") ||
        !BookCreateDirectory(outputRoot) ||
        !BookJoin(outputDirectory, sizeof(outputDirectory), outputRoot, "native-books") ||
        !BookCreateDirectory(outputDirectory)) {
        fputs("Cannot prepare output/native-books.\n", stderr);
        return 1;
    }
    int n = snprintf(outputName, sizeof(outputName), "%s.md", book.slug);
    if (n < 0 || (size_t)n >= sizeof(outputName) ||
        !BookJoin(outputFile, sizeof(outputFile), outputDirectory, outputName)) return 2;
#if defined(_WIN32)
    unsigned long processId = (unsigned long)_getpid();
#else
    unsigned long processId = (unsigned long)getpid();
#endif
    n = snprintf(temporary, sizeof(temporary), "%s.part-%lu", outputFile, processId);
    if (n < 0 || (size_t)n >= sizeof(temporary)) return 2;
    /* Treat an existing temporary file as a collision, not as scratch to erase. */
    if (BookFile(temporary)) {
        fprintf(stderr, "Temporary output already exists: %s\n", temporary);
        return 1;
    }
    FILE *output = fopen(temporary, "wb");
    if (output == NULL) return 1;
    int success = fprintf(output, "# %s\n\n", book.title) > 0;
    for (size_t i = 0U; success && i < book.count; ++i) {
        char path[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!BookChapterPath(path, sizeof(path), repositoryRoot, &book, i) ||
            !BookAppendChapter(output, path)) success = 0;
        if (success && fputs("\n\n---\n\n", output) == EOF) success = 0;
    }
    if (fflush(output) != 0) success = 0;
    if (fclose(output) != 0) success = 0;
    if (!success) {
        (void)remove(temporary); /* Delete only this unsuccessful generated scratch file. */
        fputs("Could not assemble all chapters; published output was not changed.\n", stderr);
        return 1;
    }
    if (BookFile(outputFile)) {
        if (BookCompareFiles(temporary, outputFile)) {
            (void)remove(temporary);
            printf("Unchanged book manuscript: %s\n", outputFile);
            return 0;
        }
        if (!refresh) {
            (void)remove(temporary);
            fprintf(stderr, "DIFFERENT (preserved): %s\nUse --refresh after comparing the existing manuscript.\n",
                    outputFile);
            return 3;
        }
    }
    if (!BookReplace(temporary, outputFile)) {
        (void)remove(temporary);
        fputs("Cannot publish completed manuscript. Earlier output was preserved.\n", stderr);
        return 1;
    }
    printf("Assembled %zu chapters into %s\n", book.count, outputFile);
    return 0;
}

/* Read the same manifest contract used by `book assemble`, but do not write
 * a manuscript. This gives a contributor a safe first validation command
 * before choosing whether to generate publication output. */
int UmicomEducationBookCheck(const char *repositoryRoot, const char *volumeSlug)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0' || !BookValidSlug(volumeSlug)) {
        fputs("Provide a source repository and one safe book volume slug.\n", stderr);
        return 2;
    }
    EducationBook book;
    int status = BookLoadManifest(repositoryRoot, volumeSlug, &book);
    if (status != 0) return status;
    if (!BookAllChaptersExist(repositoryRoot, &book)) return 2;
    printf("[OK] %s: %zu original chapters present; no files changed.\n",
           book.slug, book.count);
    return 0;
}

static int BookSortSlugs(const void *left, const void *right)
{
    return strcmp((const char *)left, (const char *)right);
}

int UmicomEducationBookList(const char *repositoryRoot)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0') return 2;
    char directory[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!BookJoin(directory, sizeof(directory), repositoryRoot, "dropzone/documents")) return 2;
    char slugs[EDUCATION_BOOK_VOLUMES][128];
    size_t count = 0U;
#if defined(_WIN32)
    char pattern[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!BookJoin(pattern, sizeof(pattern), directory, "*")) return 2;
    struct _finddata_t record;
    intptr_t scan = _findfirst(pattern, &record);
    if (scan == -1) { fputs("Cannot list book source directory.\n", stderr); return 1; }
    do {
        if ((record.attrib & _A_SUBDIR) == 0 || !BookValidSlug(record.name)) continue;
        char item[UMICOM_EDUCATION_PATH_CAPACITY];
        char manifest[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!BookJoin(item, sizeof(item), directory, record.name) ||
            !BookDirectory(item) ||
            !BookJoin(manifest, sizeof(manifest), item, "volume.yml") || !BookFile(manifest)) continue;
        if (count == EDUCATION_BOOK_VOLUMES) { _findclose(scan); return 2; }
        memcpy(slugs[count++], record.name, strlen(record.name) + 1U);
    } while (_findnext(scan, &record) == 0);
    _findclose(scan);
#else
    DIR *scan = opendir(directory);
    if (scan == NULL) { fputs("Cannot list book source directory.\n", stderr); return 1; }
    struct dirent *entry;
    while ((entry = readdir(scan)) != NULL) {
        if (!BookValidSlug(entry->d_name)) continue;
        char item[UMICOM_EDUCATION_PATH_CAPACITY];
        char manifest[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!BookJoin(item, sizeof(item), directory, entry->d_name) ||
            !BookDirectory(item) ||
            !BookJoin(manifest, sizeof(manifest), item, "volume.yml") || !BookFile(manifest)) continue;
        if (count == EDUCATION_BOOK_VOLUMES) { closedir(scan); return 2; }
        memcpy(slugs[count++], entry->d_name, strlen(entry->d_name) + 1U);
    }
    closedir(scan);
#endif
    qsort(slugs, count, sizeof(slugs[0]), BookSortSlugs);
    if (count == 0U) {
        puts("No volume.yml manifests found in dropzone/documents.");
        return 0;
    }
    int failures = 0;
    for (size_t i = 0U; i < count; ++i) {
        EducationBook book;
        int result = BookLoadManifest(repositoryRoot, slugs[i], &book);
        if (result != 0) { failures = 1; continue; }
        int complete = BookAllChaptersExist(repositoryRoot, &book);
        printf("%s | %zu chapters | %s%s\n", book.slug, book.count,
               book.title, complete ? "" : " [missing files]");
        if (!complete) failures = 1;
    }
    return failures;
}
