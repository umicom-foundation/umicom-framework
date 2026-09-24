/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/developer_intelligence_internal.h
 *
 * PURPOSE:
 *   Share policy evaluation across language and AI development workflows while
 *   canonical Language, AI, Knowledge and Teacher engines remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_INTERNAL_H
#define UMICOM_DEVELOPER_INTELLIGENCE_INTERNAL_H

#include "umicom/developer_intelligence/types.h"

typedef enum UmiDeveloperIntelligencePolicyKind {
    UMI_DEVELOPER_INTELLIGENCE_SCORE = 1,
    UMI_DEVELOPER_INTELLIGENCE_ROUTE = 2,
    UMI_DEVELOPER_INTELLIGENCE_BUDGET = 3,
    UMI_DEVELOPER_INTELLIGENCE_FRESHNESS = 4,
    UMI_DEVELOPER_INTELLIGENCE_CONFLICT = 5,
    UMI_DEVELOPER_INTELLIGENCE_APPROVAL = 6,
    UMI_DEVELOPER_INTELLIGENCE_TRUST = 7,
    UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL = 8,
    UMI_DEVELOPER_INTELLIGENCE_CITATION = 9,
    UMI_DEVELOPER_INTELLIGENCE_RISK = 10,
    UMI_DEVELOPER_INTELLIGENCE_MASTERY = 11
} UmiDeveloperIntelligencePolicyKind;

typedef struct UmiDeveloperIntelligencePolicy {
    const char *id;
    const char *label;
    UmiDeveloperIntelligencePolicyKind kind;
    double minimumScore;
    double maximumRisk;
    double lexicalWeight;
    double semanticWeight;
    double contextWeight;
    double confidenceWeight;
    uint64_t requiredSources;
    int requireTrust;
    int preferLocal;
} UmiDeveloperIntelligencePolicy;

UmiStatus UmiDeveloperIntelligenceEvaluatePolicy(
    const UmiDeveloperIntelligencePolicy *policy,
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

#endif
