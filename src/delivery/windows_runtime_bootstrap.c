/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/windows_runtime_bootstrap.c
 *
 * PURPOSE:
 *   Locate a private Windows runtime relative to the executable, not the
 *   developer's source checkout or working directory. All environment changes
 *   belong to this process only. Startup DLLs are deployed by CMake before this
 *   code can run; a constructor cannot repair a missing load-time DLL.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "umicom/delivery/windows_runtime_cache.h"

#define UMI_RUNTIME_PATH_CAPACITY 32768U
#define UMI_RUNTIME_CACHE_LIMIT (4U * 1024U * 1024U)

/* Each process owns its temporary cache; concurrent application starts cannot
 * overwrite one another. The installed directory is never written at startup. */
static wchar_t umiRuntimeCachePath[MAX_PATH + 1U];

static void UmiWindowsRuntimeRemoveCache(void)
{
    if (umiRuntimeCachePath[0] != L'\0') {
        (void)DeleteFileW(umiRuntimeCachePath);
        umiRuntimeCachePath[0] = L'\0';
    }
}

/* Report the real resource failure instead of falling back to MSYS2 paths. */
_Noreturn static void UmiWindowsRuntimeFail(const wchar_t *detail)
{
    (void)MessageBoxW(NULL, detail, L"Umicom runtime could not be prepared",
                     MB_OK | MB_ICONERROR);
    UmiWindowsRuntimeRemoveCache();
    exit(EXIT_FAILURE);
}

static int UmiWindowsRuntimeJoin(wchar_t *output, size_t capacity,
                                 const wchar_t *root, const wchar_t *relative)
{
    const size_t rootSize = wcslen(root);
    const size_t relativeSize = wcslen(relative);
    if (rootSize >= capacity || relativeSize >= capacity - rootSize ||
        capacity - rootSize - relativeSize < 2U) {
        return 0;
    }
    (void)wmemcpy(output, root, rootSize);
    output[rootSize] = L'\\';
    (void)wmemcpy(output + rootSize + 1U, relative, relativeSize + 1U);
    return 1;
}

static int UmiWindowsRuntimeExists(const wchar_t *path)
{
    return GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

/* _wputenv_s updates the C runtime's getenv view as well as the Win32 process
 * environment. SetEnvironmentVariableW alone would miss existing C callers. */
static void UmiWindowsRuntimeSet(const wchar_t *name, const wchar_t *root,
                                 const wchar_t *relative, int requireExisting)
{
    wchar_t *path = malloc(UMI_RUNTIME_PATH_CAPACITY * sizeof(*path));
    if (path == NULL ||
        !UmiWindowsRuntimeJoin(path, UMI_RUNTIME_PATH_CAPACITY, root, relative)) {
        free(path);
        UmiWindowsRuntimeFail(L"The runtime resource path is too long or memory is unavailable.");
    }
    if ((!requireExisting || UmiWindowsRuntimeExists(path)) &&
        _wputenv_s(name, path) != 0) {
        free(path);
        UmiWindowsRuntimeFail(L"The application could not set its private resource environment.");
    }
    free(path);
}

static void UmiWindowsRuntimePrepareCache(const wchar_t *root)
{
    wchar_t *path = malloc(UMI_RUNTIME_PATH_CAPACITY * sizeof(*path));
    wchar_t tempDirectory[MAX_PATH + 1U];
    char *templateText = NULL;
    char *rootUtf8 = NULL;
    char *expanded = NULL;
    FILE *input = NULL;
    FILE *output = NULL;
    size_t required = 0U;
    long fileSize;
    int rootSize;
    int succeeded = 0;
    DWORD tempLength;

    if (path == NULL || !UmiWindowsRuntimeJoin(path, UMI_RUNTIME_PATH_CAPACITY,
            root, L"share\\umicom\\runtime\\gdk-pixbuf-loaders.cache.in")) {
        goto done;
    }
    if (!UmiWindowsRuntimeExists(path)) {
        /* A build without external GdkPixbuf loaders has no cache to relocate. */
        succeeded = 1;
        goto done;
    }
    input = _wfopen(path, L"rb");
    if (input == NULL || fseek(input, 0L, SEEK_END) != 0) {
        goto done;
    }
    fileSize = ftell(input);
    if (fileSize < 0L || (unsigned long)fileSize > UMI_RUNTIME_CACHE_LIMIT ||
        fseek(input, 0L, SEEK_SET) != 0) {
        goto done;
    }
    templateText = malloc((size_t)fileSize + 1U);
    if (templateText == NULL ||
        fread(templateText, 1U, (size_t)fileSize, input) != (size_t)fileSize) {
        goto done;
    }
    templateText[(size_t)fileSize] = '\0';
    rootSize = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, root, -1,
                                   NULL, 0, NULL, NULL);
    if (rootSize <= 0) {
        goto done;
    }
    rootUtf8 = malloc((size_t)rootSize);
    if (rootUtf8 == NULL || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
            root, -1, rootUtf8, rootSize, NULL, NULL) != rootSize ||
        UmiWindowsRuntimeExpandCache(templateText, (size_t)fileSize, rootUtf8,
            NULL, 0U, &required) != UMI_WINDOWS_RUNTIME_CACHE_OK) {
        goto done;
    }
    expanded = malloc(required);
    if (expanded == NULL || UmiWindowsRuntimeExpandCache(templateText,
            (size_t)fileSize, rootUtf8, expanded, required, &required) !=
            UMI_WINDOWS_RUNTIME_CACHE_OK) {
        goto done;
    }
    tempLength = GetTempPathW((DWORD)(MAX_PATH + 1U), tempDirectory);
    if (tempLength == 0U || tempLength > MAX_PATH ||
        GetTempFileNameW(tempDirectory, L"umi", 0U, umiRuntimeCachePath) == 0U) {
        goto done;
    }
    output = _wfopen(umiRuntimeCachePath, L"wb");
    if (output == NULL || fwrite(expanded, 1U, required - 1U, output) != required - 1U) {
        goto done;
    }
    if (fclose(output) != 0) {
        output = NULL;
        goto done;
    }
    output = NULL;
    if (_wputenv_s(L"GDK_PIXBUF_MODULE_FILE", umiRuntimeCachePath) != 0 ||
        atexit(UmiWindowsRuntimeRemoveCache) != 0) {
        goto done;
    }
    succeeded = 1;

done:
    if (input != NULL) {
        (void)fclose(input);
    }
    if (output != NULL) {
        (void)fclose(output);
    }
    free(expanded);
    free(rootUtf8);
    free(templateText);
    free(path);
    if (!succeeded) {
        UmiWindowsRuntimeFail(L"The private image-loader cache could not be created. Check that the application package is complete and your temporary folder is writable.");
    }
}

/* Compiled directly into each executable, not placed in a static archive where
 * an otherwise unreferenced constructor could be discarded by the linker. */
static void UmiWindowsRuntimeInitialise(void)
{
    wchar_t *root = calloc(UMI_RUNTIME_PATH_CAPACITY, sizeof(*root));
    wchar_t *marker = malloc(UMI_RUNTIME_PATH_CAPACITY * sizeof(*marker));
    wchar_t *separator;
    DWORD length;
    if (root == NULL || marker == NULL) {
        free(root);
        free(marker);
        UmiWindowsRuntimeFail(L"There is not enough memory to locate the Umicom runtime.");
    }
    length = GetModuleFileNameW(NULL, root, UMI_RUNTIME_PATH_CAPACITY);
    if (length == 0U || length >= UMI_RUNTIME_PATH_CAPACITY) {
        free(root);
        free(marker);
        UmiWindowsRuntimeFail(L"The executable location could not be resolved.");
    }
    separator = wcsrchr(root, L'\\');
    if (separator == NULL) {
        free(root);
        free(marker);
        UmiWindowsRuntimeFail(L"The executable location is not an absolute Windows path.");
    }
    *separator = L'\0';
    /* Installed layout is root/bin. Standalone targets can use root directly. */
    separator = wcsrchr(root, L'\\');
    if (separator != NULL && _wcsicmp(separator + 1, L"bin") == 0) {
        *separator = L'\0';
    }
    if (!UmiWindowsRuntimeJoin(marker, UMI_RUNTIME_PATH_CAPACITY, root,
                               L"share\\umicom\\runtime\\deployment.marker") ||
        !UmiWindowsRuntimeExists(marker)) {
        free(root);
        free(marker);
        UmiWindowsRuntimeFail(L"The Umicom runtime has not been deployed beside this application. Rebuild the umicom-windows-runtime target or reinstall the complete application package.");
    }
    UmiWindowsRuntimeSet(L"GTK_DATA_PREFIX", root, L".", 1);
    UmiWindowsRuntimeSet(L"GTK_EXE_PREFIX", root, L".", 1);
    UmiWindowsRuntimeSet(L"GTK_PATH", root, L"lib\\gtk-4.0", 0);
    UmiWindowsRuntimeSet(L"XDG_DATA_DIRS", root, L"share", 1);
    UmiWindowsRuntimeSet(L"GSETTINGS_SCHEMA_DIR", root, L"share\\glib-2.0\\schemas", 1);
    UmiWindowsRuntimeSet(L"GIO_MODULE_DIR", root, L"lib\\gio\\modules", 0);
    UmiWindowsRuntimeSet(L"GIO_EXTRA_MODULES", root, L"lib\\gio\\modules", 0);
    UmiWindowsRuntimeSet(L"FONTCONFIG_PATH", root, L"etc\\fonts", 1);
    UmiWindowsRuntimeSet(L"FONTCONFIG_FILE", root, L"etc\\fonts\\fonts.conf", 1);
    UmiWindowsRuntimeSet(L"UMICOM_FRAMEWORK_RESOURCE_ROOT", root,
                         L"share\\umicom\\framework\\resources", 1);
    UmiWindowsRuntimePrepareCache(root);
    free(marker);
    free(root);
}

#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
__declspec(allocate(".CRT$XCU")) void (__cdecl *umiWindowsRuntimeInitialiser)(void) =
    UmiWindowsRuntimeInitialise;
#if defined(_M_IX86)
#pragma comment(linker, "/include:_umiWindowsRuntimeInitialiser")
#else
#pragma comment(linker, "/include:umiWindowsRuntimeInitialiser")
#endif
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void UmiWindowsRuntimeConstructor(void)
{
    UmiWindowsRuntimeInitialise();
}
#else
#error "Windows runtime startup requires a supported constructor mechanism."
#endif
#endif
