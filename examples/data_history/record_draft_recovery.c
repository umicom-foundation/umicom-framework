/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/record_draft_recovery.c
 *
 * PURPOSE:
 *   The installed SDK can capture and restore a draft without GTK or file I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The installed SDK can capture and restore a draft without GTK or file I/O. */
#include "umicom/data/record_draft_snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiRecordDraft *draft = NULL, *recovered = NULL;
    UmiRecordDraftSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    unsigned char *bytes = NULL; size_t size = 0U;
    UmiStatus status = snapshot == NULL ? UMI_STATUS_OUT_OF_MEMORY : umi_record_draft_create(&draft);
    if (status == UMI_STATUS_OK) status = umi_record_draft_begin(draft, "example.note");
    if (status == UMI_STATUS_OK) status = umi_record_draft_set_content(draft, "Work retained for review", 24U);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_capture(draft, snapshot);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_encode(snapshot, &bytes, &size);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_decode(bytes, size, snapshot);
    if (status == UMI_STATUS_OK) status = umi_record_draft_create(&recovered);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_restore(recovered, snapshot);
    UmiRecordDraftState state = {0};
    if (status == UMI_STATUS_OK) status = umi_record_draft_state(recovered, &state);
    if (status == UMI_STATUS_OK && (!state.dirty || !state.is_new || state.length != 24U)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) puts("Recovered 24 unsaved bytes; no record was committed.");
    else fprintf(stderr, "Recovery example: %s\n", umi_status_text(status));
    umi_record_draft_snapshot_bytes_free(bytes); free(snapshot);
    umi_record_draft_destroy(draft); umi_record_draft_destroy(recovered);
    return status == UMI_STATUS_OK ? 0 : 1;
}
