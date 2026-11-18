/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_reload/test_reload_batch.c
 *
 * PURPOSE:
 *   Verify revision-aware reload completion, bounded state and failure preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/frontend/hot_reload.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)
/* Fixed-capacity fixtures live outside the thread stack. CTest starts a fresh
 * process for each case; production code gains no global mutable state. */
static UmiFrontendHotReload reload, other, before;
static UmiFrontendReloadBatch batch, competing;
static UmiFrontendReloadReceipt receipt;

static int EditsDuringBuild(void)
{
    CHECK(umi_frontend_hot_reload_init(&reload, 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view.ui", 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "theme.css", 2) == UMI_STATUS_OK);
    before = reload;
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view.ui", 3) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "events.c", 4) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_OK);
    CHECK(receipt.acknowledged == 1 && receipt.remaining == 2 && receipt.generation == 2);
    CHECK(strcmp(reload.pending[0].path, "view.ui") == 0 && reload.pending[0].revision == 3);
    CHECK(strcmp(reload.pending[1].path, "events.c") == 0 && reload.pending[1].revision == 4);
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_OK);
    CHECK(receipt.acknowledged == 2 && receipt.remaining == 0 && receipt.generation == 3);
    return 0;
}
static int FailedAndCompetingBuilds(void)
{
    CHECK(umi_frontend_hot_reload_init(&reload, 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "counter.c", 7) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    before = reload;
    /* Failure has no acknowledgement: a retry still captures the same work. */
    CHECK(umi_frontend_hot_reload_capture(&reload, &competing) == UMI_STATUS_OK);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    CHECK(competing.count == 1 && competing.changes[0].revision == 7);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_OK);
    before = reload;
    receipt = (UmiFrontendReloadReceipt){99, 98, 97};
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &competing, &receipt) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    CHECK(receipt.generation == 99 && receipt.acknowledged == 98 && receipt.remaining == 97);
    return 0;
}
static int AllPathsChangedAgain(void)
{
    CHECK(umi_frontend_hot_reload_init(&reload, 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view.ui", 0) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view.ui", 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_OK);
    CHECK(receipt.acknowledged == 0 && receipt.remaining == 1 && receipt.generation == 2);
    CHECK(reload.pending[0].revision == 1);
    return 0;
}
static int InvalidState(void)
{
    CHECK(umi_frontend_hot_reload_init(&reload, 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view.ui", 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    other = reload;
    CHECK(umi_frontend_hot_reload_acknowledge(&other, &batch, &receipt) == UMI_STATUS_INVALID_STATE);
    reload.pending_count = UMI_FRONTEND_DEV_MAX_CHANGES + 1U;
    before = reload;
    CHECK(umi_frontend_hot_reload_queue(&reload, "x", 2) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_frontend_hot_reload_capture(&reload, &competing) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    reload.pending_count = 1;
    memset(reload.pending[0].path, 'x', sizeof(reload.pending[0].path));
    CHECK(umi_frontend_hot_reload_capture(&reload, &competing) == UMI_STATUS_INVALID_STATE);
    reload = other;
    batch.count = UMI_FRONTEND_DEV_MAX_CHANGES + 1U;
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_INVALID_STATE);
    batch.count = 2;
    batch.changes[1] = batch.changes[0];
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_frontend_hot_reload_capture(NULL, &batch) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_frontend_hot_reload_capture(&reload, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, NULL, &receipt) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static int LimitsAndCompatibility(void)
{
    char name[32], long_path[UMI_FRONTEND_DEV_PATH_CAPACITY + 1U];
    uint64_t generation = 99;
    CHECK(umi_frontend_hot_reload_init(&reload, 1) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_complete(&reload, &generation) == UMI_STATUS_NOT_FOUND);
    CHECK(generation == 99);
    for (size_t i = 0; i < UMI_FRONTEND_DEV_MAX_CHANGES; ++i) {
        snprintf(name, sizeof(name), "view-%zu.ui", i);
        CHECK(umi_frontend_hot_reload_queue(&reload, name, i) == UMI_STATUS_OK);
    }
    before = reload;
    CHECK(umi_frontend_hot_reload_queue(&reload, "extra", 1) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    CHECK(umi_frontend_hot_reload_queue(&reload, "view-0.ui", 900) == UMI_STATUS_OK);
    CHECK(reload.pending[0].revision == 900);
    memset(long_path, 'x', sizeof(long_path) - 1U);
    long_path[sizeof(long_path) - 1U] = '\0';
    before = reload;
    CHECK(umi_frontend_hot_reload_queue(&reload, long_path, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0);
    CHECK(umi_frontend_hot_reload_complete(&reload, &generation) == UMI_STATUS_OK);
    CHECK(generation == 2 && reload.pending_count == 0);
    CHECK(umi_frontend_hot_reload_queue(&reload, "last", 1) == UMI_STATUS_OK);
    reload.generation = UINT64_MAX;
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_OK);
    before = reload;
    CHECK(umi_frontend_hot_reload_complete(&reload, &generation) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_frontend_hot_reload_acknowledge(&reload, &batch, &receipt) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&reload, &before, sizeof(reload)) == 0 && generation == 2);
    CHECK(umi_frontend_hot_reload_init(&reload, 0) == UMI_STATUS_OK);
    CHECK(umi_frontend_hot_reload_queue(&reload, "x", 1) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_frontend_hot_reload_capture(&reload, &batch) == UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "interleaved") == 0) return EditsDuringBuild();
    if (strcmp(argv[1], "failed") == 0) return FailedAndCompetingBuilds();
    if (strcmp(argv[1], "all-newer") == 0) return AllPathsChangedAgain();
    if (strcmp(argv[1], "invalid") == 0) return InvalidState();
    if (strcmp(argv[1], "limits") == 0) return LimitsAndCompatibility();
    return 2;
}
