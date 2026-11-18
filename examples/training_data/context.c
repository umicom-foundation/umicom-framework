/*-----------------------------------------------------------------------------
 * Umicom Framework SDK Example
 * File: examples/training_data/context.c
 *
 * PURPOSE:
 *   Inspect three explicit teaching records before selecting a native decoder context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/jsonl_context.h"
#include <stdio.h>

int main(void)
{
    static const char records[]=
        "{\"source_id\":\"one\",\"prompt\":\"p\",\"completion\":\"c\"}\n"
        "{\"source_id\":\"two\",\"prompt\":\"\",\"completion\":\"xyz\"}\n"
        "{\"source_id\":\"three\",\"prompt\":\"long\",\"completion\":\"reply\"}\n";
    UmiAiTrainingJsonlContextSummary summary;
    UmiAiTrainingJsonlReport report;
    UmiStatus status=UmiAiTrainingJsonlInspectContext(records,sizeof records-1U,
        4,NULL,&summary,&report);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Context inspection refused: status=%d, line=%zu\n",(int)status,report.failedLine);
        return 1;
    }
    /* A completed parse is not approval to discard the overlong example. Keep
     * that decision visible to the person reviewing the dataset. */
    printf("Umicom native byte-pair context: %zu rows\n",summary.contextLength);
    printf("Records: %zu; fitting: %zu; overlong: %zu; first overlong line: %zu\n",
        summary.records,summary.fittingRecords,summary.overlongRecords,summary.firstOverlongLine);
    printf("All required rows: %zu; fitting valid: %zu; fitting selected: %zu; fitting padding: %zu\n",
        summary.requiredRows,summary.fittingValidRows,summary.fittingSelectedRows,summary.fittingPaddingRows);
    puts("Inspection only. No text was truncated and no model was trained.");
    return summary.records==3U && summary.fittingRecords==2U && summary.overlongRecords==1U &&
        summary.firstOverlongLine==3U && summary.requiredRows==17U && summary.fittingValidRows==7U &&
        summary.fittingSelectedRows==6U && summary.fittingPaddingRows==1U && !ferror(stdout) ? 0 : 1;
}
