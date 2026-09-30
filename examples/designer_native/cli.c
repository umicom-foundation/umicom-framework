/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/designer_native/cli.c
 * PURPOSE:
 *   Native source preparation. Nothing in this command builds or runs a project.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/designer_native/cli.c
 * Native source preparation. Nothing in this command builds or runs a project.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/designer/native_project.h"
#include "umicom/declarative/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#define INPUT_LIMIT (1024U * 1024U)

/* Read a bounded regular file. A pipe/device must not make source review wait
 * for external input. The path is explicit; no source-root guessing occurs. */
static UmiStatus ReadInput(const char *path, char **outText)
{
    FILE *file = NULL;
    *outText = NULL;
#ifdef _WIN32
    wchar_t wide[UMI_DESIGNER_NATIVE_PATH_LIMIT];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide,
            (int)UMI_DESIGNER_NATIVE_PATH_LIMIT)) return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE handle = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) return UMI_STATUS_IO_ERROR;
    if (GetFileType(handle) != FILE_TYPE_DISK) { CloseHandle(handle); return UMI_STATUS_INVALID_ARGUMENT; }
    int fd = _open_osfhandle((intptr_t)handle, _O_RDONLY | _O_BINARY);
    if (fd < 0) { CloseHandle(handle); return UMI_STATUS_IO_ERROR; }
    file = _fdopen(fd, "rb");
    if (file == NULL) { _close(fd); return UMI_STATUS_IO_ERROR; }
    struct _stat64 info;
    if (_fstat64(fd, &info) != 0 || (info.st_mode & _S_IFMT) != _S_IFREG) {
        fclose(file); return UMI_STATUS_INVALID_ARGUMENT;
    }
#else
    int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) return UMI_STATUS_IO_ERROR;
    struct stat info;
    if (fstat(fd, &info) != 0 || !S_ISREG(info.st_mode)) { close(fd); return UMI_STATUS_INVALID_ARGUMENT; }
    file = fdopen(fd, "rb");
    if (file == NULL) { close(fd); return UMI_STATUS_IO_ERROR; }
#endif
    if (info.st_size < 0 || (uintmax_t)info.st_size > INPUT_LIMIT) {
        fclose(file); return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char *text = malloc(INPUT_LIMIT + 1U);
    if (text == NULL) { fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t count = fread(text, 1U, INPUT_LIMIT, file);
    int extra = fgetc(file);
    UmiStatus status = ferror(file) ? UMI_STATUS_IO_ERROR :
        extra != EOF ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK && memchr(text, '\0', count) != NULL) status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { free(text); return status; }
    text[count] = '\0';
    *outText = text;
    return UMI_STATUS_OK;
}

static void Usage(void)
{
    puts("Umicom native designer project tool\n"
         "  umicom-designer-project --self-test\n"
         "  umicom-designer-project notes <new-absolute-directory>\n"
         "  umicom-designer-project export <input.umiapp> <project_name> <new-absolute-directory>\n"
         "A new directory is required; its parent must already exist.\n"
         "Source only. No build, installation, file deletion, shell or guest is started.");
}

int main(int argc, char **argv)
{
    UmiDeclDocument *document = NULL;
    UmiDesignerNativeProject *project = NULL;
    UmiDesignerNativeProjectSummary summary;
    UmiDesignerNativePublishResult written = {0};
    UmiStatus status;
    char explanation[512] = "";
    const char *destination = NULL, *name = "umicom_notes";
    int selfTest = argc == 2 && strcmp(argv[1], "--self-test") == 0;
    if (argc == 2 && strcmp(argv[1], "--help") == 0) { Usage(); return 0; }
    if (selfTest || (argc == 3 && strcmp(argv[1], "notes") == 0)) {
        status = UmiDesignerNativeNotesDocument(&document);
        if (!selfTest) destination = argv[2];
    } else if (argc == 5 && strcmp(argv[1], "export") == 0) {
        char *text = NULL;
        UmiDeclDiagnosticList *diagnostics = calloc(1U, sizeof *diagnostics);
        status = diagnostics != NULL ? ReadInput(argv[2], &text) : UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK) status = umi_decl_parse_text(text, &document, diagnostics);
        if (status != UMI_STATUS_OK && diagnostics != NULL) {
            for (size_t i = 0U; i < diagnostics->count; ++i)
                fprintf(stderr, "line %zu: %s\n", diagnostics->items[i].line, diagnostics->items[i].message);
        }
        free(text); free(diagnostics);
        name = argv[3]; destination = argv[4];
    } else { Usage(); return 2; }
    if (status == UMI_STATUS_OK)
        status = UmiDesignerNativeProjectCreate(document, name, &project, explanation, sizeof explanation);
    if (status == UMI_STATUS_OK) status = UmiDesignerNativeProjectGetSummary(project, &summary);
    if (status == UMI_STATUS_OK && !selfTest) {
        /* The plan-success message describes memory-only preparation. Do not
         * repeat it after a publishing failure that may have written files. */
        explanation[0] = '\0';
        for (size_t i = 0U; i < summary.fileCount; ++i) {
            UmiDesignerNativeFileView file;
            status = UmiDesignerNativeProjectFile(project, i, &file);
            if (status != UMI_STATUS_OK) break;
            printf("%s: %zu bytes\n", file.path, file.length);
        }
        if (status == UMI_STATUS_OK) status = UmiDesignerNativeProjectPublish(project, destination, &written);
    }
    if (status == UMI_STATUS_OK) {
        puts(selfTest ? "Native project self-test passed. Memory only; no files written." :
            "Source preparation completed in a new directory. No build or application was run.");
    } else {
        fprintf(stderr, "Source preparation failed: %s. %s\n", umi_status_text(status), explanation);
        if (written.directoryCreated)
            fprintf(stderr, "The incomplete directory was retained for review (%zu complete files). Use a different new directory after correcting the error.\n", written.filesWritten);
    }
    UmiDesignerNativeProjectDestroy(project);
    umi_decl_document_destroy(document);
    return status == UMI_STATUS_OK ? 0 : 1;
}
