/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Count words in a Notes draft on a worker, then join before releasing its data.
 * This is the complete memory-only lesson; it performs no application edits.
 *---------------------------------------------------------------------------*/
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NotesIndex {
    const char *text;
    size_t words;
    atomic_int cancelRequested;
} NotesIndex;

static int CountWords(void *context)
{
    NotesIndex *index = context;
    int inWord = 0;
    for (const unsigned char *p = (const unsigned char *)index->text; *p; ++p) {
        if (atomic_load_explicit(&index->cancelRequested, memory_order_relaxed))
            return 2;
        int space = *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t';
        if (!space && !inWord) ++index->words;
        inWord = !space;
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--self-test") && strcmp(argv[1], "cancel"))) {
        fputs("Usage: umicom-thread-lifecycle [--self-test|cancel]\n", stderr);
        return 2;
    }
    NotesIndex index = {0};
    index.text = "Save the workshop notes before closing the window.";
    atomic_init(&index.cancelRequested, 0);
    UmiThread *thread = NULL;
    int result = -1;
    const int cancelling = argc == 2 && !strcmp(argv[1], "cancel");
    /* A queued cancellation is deterministic in this lesson. In an application,
     * its controller sets this atomic flag when the user presses Cancel. */
    if (cancelling) atomic_store(&index.cancelRequested, 1);
    UmiStatus status = umi_thread_start(CountWords, &index, &thread);
    if (status != UMI_STATUS_OK) { fprintf(stderr,"Start failed: %d\n",(int)status); return 1; }
    status = UmiThreadTryGetExitCode(thread, &result);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_BUSY) {
        /* Even an observation error is not a reason to free a live context. */
        (void)umi_thread_join(thread, NULL); (void)UmiThreadRelease(&thread); return 1;
    }
    status = umi_thread_join(thread, &result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr,"Join failed: %d. Caller data must stay alive.\n",(int)status);
        /* This executable now ends the process; a GUI must retain the context. */
        return 1;
    }
    if (UmiThreadRelease(&thread) != UMI_STATUS_OK || thread != NULL) return 1;
    if (cancelling) {
        if (result != 2 || index.words != 0U) return 1;
        puts("The Notes index was cancelled before counting; the worker was joined.");
    } else {
        if (result != 0 || index.words != 8U) return 1;
        printf("Notes index: %zu words. The worker was joined before its data left scope.\n",index.words);
    }
    puts("Practice complete. No file, database, window or network was opened.");
    return 0;
}
