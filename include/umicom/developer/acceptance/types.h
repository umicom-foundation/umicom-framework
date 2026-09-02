/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/types.h
 *
 * PURPOSE:
 *   Define bounded evidence and gate-result records used to verify Framework developer capabilities before product adoption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_ACCEPTANCE_TYPES_H
#define UMICOM_DEVELOPER_ACCEPTANCE_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_ACCEPTANCE_MAX_EVIDENCE 256U
#define UMI_DEVELOPER_ACCEPTANCE_MAX_REQUIREMENTS 64U
#define UMI_DEVELOPER_ACCEPTANCE_TEXT_CAPACITY 256U

/**
 * List the named developer acceptance state values accepted by this public contract.
 */
typedef enum UmiDeveloperAcceptanceState {
    UMI_DEVELOPER_ACCEPTANCE_UNKNOWN = 0,
    UMI_DEVELOPER_ACCEPTANCE_PASS = 1,
    UMI_DEVELOPER_ACCEPTANCE_FAIL = 2,
    UMI_DEVELOPER_ACCEPTANCE_BLOCKED = 3
} UmiDeveloperAcceptanceState;

/**
 * Represent the developer acceptance evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperAcceptanceEvidence {
    char capability_id[128];
    char check_id[128];
    UmiDeveloperAcceptanceState state;
    char detail[UMI_DEVELOPER_ACCEPTANCE_TEXT_CAPACITY];
} UmiDeveloperAcceptanceEvidence;

/**
 * Represent the developer acceptance gate data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperAcceptanceGate {
    char gate_id[128];
    char title[UMI_DEVELOPER_ACCEPTANCE_TEXT_CAPACITY];
    const char *required_capability_ids[UMI_DEVELOPER_ACCEPTANCE_MAX_REQUIREMENTS];
    size_t requirement_count;
} UmiDeveloperAcceptanceGate;

/**
 * Represent the developer acceptance gate result data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperAcceptanceGateResult {
    char gate_id[128];
    size_t passed_count;
    size_t failed_count;
    size_t blocked_count;
    size_t missing_count;
    int passed;
} UmiDeveloperAcceptanceGateResult;

#ifdef __cplusplus
}
#endif
#endif
