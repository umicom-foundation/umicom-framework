/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/approval_persistence.h
 *
 * PURPOSE:
 *   Persist recent AI developer approval requests and their decisions through
 *   the existing Framework Session Store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_PERSISTENCE_H

#include "umicom/ai_developer_experience/approval_queue.h"
#include "umicom/ai_developer_experience/persistence_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_PERSISTED_APPROVALS 5U
#define UMI_AI_DEVELOPER_APPROVAL_ARGUMENT_CHUNKS 8U

UmiStatus umi_ai_developer_approvals_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperApprovalQueue *queue);

UmiStatus umi_ai_developer_approvals_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperApprovalQueue *queue,
    size_t *out_restored_count);

#ifdef __cplusplus
}
#endif
#endif
