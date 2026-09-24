/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/service.h
 *
 * PURPOSE:
 *   Discover and evaluate Framework-owned language, refactoring, RAG, AI coding
 *   and AI Teacher workbench policies through stable capability identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_SERVICE_H
#define UMICOM_DEVELOPER_INTELLIGENCE_SERVICE_H

#include <stddef.h>

#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t UmiDeveloperIntelligenceServiceCount(void);

UmiStatus UmiDeveloperIntelligenceServiceIdAt(
    size_t index,
    const char **outId);

UmiStatus UmiDeveloperIntelligenceServiceEvaluate(
    const char *id,
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
