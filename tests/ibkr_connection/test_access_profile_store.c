/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_access_profile_store.c
 *
 * PURPOSE:
 *   Verify mode preservation, damaged-file refusal and cooperative settings ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_profiles.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/filesystem.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static int NewPath(const char *directory, char *path, size_t capacity)
{
    /* Each invocation claims its own fixture, including retries after a failed
     * run. Never clear an existing test directory or someone else's evidence. */
    uint64_t stamp = UmiIbkrMonotonicMilliseconds();
    for (unsigned attempt = 0U; attempt < 1000U; ++attempt) {
        int count = snprintf(path, capacity, "%s/settings-%" PRIu64 "-%u.txt",
            directory, stamp, attempt);
        CHECK(count > 0 && (size_t)count < capacity);
        UmiOutputFile *file = NULL;
        UmiStatus status = UmiOutputFileCreate(path, &file);
        if (status == UMI_STATUS_ALREADY_EXISTS) continue;
        CHECK(status == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file);
        CHECK(umi_fs_remove_tree(path) == UMI_STATUS_OK);
        return 0;
    }
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    char path[UMI_PATH_CAPACITY], guard[UMI_PATH_CAPACITY];
    CHECK(NewPath(argv[2], path, sizeof(path)) == 0);
    int count = snprintf(guard, sizeof(guard), "%s.umi-guard", path);
    CHECK(count > 0 && (size_t)count < sizeof(guard));
    UmiBrokerAccessProfiles before, output;
    CHECK(UmiBrokerAccessProfilesDefault(&before) == UMI_STATUS_OK);
    output = before;
    UmiBrokerAccessPlan paper = before.paper, live = before.live;
    paper.port = 9000U; paper.clientId = 201; paper.program = UMI_IBKR_GATEWAY;
    live.port = 9001U; live.clientId = 202;
    const char *name = argv[1];
    if (strcmp(name, "missing") == 0) {
        CHECK(UmiBrokerAccessProfilesLoad(path, &output) == UMI_STATUS_NOT_FOUND);
        CHECK(output.paper.port == before.paper.port && output.live.port == before.live.port);
        CHECK(!umi_fs_exists(path) && !umi_fs_exists(guard));
    } else if (strcmp(name, "preserve") == 0) {
        CHECK(UmiBrokerAccessProfilesStore(path, &paper) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesStore(path, &live) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesLoad(path, &output) == UMI_STATUS_OK);
        CHECK(output.paper.port == paper.port && output.paper.program == paper.program);
        CHECK(output.paper.clientId == paper.clientId && output.live.clientId == live.clientId);
        CHECK(output.live.port == live.port);
        UmiIbkrConnectionOptions options;
        CHECK(UmiBrokerAccessPlanConnectionOptions(&output.live, false, &options) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "damaged") == 0) {
        const char damaged[] = "UMICOM-BROKER-PROFILES 99\nkeep for review\n";
        CHECK(umi_fs_write_bytes(path, damaged, sizeof(damaged) - 1U) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesStore(path, &paper) == UMI_STATUS_PARSE_ERROR);
        unsigned char *bytes = NULL; size_t size = 0U;
        CHECK(UmiInputFileRead(path, sizeof(damaged), &bytes, &size) == UMI_STATUS_OK);
        CHECK(size == sizeof(damaged) - 1U && memcmp(bytes, damaged, size) == 0);
        UmiInputFileFree(bytes);
        CHECK(UmiBrokerAccessProfilesLoad(path, &output) == UMI_STATUS_PARSE_ERROR);
        CHECK(output.paper.port == before.paper.port);
    } else if (strcmp(name, "busy") == 0) {
        UmiFileLease *lease = NULL;
        CHECK(UmiFileLeaseTryAcquire(guard, &lease) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesStore(path, &paper) == UMI_STATUS_BUSY);
        CHECK(!umi_fs_exists(path));
        CHECK(UmiFileLeaseRelease(&lease) == UMI_STATUS_OK);
        CHECK(umi_fs_exists(guard));
        CHECK(UmiBrokerAccessProfilesStore(path, &paper) == UMI_STATUS_OK);
    } else if (strcmp(name, "invalid") == 0) {
        UmiBrokerAccessPlan guest;
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_GUEST, &guest) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesStore(path, &guest) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesStore("relative-settings.txt", &paper) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesLoad(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_fs_exists(path) && !umi_fs_exists(guard));
    } else return 2;
    /* The invocation owns these two leaves and all leases have been released. */
    if (umi_fs_exists(path)) CHECK(umi_fs_remove_tree(path) == UMI_STATUS_OK);
    if (umi_fs_exists(guard)) CHECK(umi_fs_remove_tree(guard) == UMI_STATUS_OK);
    return 0;
}
