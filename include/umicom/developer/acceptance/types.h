/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/types.h
 *
 * PURPOSE:
 *   Define bounded evidence and gate-result records used to verify Framework developer capabilities before product adoption.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiDeveloperAcceptanceState {
    UMI_DEVELOPER_ACCEPTANCE_UNKNOWN = 0,
    UMI_DEVELOPER_ACCEPTANCE_PASS = 1,
    UMI_DEVELOPER_ACCEPTANCE_FAIL = 2,
    UMI_DEVELOPER_ACCEPTANCE_BLOCKED = 3
} UmiDeveloperAcceptanceState;

typedef struct UmiDeveloperAcceptanceEvidence {
    char capability_id[128];
    char check_id[128];
    UmiDeveloperAcceptanceState state;
    char detail[UMI_DEVELOPER_ACCEPTANCE_TEXT_CAPACITY];
} UmiDeveloperAcceptanceEvidence;

typedef struct UmiDeveloperAcceptanceGate {
    char gate_id[128];
    char title[UMI_DEVELOPER_ACCEPTANCE_TEXT_CAPACITY];
    const char *required_capability_ids[UMI_DEVELOPER_ACCEPTANCE_MAX_REQUIREMENTS];
    size_t requirement_count;
} UmiDeveloperAcceptanceGate;

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
