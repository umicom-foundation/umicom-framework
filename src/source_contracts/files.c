/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/files.c
 *
 * PURPOSE:
 *   Read UTF-8 paths and collect deterministic audit inputs without changing files.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#endif
#include "internal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
static wchar_t *widePath(const char *path)
{
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0) return NULL;
    wchar_t *out = malloc((size_t)count * sizeof(*out));
    if (out != NULL && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, out, count) != count) { free(out); out = NULL; }
    return out;
}
static char *utf8Path(const wchar_t *path)
{
    int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, NULL, 0, NULL, NULL);
    if (count <= 0) return NULL;
    char *out = malloc((size_t)count);
    if (out != NULL && WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path, -1, out, count, NULL, NULL) != count) { free(out); out = NULL; }
    return out;
}
#endif

char *ScJoin(const char *root, const char *path)
{
    if (root == NULL || path == NULL) return NULL;
    size_t a = strlen(root), b = strlen(path);
    if (a > SIZE_MAX - b - 2U) return NULL;
    char *joined = malloc(a + b + 2U);
    if (joined == NULL) return NULL;
    memcpy(joined, root, a); joined[a] = '/'; memcpy(joined + a + 1U, path, b + 1U);
    return joined;
}
char *ScRead(const char *path)
{
    FILE *file;
#if defined(_WIN32)
    wchar_t *wide = widePath(path);
    if (wide == NULL) return NULL;
    file = _wfopen(wide, L"rb"); free(wide);
#else
    file = fopen(path, "rb");
#endif
    if (file == NULL) return NULL;
    size_t capacity = 8192U, used = 0U;
    char *data = malloc(capacity + 1U);
    if (data == NULL) { fclose(file); return NULL; }
    for (;;) {
        size_t got = fread(data + used, 1U, capacity - used, file); used += got;
        if (ferror(file)) goto invalid;
        if (feof(file)) break;
        if (used == capacity) {
            if (capacity == SC_MAX_FILE_BYTES) {
                if (fgetc(file) != EOF || ferror(file)) goto invalid;
                break;
            }
            size_t next = capacity * 2U;
            if (next > SC_MAX_FILE_BYTES) next = SC_MAX_FILE_BYTES;
            char *grown = realloc(data, next + 1U);
            if (grown == NULL) goto invalid;
            data = grown; capacity = next;
        }
    }
    if (fclose(file) != 0) { free(data); return NULL; }
    if (memchr(data, '\0', used) != NULL) { free(data); return NULL; }
    data[used] = '\0'; return data;
invalid:
    fclose(file); free(data); return NULL;
}
static bool wanted(const char *name, unsigned kind)
{
    if (kind == 2U) return strcmp(name, "application.umicom.yaml") == 0;
    const char *dot = strrchr(name, '.');
    return dot != NULL && (kind == 1U ? strcmp(dot, ".h") == 0 : strcmp(dot, ".c") == 0 || strcmp(dot, ".inc") == 0);
}
static bool ignoredDirectory(const char *name)
{
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strcmp(name, ".git") == 0 ||
        strcmp(name, "build") == 0 || strncmp(name, "build-", 6U) == 0;
}
static int appendFile(ScFiles *files, const char *path)
{
    if (files->count == SC_MAX_FILES) return 2;
    if (files->count == files->capacity) {
        size_t capacity = files->capacity == 0U ? 128U : files->capacity * 2U;
        if (capacity > SC_MAX_FILES) capacity = SC_MAX_FILES;
        char **items = realloc(files->items, capacity * sizeof(*items));
        if (items == NULL) return 2;
        files->items = items; files->capacity = capacity;
    }
    char *copy = ScCopy(path, strlen(path));
    if (copy == NULL) return 2;
    files->items[files->count++] = copy; return 0;
}
static int collectDirectory(const char *root, unsigned kind, ScFiles *files, size_t depth)
{
    if (depth > SC_MAX_DEPTH) return 2;
    int result = 0;
#if defined(_WIN32)
    char *pattern = ScJoin(root, "*");
    if (pattern == NULL) return 2;
    wchar_t *wide = widePath(pattern); free(pattern);
    if (wide == NULL) return 2;
    WIN32_FIND_DATAW data;
    HANDLE search = FindFirstFileW(wide, &data); free(wide);
    if (search == INVALID_HANDLE_VALUE) return GetLastError() == ERROR_FILE_NOT_FOUND ? 0 : 2;
    do {
        char *name = utf8Path(data.cFileName);
        if (name == NULL) { result = 2; break; }
        if (!ignoredDirectory(name) && !(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            char *path = ScJoin(root, name);
            if (path == NULL) result = 2;
            else if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (kind != 2U || depth == 0U)) result = collectDirectory(path, kind, files, depth + 1U);
            else if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (kind != 2U || depth == 1U) && wanted(name, kind)) result = appendFile(files, path);
            free(path);
        }
        free(name);
        if (result != 0) break;
    } while (FindNextFileW(search, &data));
    if (result == 0 && GetLastError() != ERROR_NO_MORE_FILES) result = 2;
    FindClose(search);
#else
    DIR *directory = opendir(root);
    if (directory == NULL) return 2;
    struct dirent *entry;
    for (;;) {
        errno = 0; entry = readdir(directory);
        if (entry == NULL) { if (errno != 0) result = 2; break; }
        if (ignoredDirectory(entry->d_name)) continue;
        char *path = ScJoin(root, entry->d_name);
        if (path == NULL) { result = 2; break; }
        struct stat info;
        if (lstat(path, &info) != 0) result = 2;
        else if (S_ISDIR(info.st_mode) && (kind != 2U || depth == 0U)) result = collectDirectory(path, kind, files, depth + 1U);
        else if (S_ISREG(info.st_mode) && (kind != 2U || depth == 1U) && wanted(entry->d_name, kind)) result = appendFile(files, path);
        free(path);
        if (result != 0) break;
    }
    if (closedir(directory) != 0) result = 2;
#endif
    return result;
}
static int collectRoot(const char *root, unsigned kind, ScFiles *files)
{
    if (root == NULL || root[0] == '\0' || files == NULL) return 2;
    char *absolute = NULL;
#if defined(_WIN32)
    wchar_t *wide = widePath(root);
    if (wide == NULL) return 2;
    DWORD attributes = GetFileAttributesW(wide);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY) || (attributes & FILE_ATTRIBUTE_REPARSE_POINT)) { free(wide); return 2; }
    wchar_t *full = _wfullpath(NULL, wide, 0U); free(wide);
    if (full != NULL) { absolute = utf8Path(full); free(full); }
#else
    struct stat info;
    if (lstat(root, &info) != 0 || !S_ISDIR(info.st_mode)) return 2;
    absolute = realpath(root, NULL);
#endif
    if (absolute == NULL) return 2;
    int result = collectDirectory(absolute, kind, files, 0U);
    free(absolute); return result;
}
void ScFilesFree(ScFiles *files)
{
    if (files == NULL) return;
    for (size_t i = 0U; i < files->count; ++i) free(files->items[i]);
    free(files->items); memset(files, 0, sizeof(*files));
}

int ScCollect(const char *root, bool headers, ScFiles *files)
{ return collectRoot(root, headers ? 1U : 0U, files); }
int ScCollectManifests(const char *root, ScFiles *files)
{ return collectRoot(root, 2U, files); }
