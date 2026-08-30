/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_generation_evidence.h
 *
 * PURPOSE:
 *   Publish the public application generation evidence contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATION_EVIDENCE_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATION_EVIDENCE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    uint64_t umi_project_workspace_application_generation_evidence_fingerprint(const char*text);
#ifdef __cplusplus
}
#endif
#endif
