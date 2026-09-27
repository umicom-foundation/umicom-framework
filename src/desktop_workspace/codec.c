/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Canonical little-endian records with explicit lengths. This is data, not
 * executable configuration: no path, command, URI or environment is restored.
 *---------------------------------------------------------------------------*/
#include "internal.h"
typedef struct DwBytes { unsigned char *data; size_t size, at; int valid; } DwBytes;
static void Put(DwBytes *b, uint64_t value, unsigned bytes)
{
    if (bytes > b->size - b->at) { b->valid = 0; return; }
    for (unsigned i = 0; i < bytes; ++i) { b->data[b->at++] = (unsigned char)(value & 255U); value >>= 8; }
}
static uint64_t Get(DwBytes *b, unsigned bytes)
{
    if (bytes > b->size - b->at) { b->valid = 0; return 0; }
    uint64_t value = 0;
    for (unsigned i = 0; i < bytes; ++i) value |= (uint64_t)b->data[b->at++] << (i * 8U);
    return value;
}
static void PutText(DwBytes *b, const char *text)
{
    size_t n = strlen(text);
    Put(b, n, 2);
    if (!b->valid || n > b->size - b->at) { b->valid = 0; return; }
    memcpy(b->data + b->at, text, n); b->at += n;
}
static void GetText(DwBytes *b, char *out, size_t capacity)
{
    size_t n = (size_t)Get(b, 2);
    if (!b->valid || n >= capacity || n > b->size - b->at || memchr(b->data + b->at, 0, n)) {
        b->valid = 0; return;
    }
    memcpy(out, b->data + b->at, n); out[n] = 0; b->at += n;
}
UmiStatus DwEncode(const UmiDesktopWorkspaceSnapshot *s, unsigned char **out, size_t *length)
{
    if (!out || !length) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL; *length = 0;
    if (UmiDesktopWorkspaceValidate(s) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *data = calloc(1, DW_MAX_BYTES);
    if (!data) return UMI_STATUS_OUT_OF_MEMORY;
    DwBytes b = { data, DW_MAX_BYTES, 0, 1 };
    Put(&b, UINT32_C(0x31574455), 4); /* UDW1 */
    Put(&b, s->revision, 8); Put(&b, (unsigned)s->theme, 1);
    Put(&b, s->fontPoints, 1); Put(&b, (unsigned)s->sidebarVisible, 1);
    Put(&b, s->noteCount, 1); PutText(&b, s->selectedNote);
    for (size_t i = 0; i < s->noteCount; ++i) {
        PutText(&b, s->notes[i].id); PutText(&b, s->notes[i].title); PutText(&b, s->notes[i].body);
    }
    if (!b.valid) { free(data); return UMI_STATUS_CAPACITY_EXCEEDED; }
    *out = data; *length = b.at; return UMI_STATUS_OK;
}
UmiStatus DwDecode(const void *data, size_t length, UmiDesktopWorkspaceSnapshot *out)
{
    if (!data || !out || length > DW_MAX_BYTES) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDesktopWorkspaceSnapshot *s = calloc(1, sizeof *s);
    if (!s) return UMI_STATUS_OUT_OF_MEMORY;
    DwBytes b = { (unsigned char *)(uintptr_t)data, length, 0, 1 };
    int magic = Get(&b, 4) == UINT32_C(0x31574455);
    s->revision = Get(&b, 8); s->theme = (UmiDesktopWorkspaceTheme)Get(&b, 1);
    s->fontPoints = (unsigned)Get(&b, 1); s->sidebarVisible = (int)Get(&b, 1);
    s->noteCount = (size_t)Get(&b, 1); GetText(&b, s->selectedNote, sizeof s->selectedNote);
    if (s->noteCount > UMI_DESKTOP_WORKSPACE_NOTES) b.valid = 0;
    for (size_t i = 0; b.valid && i < s->noteCount; ++i) {
        GetText(&b, s->notes[i].id, sizeof s->notes[i].id);
        GetText(&b, s->notes[i].title, sizeof s->notes[i].title);
        GetText(&b, s->notes[i].body, sizeof s->notes[i].body);
    }
    UmiStatus status = magic && b.valid && b.at == b.size && s->revision &&
        UmiDesktopWorkspaceValidate(s) == UMI_STATUS_OK ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) *out = *s;
    free(s); return status;
}
