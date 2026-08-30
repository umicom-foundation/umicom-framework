/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/compatibility.h
 *
 * PURPOSE:
 *   Measure publication compatibility between a source endpoint, group and receiving endpoint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_COMPATIBILITY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_COMPATIBILITY_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostCompatibility {
    bool source_can_publish;
    bool group_accepts;
    bool target_can_follow;
    bool target_accepts;
    bool compatible;
    int score;
} UmiWorkbenchContextHostCompatibility;
UmiStatus umi_workbench_context_host_compatibility_evaluate(
    const UmiWorkbenchContextHost *host,const char *source_endpoint_id,
    const char *target_endpoint_id,const char *group_id,UmiContextKind kind,
    UmiWorkbenchContextHostCompatibility *out_compatibility);
#ifdef __cplusplus
}
#endif
#endif
