/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/main.c
 *
 * PURPOSE:
 *   Show why a failed process cannot become successful merely by filtering its problems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review.h"
#include "umicom/build/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiBuildHistory *history = NULL;
    UmiBuildReview *review = NULL;
    UmiBuildResult *result = calloc(1U, sizeof *result);
    UmiBuildReviewSummary summary;
    UmiBuildReviewFilter filter = {UMI_BUILD_DIAGNOSTIC_FATAL, ""};
    UmiStatus status = result == NULL ? UMI_STATUS_OUT_OF_MEMORY : umi_build_history_create(2U, &history);
    if (status != UMI_STATUS_OK) { free(result); return 1; }
    result->operation_id = 1U;
    result->phase = UMI_BUILD_PHASE_BUILD;
    result->state = UMI_BUILD_STATE_FAILED;
    result->status = UMI_STATUS_IO_ERROR;
    result->exit_code = 1;
    strcpy(result->profile_id, "notes.debug");
    strcpy(result->command, "cmake --build build");
    strcpy(result->output, "C:/Umicom Notes/main.c:8:5: error: expected ';' before return\n");
    status = umi_build_parse_output(result->output, &result->diagnostics);
    if (status == UMI_STATUS_OK) status = umi_build_history_append(history, result);
    if (status == UMI_STATUS_OK) status = UmiBuildReviewCapture(history, 2U, &review);
    /* The copied review does not depend on the worker or history lifetime. */
    free(result);
    umi_build_history_destroy(history);
    if (status == UMI_STATUS_OK) status = UmiBuildReviewSummarise(review, 0U, &filter, &summary);
    if (status == UMI_STATUS_OK) {
        printf("Outcome: %s\nVisible problems after filtering: %zu\n",
            UmiBuildReviewOutcomeText(summary.outcome), summary.visibleDiagnostics);
        puts("Filtering did not turn the failed build into a successful build.");
        puts("Practice complete. Memory only; no compiler, files, database or network was used.");
    }
    UmiBuildReviewDestroy(review);
    return status == UMI_STATUS_OK ? 0 : 1;
}
