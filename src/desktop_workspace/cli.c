/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native command-line adapter. Opening is a local workspace session, not a
 * read-only database inspector. Commands never execute stored content.
 *---------------------------------------------------------------------------*/
#include "internal.h"
static void Usage(void)
{
    puts("Umicom Desktop Workspace\n"
        "  show --directory PATH\n"
        "  put --directory PATH --expected REV --id ID --title TITLE --text TEXT\n"
        "  remove --directory PATH --expected REV --id ID\n"
        "  restore --directory PATH --expected REV --checkpoint REV\n"
        "  settings --directory PATH --expected REV --theme system|light|dark --font 10..28\n"
        "  --self-test\n"
        "Use a dedicated private directory. Its parent must already exist.\n"
        "An operation opens and cleanly closes a local session. No OS login or guest is started.");
}
static int Number(const char *text, uint64_t *out)
{
    if (!text || !*text) return 0;
    uint64_t value = 0;
    for (size_t i = 0; text[i]; ++i) {
        if (text[i] < '0' || text[i] > '9') return 0;
        unsigned d = (unsigned)(text[i] - '0');
        if (value > (UINT64_MAX-d)/10U) return 0;
        value = value*10U+d;
    }
    *out = value; return 1;
}
int UmiDesktopWorkspaceMain(int argc, char **argv)
{
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "help"))) { Usage(); return 0; }
    if (argc == 2 && !strcmp(argv[1], "--self-test")) {
        UmiDesktopWorkspace *w = NULL;
        UmiStatus status = UmiDesktopWorkspaceOpenMemory(&w);
        if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCloseClean(w);
        UmiDesktopWorkspaceDestroy(w);
        if (status != UMI_STATUS_OK) return 1;
        puts("Native workspace self-check passed. Memory only; no file or process was opened."); return 0;
    }
    if (argc < 4 || argc%2 != 0) { Usage(); return 2; }
    const char *directory = NULL, *id = NULL, *title = NULL, *text = NULL, *theme = NULL;
    uint64_t expected = 0, checkpoint = 0, font = 12;
    unsigned seen = 0;
    const char *options[] = {"--directory","--expected","--id","--title","--text","--checkpoint","--theme","--font"};
    for (int i = 2; i < argc; i += 2) {
        unsigned k = 0; while (k < 8U && strcmp(argv[i], options[k])) ++k;
        if (k == 8U || (seen & (1U << k))) { Usage(); return 2; }
        seen |= 1U << k;
        switch (k) {
        case 0: directory = argv[i+1]; break;
        case 1: if (!Number(argv[i+1], &expected)) return 2; break;
        case 2: id = argv[i+1]; break;
        case 3: title = argv[i+1]; break;
        case 4: text = argv[i+1]; break;
        case 5: if (!Number(argv[i+1], &checkpoint)) return 2; break;
        case 6: theme = argv[i+1]; break;
        case 7: if (!Number(argv[i+1], &font)) return 2; break;
        default: return 2;
        }
    }
    int action = 0; unsigned required = 0;
    if (!strcmp(argv[1], "show")) { action = 1; required = 1U; }
    else if (!strcmp(argv[1], "put")) { action = 2; required = 31U; }
    else if (!strcmp(argv[1], "remove")) { action = 3; required = 7U; }
    else if (!strcmp(argv[1], "restore")) { action = 4; required = 35U; }
    else if (!strcmp(argv[1], "settings")) { action = 5; required = 195U; }
    if (!action || seen != required || (action != 1 && !expected) || font < 10 || font > 28) { Usage(); return 2; }
    if (action == 5 && strcmp(theme, "system") && strcmp(theme, "light") && strcmp(theme, "dark")) return 2;
    UmiDesktopWorkspace *w = NULL; UmiDesktopWorkspaceSnapshot *draft = malloc(sizeof *draft);
    if (!draft) return 1;
    UmiStatus status = UmiDesktopWorkspaceOpenDirectory(directory, &w);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(w, draft);
    if (status == UMI_STATUS_OK && action != 1 && draft->revision != expected) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        if (action == 2) status = UmiDesktopWorkspacePutNote(draft, id, title, text);
        if (action == 3) status = UmiDesktopWorkspaceRemoveNote(draft, id);
        if (action == 5) {
            draft->theme = !strcmp(theme,"dark") ? UMI_DESKTOP_WORKSPACE_DARK :
                !strcmp(theme,"light") ? UMI_DESKTOP_WORKSPACE_LIGHT : UMI_DESKTOP_WORKSPACE_SYSTEM;
            draft->fontPoints = (unsigned)font;
        }
        if (status == UMI_STATUS_OK && (action == 2 || action == 3 || action == 5)) status = UmiDesktopWorkspaceCommit(w, expected, draft);
        if (status == UMI_STATUS_OK && action == 4) status = UmiDesktopWorkspaceRestore(w, expected, checkpoint);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiDesktopWorkspaceRead(w, draft);
        if (status == UMI_STATUS_OK) {
            printf("Revision: %" PRIu64 "\nRetained checkpoints: %" PRIu64 "..%" PRIu64 "\nPrevious session unfinished: %s\n",
                draft->revision, UmiDesktopWorkspaceOldestRevision(w), draft->revision,
                UmiDesktopWorkspacePreviousSessionUnfinished(w) ? "yes" : "no");
            printf("Theme: %u; font: %u pt; notes: %zu\n", (unsigned)draft->theme, draft->fontPoints, draft->noteCount);
            for (size_t i = 0; i < draft->noteCount; ++i)
                printf("%s | %s\n%s\n", draft->notes[i].id, draft->notes[i].title, draft->notes[i].body);
        }
    }
    /* Even a rejected draft can close cleanly: nothing was committed. A
     * poisoned store refuses CloseClean and retains its unfinished marker. */
    if (w) { UmiStatus close = UmiDesktopWorkspaceCloseClean(w); if (status == UMI_STATUS_OK) status = close; }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Stopped (status %d): %s\n", (int)status,
        w ? UmiDesktopWorkspaceDetail(w) : "Workspace unavailable. Check its path, permissions, lock and SQLite support.");
    UmiDesktopWorkspaceDestroy(w); free(draft);
    return status == UMI_STATUS_OK ? 0 : 1;
}
