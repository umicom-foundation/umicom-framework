/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Model edits operate on drafts, never on the authoritative saved snapshot.
 * UTF-8 is checked here so the same rules apply to GTK and native clients.
 *---------------------------------------------------------------------------*/
#include "internal.h"

int DwId(const char *text, size_t capacity)
{
    if (!text || !memchr(text, 0, capacity) || !text[0]) return 0;
    for (size_t i = 0; text[i]; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.')) return 0;
    }
    return strcmp(text, ".") && strcmp(text, "..");
}

int DwUtf8(const char *text, size_t capacity, int multiline)
{
    if (!text || !memchr(text, 0, capacity)) return 0;
    size_t length = strlen(text), i = 0;
    while (i < length) {
        unsigned char lead = (unsigned char)text[i++];
        if (lead < 0x80U) {
            if (lead == 0x7fU || (lead < 0x20U &&
                !(multiline && (lead == '\n' || lead == '\r' || lead == '\t')))) return 0;
            continue;
        }
        unsigned count;
        uint32_t code, minimum;
        if (lead >= 0xc2U && lead <= 0xdfU) { count = 1; code = lead & 31U; minimum = 0x80U; }
        else if (lead >= 0xe0U && lead <= 0xefU) { count = 2; code = lead & 15U; minimum = 0x800U; }
        else if (lead >= 0xf0U && lead <= 0xf4U) { count = 3; code = lead & 7U; minimum = 0x10000U; }
        else return 0;
        if (count > length - i) return 0;
        for (unsigned j = 0; j < count; ++j) {
            unsigned char c = (unsigned char)text[i++];
            if ((c & 0xc0U) != 0x80U) return 0;
            code = (code << 6) | (c & 63U);
        }
        if (code < minimum || code > 0x10ffffU || (code >= 0xd800U && code <= 0xdfffU)) return 0;
    }
    return 1;
}

void UmiDesktopWorkspaceSnapshotInit(UmiDesktopWorkspaceSnapshot *snapshot)
{
    if (!snapshot) return;
    memset(snapshot, 0, sizeof *snapshot);
    snapshot->fontPoints = 12U;
    snapshot->sidebarVisible = 1;
}

UmiStatus UmiDesktopWorkspaceValidate(const UmiDesktopWorkspaceSnapshot *s)
{
    if (!s || s->noteCount > UMI_DESKTOP_WORKSPACE_NOTES ||
        s->theme < UMI_DESKTOP_WORKSPACE_SYSTEM || s->theme > UMI_DESKTOP_WORKSPACE_DARK ||
        s->fontPoints < 10U || s->fontPoints > 28U ||
        (s->sidebarVisible != 0 && s->sidebarVisible != 1) ||
        !memchr(s->selectedNote, 0, sizeof s->selectedNote)) return UMI_STATUS_INVALID_ARGUMENT;
    int selected = !s->noteCount && !s->selectedNote[0];
    for (size_t i = 0; i < s->noteCount; ++i) {
        const UmiDesktopWorkspaceNote *note = &s->notes[i];
        if (!DwId(note->id, sizeof note->id) || !note->title[0] ||
            !DwUtf8(note->title, sizeof note->title, 0) ||
            !DwUtf8(note->body, sizeof note->body, 1)) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0; j < i; ++j)
            if (!strcmp(note->id, s->notes[j].id)) return UMI_STATUS_INVALID_ARGUMENT;
        if (!strcmp(note->id, s->selectedNote)) selected = 1;
    }
    return selected ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus UmiDesktopWorkspacePutNote(UmiDesktopWorkspaceSnapshot *s,
    const char *id, const char *title, const char *body)
{
    if (UmiDesktopWorkspaceValidate(s) != UMI_STATUS_OK ||
        !DwId(id, UMI_DESKTOP_WORKSPACE_ID) || !title || !title[0] ||
        !DwUtf8(title, UMI_DESKTOP_WORKSPACE_TITLE, 0) ||
        !DwUtf8(body, UMI_DESKTOP_WORKSPACE_BODY, 1)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t at = 0;
    while (at < s->noteCount && strcmp(s->notes[at].id, id)) ++at;
    if (at == UMI_DESKTOP_WORKSPACE_NOTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy to a temporary first: callers may reuse a borrowed draft field as
     * input without its previous contents being cleared halfway through. */
    UmiDesktopWorkspaceNote next = {0};
    memcpy(next.id, id, strlen(id) + 1U);
    memcpy(next.title, title, strlen(title) + 1U);
    memcpy(next.body, body, strlen(body) + 1U);
    s->notes[at] = next;
    if (at == s->noteCount) ++s->noteCount;
    memcpy(s->selectedNote, next.id, strlen(next.id) + 1U);
    return UMI_STATUS_OK;
}

UmiStatus UmiDesktopWorkspaceRemoveNote(UmiDesktopWorkspaceSnapshot *s, const char *id)
{
    if (UmiDesktopWorkspaceValidate(s) != UMI_STATUS_OK ||
        !DwId(id, UMI_DESKTOP_WORKSPACE_ID)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t at = 0;
    while (at < s->noteCount && strcmp(s->notes[at].id, id)) ++at;
    if (at == s->noteCount) return UMI_STATUS_NOT_FOUND;
    int wasSelected = !strcmp(s->selectedNote, id);
    for (size_t i = at; i + 1U < s->noteCount; ++i) s->notes[i] = s->notes[i + 1U];
    memset(&s->notes[--s->noteCount], 0, sizeof s->notes[0]);
    if (wasSelected) {
        memset(s->selectedNote, 0, sizeof s->selectedNote);
        if (s->noteCount) strcpy(s->selectedNote, s->notes[0].id);
    }
    return UMI_STATUS_OK;
}
