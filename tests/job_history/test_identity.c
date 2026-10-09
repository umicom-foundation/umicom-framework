/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_identity.c
 * PURPOSE: Exercise missing, changed, malformed and persisted job identity evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/data/job_history_internal.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static UmiJobIdentity Evidence(void)
{
    UmiJobIdentity value = {0};
    memset(value.subject, 'a', 64U);
    memset(value.configuration, 'b', 64U);
    memset(value.inputs, 'c', 64U);
    return value;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiJobIdentity identity = Evidence(), changed = identity, empty = {0};
    if (strcmp(mode, "comparison") == 0)
    {
        CHECK(UmiJobIdentityCompare(&empty, &identity) == UMI_JOB_IDENTITY_UNKNOWN);
        CHECK(UmiJobIdentityCompare(NULL, &identity) == UMI_JOB_IDENTITY_UNKNOWN);
        CHECK(UmiJobIdentityCompare(&identity, &identity) == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
        changed.subject[0] = 'd';
        CHECK(UmiJobIdentityCompare(&identity, &changed) == UMI_JOB_IDENTITY_DIFFERENT_SUBJECT);
        changed = identity;
        changed.configuration[0] = 'd';
        CHECK(UmiJobIdentityCompare(&identity, &changed) ==
              UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION);
        changed = identity;
        changed.inputs[0] = 'd';
        CHECK(UmiJobIdentityCompare(&identity, &changed) == UMI_JOB_IDENTITY_DIFFERENT_INPUTS);
        changed.inputs[0] = '\0';
        CHECK(UmiJobIdentityCompare(&identity, &changed) == UMI_JOB_IDENTITY_INPUTS_UNRECORDED);
        CHECK(UmiJobIdentityCompare(&changed, &changed) == UMI_JOB_IDENTITY_INPUTS_UNRECORDED);
        return 0;
    }
    if (strcmp(mode, "invalid") == 0)
    {
        changed.subject[64] = 'x';
        CHECK(UmiJobIdentityValidate(&changed) == UMI_STATUS_INVALID_ARGUMENT);
        changed = identity;
        changed.subject[0] = 'A';
        CHECK(UmiJobIdentityValidate(&changed) == UMI_STATUS_INVALID_ARGUMENT);
        changed = identity;
        changed.configuration[3] = '\0';
        CHECK(UmiJobIdentityValidate(&changed) == UMI_STATUS_INVALID_ARGUMENT);
        changed = identity;
        changed.configuration[0] = '\0';
        CHECK(UmiJobIdentityValidate(&changed) == UMI_STATUS_INVALID_ARGUMENT);
        changed = empty;
        strcpy(changed.inputs, identity.inputs);
        CHECK(UmiJobIdentityValidate(&changed) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    UmiJobHistoryEntry entry = {0}, output, sentinel;
    entry.id = 1U;
    entry.revision = 1U;
    entry.state = UMI_JOB_HISTORY_PREPARED;
    entry.total_steps = 1U;
    strcpy(entry.kind, "build");
    strcpy(entry.label, "Build");
    entry.identity = identity;
    char wire[UMI_JOB_HISTORY_WIRE_CAPACITY], broken[UMI_JOB_HISTORY_WIRE_CAPACITY];
    CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
    if (strcmp(mode, "codec") == 0)
    {
        CHECK(UmiJobHistoryDecode(wire, &output) == UMI_STATUS_OK);
        CHECK(UmiJobIdentityCompare(&identity, &output.identity) ==
              UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
        memset(entry.kind, 'k', sizeof(entry.kind) - 1U);
        entry.kind[sizeof(entry.kind) - 1U] = '\0';
        memset(entry.label, 'l', sizeof(entry.label) - 1U);
        entry.label[sizeof(entry.label) - 1U] = '\0';
        entry.id = UINT64_MAX;
        entry.revision = UINT64_MAX;
        CHECK(UmiJobHistoryEncode(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(UmiJobHistoryDecode(wire, &output) == UMI_STATUS_OK && output.id == UINT64_MAX);
        CHECK(strcmp(entry.label, output.label) == 0);
        return 0;
    }
    if (strcmp(mode, "legacy") == 0)
    {
        CHECK(UmiJobHistoryDecode("1|9|1|1|0|1|0|6275696c64|4275696c64", &output) == UMI_STATUS_OK);
        CHECK(!UmiJobIdentityIsRecorded(&output.identity));
        CHECK(UmiJobHistoryEncode(&output, wire, sizeof(wire)) == UMI_STATUS_OK && wire[0] == '1');
        return 0;
    }
    if (strcmp(mode, "malformed") == 0)
    {
        memset(&sentinel, 0x5a, sizeof(sentinel));
        /* Every cut before the mandatory settings digest completes must fail.
         * Missing optional inputs after its delimiter is a valid unknown set. */
        size_t prefix = strlen(wire) - 65U;
        for (size_t length = 0U; length <= prefix; ++length)
        {
            memcpy(broken, wire, length);
            broken[length] = '\0';
            memcpy(&output, &sentinel, sizeof(output));
            CHECK(UmiJobHistoryDecode(broken, &output) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
        }
        strcpy(broken, wire);
        strcat(broken, "|junk");
        CHECK(UmiJobHistoryDecode(broken, &output) == UMI_STATUS_PARSE_ERROR);
        strcpy(broken, wire);
        broken[0] = '9';
        CHECK(UmiJobHistoryDecode(broken, &output) == UMI_STATUS_PARSE_ERROR);
        return 0;
    }
    if (strcmp(mode, "store") != 0)
        return 2;
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    CHECK(snapshot != NULL);
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "identity", &history) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryBeginIdentified(history, "build", "Build", 1U, &identity, &entry) ==
          UMI_STATUS_OK);
    /* Neither the caller's mutable input nor a copied result may rewrite the
     * identity already committed alongside the job. */
    changed = identity;
    identity.subject[0] = 'f';
    entry.identity.subject[0] = 'e';
    CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0U,
                              UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
    CHECK(UmiJobIdentityCompare(&entry.identity, &changed) ==
          UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 1U,
                              UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 1U);
    CHECK(UmiJobIdentityCompare(&snapshot->entries[0].identity, &changed) ==
          UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    char raw[UMI_JOB_HISTORY_WIRE_CAPACITY];
    CHECK(umi_data_server_get(server, "job-history/identity/slot/0", raw, sizeof(raw)) ==
          UMI_STATUS_OK);
    CHECK(strstr(raw, changed.subject) != NULL);
    changed.configuration[0] = '\0';
    memset(&sentinel, 0x5a, sizeof(sentinel));
    memcpy(&output, &sentinel, sizeof(output));
    CHECK(UmiJobHistoryBeginIdentified(history, "build", "Build", 1U, &changed, &output) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(snapshot);
    return 0;
}
