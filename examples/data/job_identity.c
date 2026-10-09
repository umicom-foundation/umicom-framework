/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data/job_identity.c
 * PURPOSE: Attach non-secret content identities to a job and explain changed input evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include "umicom/data/job_history.h"
#include "umicom/data/job_inputs.h"
#include <stdio.h>

/* The same stable logical name must be used when comparing input revisions.
 * This helper owns the builder only; the caller retains the actual buffer. */
static UmiStatus NotesIdentity(const char *bytes, size_t length, char digest[65])
{
    UmiJobInputs *inputs = NULL;
    UmiStatus status = UmiJobInputsCreate(1U, &inputs);
    if (status == UMI_STATUS_OK)
        status = UmiJobInputsAddBytes(inputs, "notes.txt", bytes, length);
    if (status == UMI_STATUS_OK)
        status = UmiJobInputsSeal(inputs, digest);
    UmiJobInputsDestroy(inputs);
    return status;
}

/* This small example hashes explicit buffers. A real import or render host
 * must define its complete input set, retain its owned snapshot and include all
 * settings that influence output. A file name or modification time is not a
 * substitute for a content digest. No outside program or provider is invoked. */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    UmiJobIdentity identity = {0}, current = {0};
    UmiJobHistoryEntry entry = {0};
    UmiStatus status = UmiSha256Buffer("private-example-project", 23U, identity.subject);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer("plain-text-import", 17U, identity.configuration);
    if (status == UMI_STATUS_OK)
        status = NotesIdentity("Saved notes\n", 12U, identity.inputs);
    if (status == UMI_STATUS_OK)
        status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryCreate(server, "example.import", &history);
    if (status == UMI_STATUS_OK)
        status =
            UmiJobHistoryBeginIdentified(history, "import", "Local notes", 1U, &identity, &entry);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0U,
                                     UMI_STATUS_OK, &entry);
    /* Count the lines in the exact buffer whose digest was recorded. Larger
     * hosts replace this operation with their own owned, immutable input set. */
    size_t lines = 0U;
    const char notes[] = "Saved notes\n";
    if (status == UMI_STATUS_OK)
        for (size_t index = 0U; index < sizeof(notes) - 1U; ++index)
            if (notes[index] == '\n')
                ++lines;
    if (status == UMI_STATUS_OK)
        printf("Counted %zu saved line(s).\n", lines);
    if (status == UMI_STATUS_OK)
        status = UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED,
                                     1U, UMI_STATUS_OK, &entry);
    current = identity;
    if (status == UMI_STATUS_OK)
        status = NotesIdentity("Changed notes\n", 14U, current.inputs);
    if (status == UMI_STATUS_OK)
        puts(UmiJobIdentityComparisonText(UmiJobIdentityCompare(&entry.identity, &current)));
    else
        fprintf(stderr, "%s\n", umi_status_text(status));
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    return status == UMI_STATUS_OK ? 0 : 1;
}
