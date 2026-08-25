/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/dead_letter_policy.h
 *
 * PURPOSE:
 *   Describe bounded dead-letter escalation and retention rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_DEAD_LETTER_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_DEAD_LETTER_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricDeadLetterPolicy {
    char policy_id[UMI_FABRIC_ID_CAPACITY];
    char destination[UMI_FABRIC_URI_CAPACITY];
    uint32_t after_attempts;
    uint64_t retention_ms;
    bool include_payload;
} UmiFabricDeadLetterPolicy;

UmiStatus umi_fabric_dead_letter_policy_init(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload);
UmiStatus umi_fabric_dead_letter_policy_validate(const UmiFabricDeadLetterPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
