/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/types.h
 *
 * PURPOSE:
 *   Define bounded orchestration signals for language intelligence, refactoring,
 *   local/remote AI, RAG, controlled coding assistance and AI Teacher workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_TYPES_H
#define UMICOM_DEVELOPER_INTELLIGENCE_TYPES_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_INTELLIGENCE_ID_CAPACITY 96U
#define UMI_DEVELOPER_INTELLIGENCE_LABEL_CAPACITY 128U

typedef struct UmiDeveloperIntelligenceInput {
    const char *label;
    double lexicalScore;
    double semanticScore;
    double contextScore;
    double confidence;
    double risk;
    double baselineScore;
    uint64_t matched;
    uint64_t total;
    uint64_t conflicts;
    uint64_t citations;
    uint64_t sourceCount;
    uint64_t usedUnits;
    uint64_t budgetUnits;
    int localAvailable;
    int remoteAvailable;
    int remoteAllowed;
    int approved;
    int trusted;
    int active;
    uint64_t revision;
} UmiDeveloperIntelligenceInput;

typedef struct UmiDeveloperIntelligenceSnapshot {
    char id[UMI_DEVELOPER_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_DEVELOPER_INTELLIGENCE_LABEL_CAPACITY];
    double score;
    double confidence;
    double risk;
    double delta;
    uint64_t matched;
    uint64_t total;
    uint64_t conflicts;
    uint64_t citations;
    uint64_t sourceCount;
    uint64_t budgetRemaining;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    int localPreferred;
    int remoteUsable;
    uint64_t revision;
} UmiDeveloperIntelligenceSnapshot;

typedef UmiStatus (*UmiDeveloperIntelligenceEvaluator)(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

void UmiDeveloperIntelligenceInputInit(UmiDeveloperIntelligenceInput *input);
void UmiDeveloperIntelligenceSnapshotInit(UmiDeveloperIntelligenceSnapshot *snapshot);
UmiStatus UmiDeveloperIntelligenceSnapshotValidate(
    const UmiDeveloperIntelligenceSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
