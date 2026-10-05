/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/acceptance_rule.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_RULE_H
#define UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_RULE_H

#include "umicom/application/production/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/types.h"

/**
 * Represent the application production acceptance rule data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionAcceptanceRule {
    int require_manifest;
    int require_layout_projection;
    int require_capabilities;
    int require_tests;
    int require_evidence;
    int allow_degraded_optional_capabilities;
} UmiApplicationProductionAcceptanceRule;

/**
 * Provide the application production acceptance rule default operation used by this module
 * and its client applications.
 */
UmiApplicationProductionAcceptanceRule
umi_application_production_acceptance_rule_default(void);
/**
 * Check that application production acceptance rule satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_production_acceptance_rule_validate(
    const UmiApplicationProductionAcceptanceRule *rule);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_application_production_acceptance_rule_archive_encode(const UmiApplicationProductionAcceptanceRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_application_production_acceptance_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiApplicationProductionAcceptanceRule *value);

#ifdef __cplusplus
}
#endif
#endif
