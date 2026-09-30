/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_workspace/repository.c
 * PURPOSE:
 *   Checkpoints are complete snapshots, chunked through the canonical Data Server. The head
 *   and retiring checkpoint change in the same transaction. No UI, SQLite call or filesystem
 *   deletion belongs in this implementation.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Checkpoints are complete snapshots, chunked through the canonical Data
 * Server. The head and retiring checkpoint change in the same transaction.
 * No UI, SQLite call or filesystem deletion belongs in this implementation.
 *---------------------------------------------------------------------------*/
#include "internal.h"

typedef struct DwRecord { uint64_t bytes; unsigned chunks; char hash[65]; } DwRecord;
static void Detail(UmiDesktopWorkspace *w, const char *text)
{ if (w) (void)snprintf(w->detail, sizeof w->detail, "%s", text); }
static void Key(uint64_t revision, unsigned chunk, char out[100])
{ (void)snprintf(out, 100, "desktop.workspace.g.%020" PRIu64 ".%03u", revision, chunk); }
static void MetaKey(uint64_t revision, char out[100])
{ (void)snprintf(out, 100, "desktop.workspace.g.%020" PRIu64 ".meta", revision); }
static void Head(uint64_t revision, uint64_t oldest, int clean, char out[256])
{ (void)snprintf(out, 256, "UDWH1\nrevision=%" PRIu64 "\noldest=%" PRIu64 "\nclean=%d\n", revision, oldest, clean); }
static int FieldNumber(const char **cursor, const char *key, uint64_t *out)
{
    size_t keyLength = strlen(key);
    if (strncmp(*cursor, key, keyLength)) return 0;
    const char *p = *cursor + keyLength;
    if (*p < '0' || *p > '9' || (*p == '0' && p[1] != '\n')) return 0;
    uint64_t value = 0;
    while (*p >= '0' && *p <= '9') {
        unsigned digit = (unsigned)(*p++ - '0');
        if (value > (UINT64_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
    }
    if (*p++ != '\n') return 0;
    *cursor = p; *out = value; return 1;
}
static UmiStatus ParseHead(const char *text, uint64_t *revision, uint64_t *oldest, int *clean)
{
    if (strncmp(text, "UDWH1\n", 6U)) return UMI_STATUS_PARSE_ERROR;
    const char *p = text + 6U; uint64_t completed;
    if (!FieldNumber(&p, "revision=", revision) || !FieldNumber(&p, "oldest=", oldest) ||
        !FieldNumber(&p, "clean=", &completed) || *p || completed > 1U ||
        !*revision || !*oldest || *oldest > *revision ||
        *revision - *oldest >= UMI_DESKTOP_WORKSPACE_HISTORY) return UMI_STATUS_PARSE_ERROR;
    *clean = (int)completed; return UMI_STATUS_OK;
}
static void RecordText(const DwRecord *record, char text[160])
{ (void)snprintf(text, 160, "UDWG1\nbytes=%" PRIu64 "\nchunks=%u\nsha256=%s\n", record->bytes, record->chunks, record->hash); }
static UmiStatus RecordRead(UmiDesktopWorkspace *w, uint64_t revision, DwRecord *record)
{
    char key[100], text[160];
    MetaKey(revision, key);
    UmiStatus status = umi_data_server_get(w->server, key, text, sizeof text);
    if (status != UMI_STATUS_OK) return status;
    memset(record, 0, sizeof *record);
    if (strncmp(text, "UDWG1\n", 6U)) return UMI_STATUS_PARSE_ERROR;
    const char *p = text + 6U; uint64_t chunks;
    if (!FieldNumber(&p, "bytes=", &record->bytes) || !FieldNumber(&p, "chunks=", &chunks) ||
        !record->bytes || record->bytes > DW_MAX_BYTES ||
        chunks != (record->bytes + DW_CHUNK - 1U) / DW_CHUNK || strncmp(p, "sha256=", 7U))
        return UMI_STATUS_PARSE_ERROR;
    p += 7U;
    if (strlen(p) != 65U || p[64] != '\n') return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0; i < 64U; ++i) {
        if (!((p[i] >= '0' && p[i] <= '9') || (p[i] >= 'a' && p[i] <= 'f'))) return UMI_STATUS_PARSE_ERROR;
        record->hash[i] = p[i];
    }
    record->chunks = (unsigned)chunks;
    return UMI_STATUS_OK;
}
static void Hex(const unsigned char *data, size_t size, char *out)
{
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0; i < size; ++i) { out[i*2U] = digits[data[i] >> 4]; out[i*2U+1U] = digits[data[i] & 15U]; }
    out[size * 2U] = 0;
}
static int Digit(char c)
{ if (c >= '0' && c <= '9') return c - '0'; if (c >= 'a' && c <= 'f') return c - 'a' + 10; return -1; }
static UmiStatus ReadGeneration(UmiDesktopWorkspace *w, uint64_t revision, UmiDesktopWorkspaceSnapshot *out)
{
    DwRecord record; UmiStatus status = RecordRead(w, revision, &record);
    if (status != UMI_STATUS_OK) return status;
    unsigned char *data = malloc((size_t)record.bytes);
    if (!data) return UMI_STATUS_OUT_OF_MEMORY;
    size_t offset = 0;
    for (unsigned i = 0; i < record.chunks; ++i) {
        char key[100], text[DW_CHUNK*2U+1U]; Key(revision, i, key);
        status = umi_data_server_get(w->server, key, text, sizeof text);
        if (status != UMI_STATUS_OK) break;
        size_t size = (size_t)record.bytes - offset;
        if (size > DW_CHUNK) size = DW_CHUNK;
        if (strlen(text) != size * 2U) { status = UMI_STATUS_PARSE_ERROR; break; }
        for (size_t j = 0; j < size; ++j) {
            int a = Digit(text[j*2U]), b = Digit(text[j*2U+1U]);
            if (a < 0 || b < 0) { status = UMI_STATUS_PARSE_ERROR; break; }
            data[offset+j] = (unsigned char)((a << 4) | b);
        }
        if (status != UMI_STATUS_OK) break;
        offset += size;
    }
    if (status == UMI_STATUS_OK) {
        char hash[65]; status = UmiNativeSha256Buffer(data, offset, hash);
        if (status == UMI_STATUS_OK && strcmp(hash, record.hash)) status = UMI_STATUS_PARSE_ERROR;
        UmiDesktopWorkspaceSnapshot *checked = malloc(sizeof *checked);
        if (!checked) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK) status = DwDecode(data, offset, checked);
        if (status == UMI_STATUS_OK && checked->revision != revision) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) *out = *checked;
        free(checked);
    }
    free(data); return status;
}
static UmiStatus Rollback(UmiDesktopWorkspace *w, UmiStatus original)
{
    if (umi_data_server_rollback(w->server) != UMI_STATUS_OK) {
        w->poisoned = 1;
        Detail(w, "Rollback failed. Close this workspace and inspect its saved state before reopening.");
        return UMI_STATUS_IO_ERROR;
    }
    Detail(w, "The change was not committed. The previous saved checkpoint is unchanged.");
    return original;
}
static UmiStatus CheckHead(UmiDesktopWorkspace *w)
{
    char current[256];
    UmiStatus status = umi_data_server_get(w->server, DW_HEAD, current, sizeof current);
    if (status == UMI_STATUS_NOT_FOUND && !w->head[0]) return UMI_STATUS_OK;
    if (status == UMI_STATUS_OK && strcmp(current, w->head)) status = UMI_STATUS_INVALID_STATE;
    return status;
}
static UmiStatus WriteGeneration(UmiDesktopWorkspace *w, const UmiDesktopWorkspaceSnapshot *s)
{
    unsigned char *data = NULL; size_t size = 0;
    UmiStatus status = DwEncode(s, &data, &size);
    if (status != UMI_STATUS_OK) return status;
    DwRecord record = { .bytes = size, .chunks = (unsigned)((size + DW_CHUNK - 1U)/DW_CHUNK) };
    status = UmiNativeSha256Buffer(data, size, record.hash);
    for (unsigned i = 0; status == UMI_STATUS_OK && i < record.chunks; ++i) {
        size_t offset = (size_t)i * DW_CHUNK, count = size - offset;
        if (count > DW_CHUNK) count = DW_CHUNK;
        char key[100], text[DW_CHUNK*2U+1U]; Key(s->revision, i, key); Hex(data+offset, count, text);
        status = umi_data_server_set(w->server, key, text);
    }
    if (status == UMI_STATUS_OK) {
        char key[100], text[160]; MetaKey(s->revision, key); RecordText(&record, text);
        status = umi_data_server_set(w->server, key, text);
    }
    free(data); return status;
}
static UmiStatus RetireGeneration(UmiDesktopWorkspace *w, uint64_t revision)
{
    DwRecord record; UmiStatus status = RecordRead(w, revision, &record);
    if (status != UMI_STATUS_OK) return status;
    for (unsigned i = 0; i < record.chunks; ++i) {
        char key[100]; Key(revision, i, key); status = umi_data_server_delete(w->server, key);
        if (status != UMI_STATUS_OK) return status;
    }
    char key[100]; MetaKey(revision, key);
    return umi_data_server_delete(w->server, key);
}

UmiStatus UmiDesktopWorkspaceCommit(UmiDesktopWorkspace *w, uint64_t expected,
    const UmiDesktopWorkspaceSnapshot *draft)
{
    if (!w || !draft) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->closed || w->poisoned || expected != w->snapshot.revision || draft->revision != expected)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiDesktopWorkspaceValidate(draft);
    if (status != UMI_STATUS_OK) return status;
    if (expected == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDesktopWorkspaceSnapshot *next = malloc(sizeof *next);
    if (!next) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *draft; next->revision = expected + 1U;
    uint64_t oldest = w->oldest ? w->oldest : 1U;
    int retire = next->revision - oldest >= UMI_DESKTOP_WORKSPACE_HISTORY;
    char head[256]; Head(next->revision, oldest + (unsigned)retire, 0, head);
    status = umi_data_server_begin(w->server);
    if (status != UMI_STATUS_OK) { free(next); return status; }
    status = CheckHead(w);
    if (status == UMI_STATUS_OK) status = WriteGeneration(w, next);
    if (status == UMI_STATUS_OK && retire) status = RetireGeneration(w, oldest);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(w->server, DW_HEAD, head);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(w->server);
    if (status != UMI_STATUS_OK) status = Rollback(w, status);
    else {
        w->snapshot = *next; w->oldest = oldest + (unsigned)retire; strcpy(w->head, head);
        Detail(w, "Checkpoint saved. Notes and preferences were committed together.");
    }
    free(next); return status;
}

UmiStatus UmiDesktopWorkspaceOpenServer(UmiDataServer *server, UmiDesktopWorkspace **out)
{
    if (!server || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (umi_data_server_in_transaction(server)) return UMI_STATUS_BUSY;
    UmiDesktopWorkspace *w = calloc(1, sizeof *w);
    if (!w) return UMI_STATUS_OUT_OF_MEMORY;
    w->server = server; UmiDesktopWorkspaceSnapshotInit(&w->snapshot);
    UmiStatus status = umi_data_server_get(server, DW_HEAD, w->head, sizeof w->head);
    if (status == UMI_STATUS_NOT_FOUND) {
        /* A dedicated empty store is required. Do not adopt unrelated records
         * or overwrite an incomplete/corrupt namespace as an empty notebook. */
        if (umi_data_server_count(server)) status = UMI_STATUS_INVALID_STATE;
        else status = UmiDesktopWorkspaceCommit(w, 0, &w->snapshot);
    } else if (status == UMI_STATUS_OK) {
        uint64_t revision = 0; int clean = 0;
        status = ParseHead(w->head, &revision, &w->oldest, &clean);
        if (status == UMI_STATUS_OK) status = ReadGeneration(w, revision, &w->snapshot);
        if (status == UMI_STATUS_OK) {
            w->unfinished = !clean;
            status = umi_data_server_begin(server);
            if (status == UMI_STATUS_OK) {
                status = CheckHead(w);
                char active[256]; Head(revision, w->oldest, 0, active);
                if (status == UMI_STATUS_OK) status = umi_data_server_set(server, DW_HEAD, active);
                if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
                if (status != UMI_STATUS_OK) status = Rollback(w, status);
                else strcpy(w->head, active);
            }
        }
    }
    if (status != UMI_STATUS_OK) { free(w); return status; }
    Detail(w, w->unfinished ? "The previous session was not closed cleanly. The last committed checkpoint was loaded; unsaved edits are not recoverable."
        : "Workspace opened. Editing creates a draft until Save checkpoint is requested.");
    *out = w; return UMI_STATUS_OK;
}
UmiStatus UmiDesktopWorkspaceOpenMemory(UmiDesktopWorkspace **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL; UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceOpenServer(server, out);
    if (status == UMI_STATUS_OK) (*out)->ownsServer = 1;
    else umi_data_server_destroy(server);
    return status;
}
UmiStatus UmiDesktopWorkspaceOpenDirectory(const char *directory, UmiDesktopWorkspace **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL; char path[1024]; DwGuard *guard = NULL; UmiDataServer *server = NULL;
    UmiStatus status = DwGuardAcquire(directory, &guard, path);
    if (status == UMI_STATUS_OK) status = umi_data_server_create_sqlite(path, &server);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceOpenServer(server, out);
    if (status == UMI_STATUS_OK) { (*out)->ownsServer = 1; (*out)->guard = guard; }
    else { umi_data_server_destroy(server); DwGuardRelease(guard); }
    return status;
}
UmiStatus UmiDesktopWorkspaceRead(const UmiDesktopWorkspace *w, UmiDesktopWorkspaceSnapshot *out)
{
    if (!w || !out) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->poisoned) return UMI_STATUS_INVALID_STATE;
    *out = w->snapshot; return UMI_STATUS_OK;
}
uint64_t UmiDesktopWorkspaceOldestRevision(const UmiDesktopWorkspace *w) { return w ? w->oldest : 0; }
int UmiDesktopWorkspacePreviousSessionUnfinished(const UmiDesktopWorkspace *w) { return w ? w->unfinished : 0; }
const char *UmiDesktopWorkspaceDetail(const UmiDesktopWorkspace *w) { return w ? w->detail : "No workspace is open."; }
UmiStatus UmiDesktopWorkspaceReadCheckpoint(UmiDesktopWorkspace *w, uint64_t revision, UmiDesktopWorkspaceSnapshot *out)
{
    if (!w || !out) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->poisoned) return UMI_STATUS_INVALID_STATE;
    if (revision < w->oldest || revision > w->snapshot.revision) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = CheckHead(w);
    if (status != UMI_STATUS_OK) return status;
    return ReadGeneration(w, revision, out);
}
UmiStatus UmiDesktopWorkspaceRestore(UmiDesktopWorkspace *w, uint64_t expected, uint64_t revision)
{
    if (!w) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected != w->snapshot.revision) return UMI_STATUS_INVALID_STATE;
    UmiDesktopWorkspaceSnapshot *snapshot = malloc(sizeof *snapshot);
    if (!snapshot) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDesktopWorkspaceReadCheckpoint(w, revision, snapshot);
    if (status == UMI_STATUS_OK) { snapshot->revision = expected; status = UmiDesktopWorkspaceCommit(w, expected, snapshot); }
    free(snapshot); return status;
}
UmiStatus UmiDesktopWorkspaceCloseClean(UmiDesktopWorkspace *w)
{
    if (!w) return UMI_STATUS_INVALID_ARGUMENT;
    if (w->poisoned) return UMI_STATUS_INVALID_STATE;
    if (w->closed) return UMI_STATUS_OK;
    UmiStatus status = umi_data_server_begin(w->server);
    if (status != UMI_STATUS_OK) return status;
    status = CheckHead(w);
    char clean[256]; Head(w->snapshot.revision, w->oldest, 1, clean);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(w->server, DW_HEAD, clean);
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(w->server);
    if (status != UMI_STATUS_OK) return Rollback(w, status);
    strcpy(w->head, clean); w->closed = 1;
    Detail(w, "Session closed cleanly. All committed checkpoints remain saved.");
    return UMI_STATUS_OK;
}
void UmiDesktopWorkspaceDestroy(UmiDesktopWorkspace *w)
{
    if (!w) return;
    if (w->ownsServer) umi_data_server_destroy(w->server);
    DwGuardRelease(w->guard);
    free(w);
}
