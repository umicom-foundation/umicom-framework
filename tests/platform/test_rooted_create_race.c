/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_rooted_create_race.c
 *
 * PURPOSE:
 *   Confirm concurrent exports cannot overwrite the winner's complete file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../build_log/fixture.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>

#define WRITERS 8U
#define PAYLOAD_BYTES 65536U
typedef struct Writer {
    const char *root;
    atomic_uint *ready;
    atomic_int *go;
    unsigned char value;
    UmiStatus status;
    UmiRootedFileCreation report;
} Writer;

static int WriteExample(void *data)
{
    Writer *writer = data;
    unsigned char *bytes = malloc(PAYLOAD_BYTES);
    if (bytes == NULL) return 1;
    memset(bytes, writer->value, PAYLOAD_BYTES);
    atomic_fetch_add_explicit(writer->ready, 1U, memory_order_release);
    while (!atomic_load_explicit(writer->go, memory_order_acquire)) umi_thread_sleep_ms(1U);
    writer->status = UmiRootedFileCreate(writer->root, "shared.c", bytes, PAYLOAD_BYTES, &writer->report);
    free(bytes);
    return 0;
}
int main(void)
{
    char root[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    atomic_uint ready;
    atomic_int go;
    atomic_init(&ready, 0U);
    atomic_init(&go, 0);
    Writer writers[WRITERS] = {0};
    UmiThread *threads[WRITERS] = {0};
    for (unsigned i = 0U; i < WRITERS; ++i) {
        writers[i].root = root;
        writers[i].ready = &ready;
        writers[i].go = &go;
        writers[i].value = (unsigned char)(i + 1U);
        CHECK(umi_thread_start(WriteExample, &writers[i], &threads[i]) == UMI_STATUS_OK);
    }
    unsigned waited = 0U;
    while (atomic_load_explicit(&ready, memory_order_acquire) != WRITERS && waited < 20000U) {
        umi_thread_sleep_ms(1U);
        ++waited;
    }
    /* Release every started worker before assessing readiness, so a slow
     * environment never leaves the callbacks waiting on a failed assertion. */
    atomic_store_explicit(&go, 1, memory_order_release);
    for (unsigned i = 0U; i < WRITERS; ++i) {
        int result = -1;
        CHECK(umi_thread_join(threads[i], &result) == UMI_STATUS_OK && result == 0);
        CHECK(UmiThreadRelease(&threads[i]) == UMI_STATUS_OK && threads[i] == NULL);
    }
    CHECK(atomic_load_explicit(&ready, memory_order_acquire) == WRITERS);
    unsigned winners = 0U;
    unsigned char winner = 0U;
    for (unsigned i = 0U; i < WRITERS; ++i) {
        Writer *writer = &writers[i];
        if (writer->status == UMI_STATUS_OK) {
            ++winners;
            winner = writer->value;
            CHECK(writer->report.created && writer->report.complete &&
                  writer->report.bytes_written == PAYLOAD_BYTES);
        } else {
            /* Windows may refuse the sharing check while the winner holds
             * its exclusive handle. Neither refusal may create or write. */
            CHECK(writer->status == UMI_STATUS_ALREADY_EXISTS || writer->status == UMI_STATUS_PERMISSION_DENIED);
            CHECK(!writer->report.created && !writer->report.complete && writer->report.bytes_written == 0U);
        }
    }
    CHECK(winners == 1U);
    unsigned char *bytes = NULL;
    size_t size = 0U;
    CHECK(UmiRootedFileRead(root, "shared.c", PAYLOAD_BYTES, &bytes, &size) == UMI_STATUS_OK);
    CHECK(size == PAYLOAD_BYTES);
    for (size_t i = 0U; i < size; ++i) CHECK(bytes[i] == winner);
    UmiRootedFileFree(bytes);
    return 0;
}
