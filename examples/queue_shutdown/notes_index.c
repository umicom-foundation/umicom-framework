/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/queue_shutdown/notes_index.c
 * A Notes Master Controller owns the text snapshots and the queue. It stops
 * admission, optionally cancels work, then polls native completion before
 * releasing either owner. No files, windows or external services are opened.
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/clock.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

typedef struct NotesIndex {
    atomic_int permitted;
    atomic_int entered;
    const char *text;
    size_t words;
} NotesIndex;

static UmiStatus CountWords(UmiTaskContext *context, void *data)
{
    NotesIndex *index = data;
    int inWord = 0;
    atomic_store_explicit(&index->entered, 1, memory_order_release);
    /* This gate makes the ownership exercise repeatable. A real indexer would
     * receive its complete input snapshot before submission, then scan it. */
    while (!atomic_load_explicit(&index->permitted, memory_order_acquire)) {
        if (umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    for (const unsigned char *p = (const unsigned char *)index->text; *p != 0U; ++p) {
        int whitespace = *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n';
        if (umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
        if (!whitespace && !inWord) ++index->words;
        inWord = !whitespace;
    }
    return UMI_STATUS_OK;
}

static uint64_t Now(void)
{
    UmiClock clock = umi_clock_system();
    return clock.monotonic_nanoseconds(&clock);
}

int main(int argc, char **argv)
{
    int cancel;
    UmiTaskQueue *queue = NULL;
    UmiTask *tasks[3] = {NULL, NULL, NULL};
    NotesIndex notes[3];
    const char *text[3] = {
        "Save the workshop notes before closing the window.",
        "A queued search owns its input snapshot.",
        "Cancellation is a request, not proof of completion."
    };
    UmiTaskQueueConfig config = {1U, 4U};
    UmiTaskQueueShutdownSnapshot snapshot;
    UmiStatus status = UMI_STATUS_OK;
    uint64_t start;
    if (argc != 2 || (strcmp(argv[1], "drain") && strcmp(argv[1], "cancel") &&
        strcmp(argv[1], "--self-test"))) {
        puts("Usage: umicom-queue-shutdown drain|cancel|--self-test");
        return argc == 2 && !strcmp(argv[1], "--help") ? 0 : 2;
    }
    cancel = !strcmp(argv[1], "cancel");
    if (!UmiThreadCanTryJoin()) {
        puts("NOT RUN: this build has no nonblocking native join adapter.");
        return 77;
    }
    for (size_t i = 0U; i < 3U; ++i) {
        atomic_init(&notes[i].permitted, i != 0U);
        atomic_init(&notes[i].entered, 0);
        notes[i].text = text[i];
        notes[i].words = 0U;
    }
    status = umi_task_queue_create(&config, &queue);
    if (status != UMI_STATUS_OK) goto cleanup;
    for (size_t i = 0U; i < 3U; ++i) {
        UmiTaskConfig task = {"Index Notes snapshot", CountWords, &notes[i], NULL, NULL};
        status = umi_task_create(&task, &tasks[i]);
        if (status == UMI_STATUS_OK) status = umi_task_queue_submit(queue, tasks[i]);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    start = Now();
    while (!atomic_load_explicit(&notes[0].entered, memory_order_acquire)) {
        if (Now() - start > 5000000000ULL) { status = UMI_STATUS_TIMEOUT; goto cleanup; }
        umi_thread_sleep_ms(1U);
    }
    status = UmiTaskQueueRequestShutdown(queue, cancel, cancel);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiTaskQueueTryFinishShutdown(queue);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_BUSY) goto cleanup;
    puts("Admission closed. The controller can process another event while workers finish.");
    atomic_store_explicit(&notes[0].permitted, 1, memory_order_release);
    start = Now();
    while ((status = UmiTaskQueueTryFinishShutdown(queue)) == UMI_STATUS_BUSY) {
        /* A GTK host schedules the next pass in its event loop instead of
         * sleeping in a UI callback. This command-line lesson yields here. */
        if (Now() - start > 5000000000ULL) { status = UMI_STATUS_TIMEOUT; goto cleanup; }
        umi_thread_sleep_ms(1U);
    }
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiTaskQueueCaptureShutdown(queue, &snapshot);
    if (status != UMI_STATUS_OK) goto cleanup;
    printf("Stopped: submitted=%" PRIu64 ", completed=%" PRIu64
        ", cancelled=%" PRIu64 ", failed=%" PRIu64 ".\n",
        snapshot.tasks.submitted, snapshot.tasks.completed,
        snapshot.tasks.cancelled, snapshot.tasks.failed);
    if (!cancel) printf("Notes word counts: %zu, %zu, %zu.\n",
        notes[0].words, notes[1].words, notes[2].words);
    status = UmiTaskQueueReleaseStopped(&queue);
    if (status != UMI_STATUS_OK) goto cleanup;
    puts("Practice complete. Every worker was joined before its text snapshots left scope.");
cleanup:
    if (queue != NULL) {
        atomic_store(&notes[0].permitted, 1);
        (void)UmiTaskQueueRequestShutdown(queue, 1, 1);
        /* CLI error cleanup may wait. A graphical controller instead retains
         * its context and keeps polling; never free a busy worker's payload. */
        UmiStatus joined = umi_task_queue_shutdown(queue, 1);
        if (joined == UMI_STATUS_OK) (void)UmiTaskQueueReleaseStopped(&queue);
        else fprintf(stderr, "Shutdown cleanup failed (%d).\n", (int)joined);
    }
    for (size_t i = 0U; i < 3U; ++i) umi_task_destroy(tasks[i]);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Notes exercise failed (%d).\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
