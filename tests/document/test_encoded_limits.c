/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_encoded_limits.c
 * PURPOSE: Keep supported large Unicode files reopenable while refusing oversized decoded drafts and serialized input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include "umicom/document/source_navigation.h"
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
    unsigned char *data;
    size_t bytes;
    unsigned reads, writes;
} Memory;
static UmiStatus Read(void *context, const char *path, unsigned char **out, size_t *size)
{
    (void)path;
    Memory *m = context;
    ++m->reads;
    if (m->data == NULL)
        return UMI_STATUS_INVALID_STATE;
    *out = malloc(m->bytes);
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(*out, m->data, m->bytes);
    *size = m->bytes;
    return UMI_STATUS_OK;
}
static UmiStatus Stat(void *context, const char *path, UmiDocumentFileInfo *out)
{
    Memory *m = context;
    *out = (UmiDocumentFileInfo){0};
    (void)snprintf(out->path, sizeof(out->path), "%s", path);
    out->exists = out->regular_file = out->readable = out->writable = 1;
    out->byte_count = m->bytes;
    return UMI_STATUS_OK;
}
static UmiStatus Write(void *context, const char *path, const void *data, size_t bytes, int atomic)
{
    (void)path;
    (void)atomic;
    Memory *m = context;
    unsigned char *copy = malloc(bytes);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, data, bytes);
    free(m->data);
    m->data = copy;
    m->bytes = bytes;
    ++m->writes;
    return UMI_STATUS_OK;
}
static void Release(void *context, void *bytes)
{
    (void)context;
    free(bytes);
}
/* Build known on-disk bytes independently of the production encoder. The
 * CRLF fixture expands each two-byte "x\n" draft pair into six UTF-16 bytes. */
static int Input(Memory *m, size_t characters, int bom_utf8, int big_endian, int crlf)
{
    size_t prefix = bom_utf8 ? 3U : 2U;
    m->bytes = prefix + characters * (bom_utf8 ? 1U : 2U) + (crlf ? characters : 0U);
    m->data = malloc(m->bytes);
    CHECK(m->data != NULL);
    if (bom_utf8)
    {
        memcpy(m->data, "\xef\xbb\xbf", 3U);
        memset(m->data + 3U, 'x', characters);
        return 0;
    }
    m->data[0] = big_endian ? 0xfeU : 0xffU;
    m->data[1] = big_endian ? 0xffU : 0xfeU;
    size_t at = 2U;
    for (size_t i = 0U; i < characters; ++i)
    {
        unsigned char value = crlf && (i % 2U) != 0U ? '\n' : 'x';
        if (value == '\n')
        {
            m->data[at++] = big_endian ? 0U : '\r';
            m->data[at++] = big_endian ? '\r' : 0U;
        }
        m->data[at++] = big_endian ? 0U : value;
        m->data[at++] = big_endian ? value : 0U;
    }
    CHECK(at == m->bytes);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1],
               *cases[] = {"utf8-bom-limit",   "utf16-le", "utf16-be", "utf16-crlf",  "decoded-limit",
                           "serialized-limit", "reload",   "external", "source-range"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    Memory m = {0};
    size_t characters = 5U * 1024U * 1024U;
    int bom_utf8 = strcmp(mode, "utf8-bom-limit") == 0;
    int crlf = strcmp(mode, "utf16-crlf") == 0, big = strcmp(mode, "utf16-be") == 0;
    if (bom_utf8 || crlf)
        characters = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
    if (strcmp(mode, "decoded-limit") == 0)
        characters = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U;
    if (strcmp(mode, "serialized-limit") == 0)
        m.bytes = UMI_DOCUMENT_COORDINATOR_MAXIMUM_FILE_BYTES + 1U;
    else
        CHECK(Input(&m, characters, bom_utf8, big, crlf) == 0);
    CHECK(m.bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDocumentProvider provider = {.struct_size = sizeof(UmiDocumentProvider),
                                    .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
                                    .provider_id = "encoded.memory",
                                    .scheme = "file",
                                    .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_WRITE |
                                             UMI_DOCUMENT_PROVIDER_STAT | UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE,
                                    .instance = &m,
                                    .read = Read,
                                    .write = Write,
                                    .stat = Stat,
                                    .release_bytes = Release};
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("encoded.limits", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
#ifdef _WIN32
    const char *path = "C:/encoded-memory/source.txt";
#else
    const char *path = "/encoded-memory/source.txt";
#endif
    char view_id[UMI_UI_ID_CAPACITY];
    UmiStatus opened;
    if (strcmp(mode, "source-range") == 0)
    {
        char uri[UMI_DOCUMENT_URI_CAPACITY];
        CHECK(umi_document_uri_from_path(path, uri, sizeof(uri)) == UMI_STATUS_OK);
        UmiDocumentId id = 0U;
        size_t offset = 99U, selected = 0U;
        opened =
            UmiDocumentCoordinatorOpenSourceRange(documents, uri, (UmiEditorTextPosition){0U, 0U},
                                                  (UmiEditorTextPosition){0U, 1U}, &id, &offset, &selected);
        CHECK(opened == UMI_STATUS_OK && id != 0U && offset == 0U && selected == 1U);
        UmiDocumentWorkingCopySnapshot current;
        CHECK(umi_document_coordinator_active_snapshot(documents, &current) == UMI_STATUS_OK);
        strcpy(view_id, current.view_id);
    }
    else
        opened = umi_document_coordinator_open(documents, path, view_id, sizeof(view_id));
    if (strcmp(mode, "decoded-limit") == 0 || strcmp(mode, "serialized-limit") == 0)
    {
        CHECK(opened == UMI_STATUS_CAPACITY_EXCEEDED && umi_document_coordinator_count(documents) == 0U &&
              umi_document_store_count(store) == 0U);
        if (strcmp(mode, "serialized-limit") == 0)
            CHECK(m.reads == 0U);
        else
            CHECK(m.reads != 0U);
        goto done;
    }
    CHECK(opened == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(documents, &snapshot) == UMI_STATUS_OK &&
          snapshot.text_length == characters);
    if (strcmp(mode, "external") == 0 || strcmp(mode, "reload") == 0)
    {
        int changed = 1;
        CHECK(umi_document_coordinator_check_external_change(documents, &changed) == UMI_STATUS_OK &&
              !changed);
        m.data[2] = 'y';
        CHECK(umi_document_coordinator_check_external_change(documents, &changed) == UMI_STATUS_OK &&
              changed);
        if (strcmp(mode, "reload") == 0)
        {
            UmiDocumentReloadPlan *plan = NULL;
            CHECK(UmiDocumentCoordinatorPrepareReload(documents, snapshot.document_id, &plan) ==
                  UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorApplyReload(documents, plan, 0) == UMI_STATUS_OK);
            UmiDocumentReloadPlanDestroy(plan);
        }
        goto done;
    }
    size_t old_bytes = m.bytes;
    CHECK(umi_document_coordinator_save_active(documents) == UMI_STATUS_OK && m.writes == 1U &&
          m.bytes == old_bytes);
    CHECK(umi_document_coordinator_close_active(documents, 0) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(documents, path, view_id, sizeof(view_id)) == UMI_STATUS_OK);
    char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(workbench), view_id, &text, &bytes) ==
              UMI_STATUS_OK &&
          bytes == characters);
    CHECK(text[0] == 'x' && text[bytes - 1U] == (crlf ? '\n' : 'x'));
    UmiUiDocumentViewModelFreeText(text);
done:
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    free(m.data);
    return 0;
}
