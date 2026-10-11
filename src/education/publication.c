/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/publication.c
 * PURPOSE:
 *   Make a deterministic, read-only inventory of built educational pages and
 *   retain the Framework's existing SHA-256 implementation as sole digest
 *   authority. A reviewer must explicitly approve any changed manifest.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/publication.h"
#include "umicom/education/site.h"
#include "umicom/base/sha256.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
# include <windows.h>
# include <direct.h>
# include <process.h>
# define PubMkdir(path) _mkdir(path)
# define PubPid() ((unsigned long)_getpid())
#else
# include <dirent.h>
# include <unistd.h>
# define PubMkdir(path) mkdir(path, 0755)
# define PubPid() ((unsigned long)getpid())
#endif

#define PUB_MAX_FILES 20000U
#define PUB_MAX_DEPTH 24U
#define PUB_LINE_CAPACITY (UMICOM_EDUCATION_PATH_CAPACITY + 80U)
#define PUB_READ_CAPACITY 32768U

typedef struct PubEntry {
    char *name;
    char sha256[UMI_SHA256_HEX_CAPACITY];
} PubEntry;

typedef struct PubInventory {
    PubEntry *entries;
    size_t count;
    size_t capacity;
    uint64_t totalBytes;
} PubInventory;

static int PubJoin(char *result, size_t capacity, const char *left, const char *right)
{
    int count = snprintf(result, capacity, "%s/%s", left, right);
    return count >= 0 && (size_t)count < capacity;
}

static int PubNameValid(const char *name)
{
    if (name == NULL || *name == '\0' || strcmp(name, ".") == 0 ||
        strcmp(name, "..") == 0) return 0;
    for (const unsigned char *p = (const unsigned char *)name; *p != 0U; ++p) {
        /* A sha256sum-style manifest has one relative path per line. Refuse
         * ambiguous separators, controls and Windows-special path syntax. */
        if (*p < 0x20U || *p == 0x7fU || *p == '/' || *p == '\\' ||
            *p == ':' || *p == '?' || *p == '*' || *p == '<' ||
            *p == '>' || *p == '|' || *p == '"') return 0;
    }
    return 1;
}

static int PubRegular(const char *path)
{
#if defined(_WIN32)
    DWORD flags = GetFileAttributesA(path);
    return flags != INVALID_FILE_ATTRIBUTES &&
           (flags & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == 0U;
#else
    struct stat info;
    return lstat(path, &info) == 0 && S_ISREG(info.st_mode);
#endif
}

static int PubDirectory(const char *path)
{
#if defined(_WIN32)
    DWORD flags = GetFileAttributesA(path);
    return flags != INVALID_FILE_ATTRIBUTES &&
           (flags & FILE_ATTRIBUTE_DIRECTORY) != 0U &&
           (flags & FILE_ATTRIBUTE_REPARSE_POINT) == 0U;
#else
    struct stat info;
    return lstat(path, &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

static void PubFree(PubInventory *inventory)
{
    for (size_t i = 0U; i < inventory->count; ++i) free(inventory->entries[i].name);
    free(inventory->entries);
    *inventory = (PubInventory){0};
}

static int PubAdd(PubInventory *inventory, const char *relative,
                  const char sha256[UMI_SHA256_HEX_CAPACITY], uint64_t bytes)
{
    if (inventory->count == PUB_MAX_FILES || UINT64_MAX - inventory->totalBytes < bytes)
        return 0;
    if (inventory->count == inventory->capacity) {
        size_t next = inventory->capacity == 0U ? 32U : inventory->capacity * 2U;
        if (next > PUB_MAX_FILES) next = PUB_MAX_FILES;
        PubEntry *expanded = realloc(inventory->entries, next * sizeof(*expanded));
        if (expanded == NULL) return 0;
        inventory->entries = expanded;
        inventory->capacity = next;
    }
    size_t length = strlen(relative);
    char *owned = malloc(length + 1U);
    if (owned == NULL) return 0;
    memcpy(owned, relative, length + 1U);
    inventory->entries[inventory->count].name = owned;
    memcpy(inventory->entries[inventory->count].sha256, sha256, UMI_SHA256_HEX_CAPACITY);
    ++inventory->count;
    inventory->totalBytes += bytes;
    return 1;
}

static int PubDigestFile(const char *path, char digest[UMI_SHA256_HEX_CAPACITY],
                         uint64_t *outBytes)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    UmiSha256 hash;
    UmiSha256Init(&hash);
    uint64_t total = 0U;
    unsigned char bytes[PUB_READ_CAPACITY];
    int okay = 1;
    for (;;) {
        size_t count = fread(bytes, 1U, sizeof(bytes), file);
        if (count > 0U) {
            if (UINT64_MAX - total < (uint64_t)count ||
                UmiSha256Update(&hash, bytes, count) != UMI_STATUS_OK) {
                okay = 0;
                break;
            }
            total += (uint64_t)count;
        }
        if (count < sizeof(bytes)) {
            if (ferror(file)) okay = 0;
            break;
        }
    }
    if (fclose(file) != 0) okay = 0;
    unsigned char raw[UMI_SHA256_BYTES];
    if (!okay || UmiSha256Final(&hash, raw) != UMI_STATUS_OK) return 0;
    UmiSha256Hex(raw, digest);
    *outBytes = total;
    return 1;
}

static int PubVisit(const char *site, const char *relative, size_t depth,
                    PubInventory *inventory)
{
    if (depth > PUB_MAX_DEPTH) {
        fputs("Website directory nesting exceeds the publication limit.\n", stderr);
        return 0;
    }
    char root[UMICOM_EDUCATION_PATH_CAPACITY];
    if (relative[0] != '\0' && !PubJoin(root, sizeof(root), site, relative)) return 0;
    if (relative[0] == '\0') {
        if (strlen(site) >= sizeof(root)) return 0;
        strcpy(root, site);
    }
    if (!PubDirectory(root)) {
        fprintf(stderr, "Refusing missing, linked or invalid site directory: %s\n", root);
        return 0;
    }
#if defined(_WIN32)
    char glob[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!PubJoin(glob, sizeof(glob), root, "*")) return 0;
    WIN32_FIND_DATAA found;
    HANDLE handle = FindFirstFileA(glob, &found);
    if (handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND;
    }
    int success = 1;
    do {
        const char *name = found.cFileName;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        if (!PubNameValid(name) || (found.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) {
            fprintf(stderr, "Refusing ambiguous or linked site entry: %s\n", name);
            success = 0; break;
        }
        char child[UMICOM_EDUCATION_PATH_CAPACITY], absolute[UMICOM_EDUCATION_PATH_CAPACITY];
        if (relative[0] == '\0') {
            if (strlen(name) >= sizeof(child)) { success = 0; break; }
            strcpy(child, name);
        } else if (!PubJoin(child, sizeof(child), relative, name)) { success = 0; break; }
        if (!PubJoin(absolute, sizeof(absolute), site, child)) { success = 0; break; }
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U) {
            if (!PubVisit(site, child, depth + 1U, inventory)) { success = 0; break; }
        } else {
            char hash[UMI_SHA256_HEX_CAPACITY]; uint64_t bytes = 0U;
            if (!PubRegular(absolute) || !PubDigestFile(absolute, hash, &bytes) ||
                !PubAdd(inventory, child, hash, bytes)) { success = 0; break; }
        }
    } while (FindNextFileA(handle, &found));
    if (GetLastError() != ERROR_NO_MORE_FILES) success = 0;
    FindClose(handle);
    return success;
#else
    DIR *directory = opendir(root);
    if (directory == NULL) return 0;
    int success = 1;
    struct dirent *found = NULL;
    errno = 0;
    while ((found = readdir(directory)) != NULL) {
        const char *name = found->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        if (!PubNameValid(name)) { success = 0; break; }
        char child[UMICOM_EDUCATION_PATH_CAPACITY], absolute[UMICOM_EDUCATION_PATH_CAPACITY];
        if (relative[0] == '\0') {
            if (strlen(name) >= sizeof(child)) { success = 0; break; }
            strcpy(child, name);
        } else if (!PubJoin(child, sizeof(child), relative, name)) { success = 0; break; }
        if (!PubJoin(absolute, sizeof(absolute), site, child)) { success = 0; break; }
        if (PubDirectory(absolute)) {
            if (!PubVisit(site, child, depth + 1U, inventory)) { success = 0; break; }
        } else {
            char hash[UMI_SHA256_HEX_CAPACITY]; uint64_t bytes = 0U;
            if (!PubRegular(absolute) || !PubDigestFile(absolute, hash, &bytes) ||
                !PubAdd(inventory, child, hash, bytes)) { success = 0; break; }
        }
        errno = 0;
    }
    if (success && errno != 0) success = 0;
    if (closedir(directory) != 0) success = 0;
    return success;
#endif
}

static int PubOrder(const void *left, const void *right)
{
    const PubEntry *a = left, *b = right;
    return strcmp(a->name, b->name);
}

static int PubInventorySite(const char *repo, PubInventory *inventory)
{
    char site[UMICOM_EDUCATION_PATH_CAPACITY];
    char home[UMICOM_EDUCATION_PATH_CAPACITY];
    if (repo == NULL || !PubJoin(site, sizeof(site), repo, "site") ||
        !PubJoin(home, sizeof(home), site, "index.html") ||
        !PubDirectory(site) || !PubRegular(home)) {
        fputs("Website is missing site/index.html. Build it before publication.\n", stderr);
        return 0;
    }
    if (!PubVisit(site, "", 0U, inventory)) return 0;
    qsort(inventory->entries, inventory->count, sizeof(*inventory->entries), PubOrder);
    for (size_t i = 1U; i < inventory->count; ++i) {
        if (strcmp(inventory->entries[i - 1U].name, inventory->entries[i].name) == 0) return 0;
    }
    return 1;
}

static int PubSame(const char *left, const char *right)
{
    FILE *a = fopen(left, "rb"), *b = fopen(right, "rb");
    if (a == NULL || b == NULL) { if (a) fclose(a); if (b) fclose(b); return 0; }
    unsigned char x[4096], y[4096];
    int same = 1;
    for (;;) {
        size_t n = fread(x, 1U, sizeof(x), a), m = fread(y, 1U, sizeof(y), b);
        if (n != m || memcmp(x, y, n) != 0) { same = 0; break; }
        if (n < sizeof(x)) {
            if (ferror(a) || ferror(b)) same = 0;
            break;
        }
    }
    if (fclose(a) != 0) same = 0;
    if (fclose(b) != 0) same = 0;
    return same;
}

static int PubManifestPath(const char *repo, char *path, size_t capacity)
{
    char output[UMICOM_EDUCATION_PATH_CAPACITY];
    char native[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!PubJoin(output, sizeof(output), repo, "output")) return 0;
    if (!PubDirectory(output) && (PubMkdir(output) != 0 || !PubDirectory(output))) return 0;
    if (!PubJoin(native, sizeof(native), output, "native-publication")) return 0;
    if (!PubDirectory(native) && (PubMkdir(native) != 0 || !PubDirectory(native))) return 0;
    return PubJoin(path, capacity, native, "site.sha256");
}

static int PubOutputPath(const char *repo, char *path, size_t capacity)
{
    return PubJoin(path, capacity, repo, "output/native-publication/site.sha256");
}

static int PubTemp(const char *target, char *temporary, size_t capacity)
{
    int count = snprintf(temporary, capacity, "%s.part-%lu", target, PubPid());
    return count >= 0 && (size_t)count < capacity;
}

static int PubReplace(const char *from, const char *to)
{
#if defined(_WIN32)
    return MoveFileExA(from, to, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(from, to) == 0;
#endif
}

int UmicomEducationPublicationManifest(const char *repositoryRoot, int refresh)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0') return 2;
    PubInventory inventory = {0};
    if (!PubInventorySite(repositoryRoot, &inventory)) { PubFree(&inventory); return 1; }
    char destination[UMICOM_EDUCATION_PATH_CAPACITY], temporary[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!PubManifestPath(repositoryRoot, destination, sizeof(destination)) ||
        !PubTemp(destination, temporary, sizeof(temporary))) { PubFree(&inventory); return 1; }
    if (PubRegular(temporary) || PubDirectory(temporary)) {
        fprintf(stderr, "Temporary publication output already exists: %s\n", temporary);
        PubFree(&inventory);
        return 1;
    }
    FILE *stream = fopen(temporary, "wb");
    if (stream == NULL) { PubFree(&inventory); return 1; }
    int success = 1;
    for (size_t i = 0U; i < inventory.count; ++i) {
        if (fprintf(stream, "%s  %s\n", inventory.entries[i].sha256,
                    inventory.entries[i].name) < 0) { success = 0; break; }
    }
    if (fclose(stream) != 0) success = 0;
    if (!success) { remove(temporary); PubFree(&inventory); return 1; }
    if (PubRegular(destination) && PubSame(destination, temporary)) {
        remove(temporary);
        printf("[OK] Existing publication manifest unchanged: %zu files\n", inventory.count);
        PubFree(&inventory);
        return 0;
    }
    if ((PubRegular(destination) || PubDirectory(destination)) && !refresh) {
        fprintf(stderr, "Manifest differs; existing record preserved. Review changes, then use --refresh: %s\n", destination);
        remove(temporary);
        PubFree(&inventory);
        return 1;
    }
    if (!PubReplace(temporary, destination)) {
        perror("Cannot publish website inventory");
        remove(temporary);
        PubFree(&inventory);
        return 1;
    }
    printf("[OK] Site inventory: %zu files; %" PRIu64 " bytes\n",
           inventory.count, inventory.totalBytes);
    printf("[OK] SHA-256 manifest: %s\n", destination);
    PubFree(&inventory);
    return 0;
}

int UmicomEducationPublicationVerify(const char *repositoryRoot)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0') return 2;
    char manifest[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!PubOutputPath(repositoryRoot, manifest, sizeof(manifest)) || !PubRegular(manifest)) {
        fputs("Publication manifest missing; run `publish manifest` after building the site.\n", stderr);
        return 1;
    }
    PubInventory inventory = {0};
    if (!PubInventorySite(repositoryRoot, &inventory)) { PubFree(&inventory); return 1; }
    FILE *stream = fopen(manifest, "rb");
    if (stream == NULL) { PubFree(&inventory); return 1; }
    char line[PUB_LINE_CAPACITY];
    size_t index = 0U;
    int okay = 1;
    while (fgets(line, sizeof(line), stream) != NULL) {
        size_t length = strlen(line);
        if (length == sizeof(line) - 1U && line[length - 1U] != '\n') { okay = 0; break; }
        if (length > 0U && line[length - 1U] == '\n') line[--length] = '\0';
        if (length > 0U && line[length - 1U] == '\r') line[--length] = '\0';
        if (length < 67U || line[64] != ' ' || line[65] != ' ' ||
            index >= inventory.count) { okay = 0; break; }
        for (size_t j = 0U; j < 64U; ++j) {
            if (!((line[j] >= '0' && line[j] <= '9') || (line[j] >= 'a' && line[j] <= 'f'))) {
                okay = 0; break;
            }
        }
        if (!okay || strcmp(line + 66U, inventory.entries[index].name) != 0 ||
            strncmp(line, inventory.entries[index].sha256, 64U) != 0) { okay = 0; break; }
        ++index;
    }
    if (ferror(stream)) okay = 0;
    if (fclose(stream) != 0) okay = 0;
    if (!okay || index != inventory.count) {
        fprintf(stderr, "[FAILED] Site content differs from %s (first differing entry %zu).\n",
                manifest, index + 1U);
        PubFree(&inventory);
        return 1;
    }
    printf("[OK] Verified %zu website files using Framework SHA-256.\n", inventory.count);
    PubFree(&inventory);
    return 0;
}
