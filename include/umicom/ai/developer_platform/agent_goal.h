/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_goal.h
 *
 * PURPOSE:
 *   Describe a governed AI developer goal without granting execution authority.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_GOAL_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_GOAL_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev agent goal data shared with callers of this public contract.
 */
typedef struct UmiAiDevAgentGoal {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevAgentGoal;

/**
 * Initialise ai dev agent goal from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_agent_goal_init(UmiAiDevAgentGoal *value);
/**
 * Provide the ai dev agent goal configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_agent_goal_configure(UmiAiDevAgentGoal *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev agent goal satisfies its contract before another service relies on it.
 */
UmiStatus umi_ai_dev_agent_goal_validate(const UmiAiDevAgentGoal *value);
/**
 * Provide the ai dev agent goal evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_agent_goal_evidence_score(const UmiAiDevAgentGoal *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ai_dev_agent_goal_archive_encode(const UmiAiDevAgentGoal *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ai_dev_agent_goal_archive_decode(const void *bytes, size_t byte_count,
    UmiAiDevAgentGoal *value);

#ifdef __cplusplus
}
#endif

#endif
