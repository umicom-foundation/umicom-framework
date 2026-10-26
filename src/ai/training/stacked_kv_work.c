/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_work.c
 * PURPOSE: Validate observed native cache-storage work against the existing
 *          conditional C23 stacked-prefix workload planner.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_work.h"

#include <stdint.h>

UmiStatus UmiAiTrainingStackedKvWorkQualify(
    const UmiAiTrainingStackedKvStore *store,
    const UmiAiTrainingStackedPrefixCachePlan *plan,
    UmiAiTrainingStackedKvWorkAudit *outAudit)
{
    UmiAiTrainingStackedKvStoreInfo inspected = {0};
    UmiStatus status;
    if (store == NULL || plan == NULL || outAudit == NULL ||
        plan->generatedTokens == 0U || plan->contextTokens == 0U ||
        plan->promptTokens == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedKvStoreInspect(store, &inspected);
    if (status != UMI_STATUS_OK) return status;
    if (inspected.contextCapacity != plan->contextTokens ||
        inspected.transformerLayers != plan->transformerLayers ||
        inspected.initialPromptTokens != plan->promptTokens ||
        inspected.reservedKvBytes != (size_t)plan->kvTensorFp32Bytes ||
        inspected.singleRowAppends != plan->prefixSingleRowAppends ||
        inspected.fullWindowRebuilds != plan->fullWindowRebuilds ||
        inspected.acceptedTokenLayerRows != plan->candidateTokenLayerRows ||
        inspected.singleRowAppends > UINT64_MAX - inspected.fullWindowRebuilds ||
        inspected.singleRowAppends + inspected.fullWindowRebuilds !=
            (uint64_t)(plan->generatedTokens - 1U))
        return UMI_STATUS_INVALID_STATE;
    const size_t progressed = plan->promptTokens + plan->generatedTokens - 1U;
    const size_t expectedRetained = progressed < plan->contextTokens
        ? progressed : plan->contextTokens;
    if (inspected.retainedTokens != expectedRetained)
        return UMI_STATUS_INVALID_STATE;
    UmiAiTrainingStackedKvWorkAudit result = {0};
    result.plannedTokenLayerRows = plan->candidateTokenLayerRows;
    result.observedAcceptedTokenLayerRows = inspected.acceptedTokenLayerRows;
    result.conditionalRowsAvoided = plan->avoidedTokenLayerRows;
    result.observedRebuilds = inspected.fullWindowRebuilds;
    result.observedRetainedTokens = inspected.retainedTokens;
    result.reservedKvBytes = inspected.reservedKvBytes;
    *outAudit = result;
    return UMI_STATUS_OK;
}
