/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_format_persistence.c
 * PURPOSE: Inspect bytes passed to an in-memory provider and keep format history coherent across Save and reload.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include "umicom/document/format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Memory
{
    unsigned char bytes[128];
    size_t length;
    unsigned writes;
    UmiDocumentCoordinator *documents;
    UmiStatus write_status, callback_status;
    int callback;
} Memory;
static UmiStatus Read(void *context, const char *path, unsigned char **out, size_t *bytes)
{
    (void)path;
    Memory *memory = context;
    *out = malloc(memory->length + 1U);
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(*out, memory->bytes, memory->length);
    (*out)[memory->length] = 0;
    *bytes = memory->length;
    return UMI_STATUS_OK;
}
static UmiStatus Stat(void *context, const char *path, UmiDocumentFileInfo *out)
{
    Memory *memory = context;
    *out = (UmiDocumentFileInfo){0};
    (void)snprintf(out->path, sizeof(out->path), "%s", path);
    out->exists = out->regular_file = out->readable = out->writable = 1;
    out->byte_count = memory->length;
    return UMI_STATUS_OK;
}
static UmiStatus Write(void *context, const char *path, const void *bytes, size_t count, int atomic)
{
    (void)path;
    (void)atomic;
    Memory *memory = context;
    if (memory->callback)
    {
        UmiDocumentFormatOptions choice = {UMI_DOCUMENT_ENCODING_UTF16_BE, UMI_DOCUMENT_LINE_ENDING_NONE, 0};
        memory->callback_status = UmiDocumentCoordinatorSetFormat(memory->documents, &choice);
    }
    if (memory->write_status != UMI_STATUS_OK)
        return memory->write_status;
    if (count > sizeof(memory->bytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(memory->bytes, bytes, count);
    memory->length = count;
    ++memory->writes;
    return UMI_STATUS_OK;
}
static void Release(void *context, void *data)
{
    (void)context;
    free(data);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"utf8",
                                            "utf8-bom",
                                            "utf16-le",
                                            "utf16-be",
                                            "crlf",
                                            "cr",
                                            "unicode",
                                            "undo-before-save",
                                            "undo-after-save",
                                            "failed-write",
                                            "busy-callback",
                                            "reload-format",
                                            "reload-encoding-only"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    Memory memory = {0};
    memcpy(memory.bytes, "a\nb\n", 4U);
    memory.length = 4U;
    UmiDocumentProvider provider = {.struct_size = sizeof(UmiDocumentProvider),
                                    .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
                                    .provider_id = "format.memory",
                                    .scheme = "file",
                                    .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_WRITE |
                                             UMI_DOCUMENT_PROVIDER_STAT | UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE,
                                    .instance = &memory,
                                    .read = Read,
                                    .write = Write,
                                    .stat = Stat,
                                    .release_bytes = Release};
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("format.memory", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, &provider, &memory.documents) == UMI_STATUS_OK);
#ifdef _WIN32
    const char *path = "C:/format-memory/source.txt";
#else
    const char *path = "/format-memory/source.txt";
#endif
    char id[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_open(memory.documents, path, id, sizeof(id)) == UMI_STATUS_OK);
    UmiDocumentFormatOptions choice = {UMI_DOCUMENT_ENCODING_UTF8_BOM, UMI_DOCUMENT_LINE_ENDING_NONE, 0};
    const unsigned char *expected = (const unsigned char *)"\xef\xbb\xbf"
                                                           "a\nb\n";
    size_t count = 7U;
    if (strcmp(mode, "utf8") == 0)
    {
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF8;
        expected = (const unsigned char *)"a\nb\n";
        count = 4U;
    }
    if (strcmp(mode, "utf16-le") == 0)
    {
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF16_LE;
        expected = (const unsigned char *)"\xff\xfe"
                                          "a\0\n\0b\0\n\0";
        count = 10U;
    }
    if (strcmp(mode, "utf16-be") == 0)
    {
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF16_BE;
        expected = (const unsigned char *)"\xfe\xff"
                                          "\0a\0\n\0b\0\n";
        count = 10U;
    }
    if (strcmp(mode, "crlf") == 0)
    {
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF8;
        choice.line_ending = UMI_DOCUMENT_LINE_ENDING_CRLF;
        expected = (const unsigned char *)"a\r\nb\r\n";
        count = 6U;
    }
    if (strcmp(mode, "cr") == 0)
    {
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF8;
        choice.line_ending = UMI_DOCUMENT_LINE_ENDING_CR;
        expected = (const unsigned char *)"a\rb\r";
        count = 4U;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
        CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);
        view.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "\xf0\x9f\x98\x80\n", 5U) == UMI_STATUS_OK);
        choice.encoding = UMI_DOCUMENT_ENCODING_UTF16_LE;
        expected = (const unsigned char *)"\xff\xfe\x3d\xd8\x00\xde\x0a\x00";
        count = 8U;
    }
    UmiDocumentWorkingCopySnapshot original;
    CHECK(umi_document_coordinator_active_snapshot(memory.documents, &original) == UMI_STATUS_OK);
    if (strcmp(mode, "reload-format") == 0 || strcmp(mode, "reload-encoding-only") == 0)
    {
        const unsigned char *incoming = (const unsigned char *)"\xef\xbb\xbf"
                                                               "a\nb\n";
        size_t incoming_bytes = 7U;
        if (strcmp(mode, "reload-format") == 0)
        {
            incoming = (const unsigned char *)"\xef\xbb\xbf"
                                              "different\r\n";
            incoming_bytes = 14U;
        }
        memcpy(memory.bytes, incoming, incoming_bytes);
        memory.length = incoming_bytes;
        UmiDocumentReloadPlan *plan = NULL;
        CHECK(UmiDocumentCoordinatorPrepareReload(memory.documents, original.document_id, &plan) ==
              UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyReload(memory.documents, plan, 0) == UMI_STATUS_OK);
        UmiDocumentReloadPlanDestroy(plan);
        UmiDocumentWorkingCopySnapshot loaded;
        CHECK(umi_document_coordinator_active_snapshot(memory.documents, &loaded) == UMI_STATUS_OK &&
              loaded.encoding == UMI_DOCUMENT_ENCODING_UTF8_BOM);
        CHECK(loaded.undo_count == original.undo_count + 1U);
        CHECK(umi_document_coordinator_undo(memory.documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(memory.documents, &loaded) == UMI_STATUS_OK &&
              loaded.encoding == original.encoding && loaded.line_ending == original.line_ending);
        CHECK(umi_document_coordinator_redo(memory.documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(memory.documents, &loaded) == UMI_STATUS_OK &&
              loaded.encoding == UMI_DOCUMENT_ENCODING_UTF8_BOM);
        goto done;
    }
    CHECK(UmiDocumentCoordinatorSetFormat(memory.documents, &choice) == UMI_STATUS_OK && memory.writes == 0U);
    if (strcmp(mode, "undo-before-save") == 0)
    {
        CHECK(umi_document_coordinator_undo(memory.documents) == UMI_STATUS_OK);
        expected = (const unsigned char *)"a\nb\n";
        count = 4U;
    }
    if (strcmp(mode, "failed-write") == 0)
        memory.write_status = UMI_STATUS_IO_ERROR;
    memory.callback = strcmp(mode, "busy-callback") == 0;
    CHECK(umi_document_coordinator_save_active(memory.documents) == memory.write_status);
    if (memory.callback)
        CHECK(memory.callback_status == UMI_STATUS_BUSY);
    UmiDocumentWorkingCopySnapshot after;
    CHECK(umi_document_coordinator_active_snapshot(memory.documents, &after) == UMI_STATUS_OK);
    if (memory.write_status != UMI_STATUS_OK)
    {
        CHECK(memory.writes == 0U && after.dirty && after.encoding == choice.encoding);
        goto done;
    }
    CHECK(memory.writes == 1U && memory.length == count && memcmp(memory.bytes, expected, count) == 0 &&
          !after.dirty);
    if (strcmp(mode, "undo-after-save") == 0)
    {
        CHECK(umi_document_coordinator_undo(memory.documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active(memory.documents) == UMI_STATUS_OK);
        CHECK(memory.writes == 2U && memory.length == 4U && memcmp(memory.bytes, "a\nb\n", 4U) == 0);
    }
done:
    umi_document_coordinator_destroy(memory.documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return 0;
}
