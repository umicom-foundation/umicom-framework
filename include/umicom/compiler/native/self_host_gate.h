/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/self_host_gate.h
 *
 * PURPOSE:
 *   Evaluate evidence for moving Umicc from experimental bootstrap use toward controlled self-hosting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SELF_HOST_GATE_H
#define UMICOM_COMPILER_NATIVE_SELF_HOST_GATE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native self host stage values accepted by this public contract.
 */
typedef enum UmiNativeSelfHostStage { UMI_NC_SELFHOST_EXPERIMENTAL=0, UMI_NC_SELFHOST_BOOTSTRAP=1, UMI_NC_SELFHOST_CANDIDATE=2, UMI_NC_SELFHOST_SELFHOST=3 } UmiNativeSelfHostStage;
/**
 * Represent the native self host evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeSelfHostEvidence { bool lexer; bool preprocessor; bool parser; bool semantics; bool ir; bool optimizer; bool codegen; bool object_writer; bool linker_bridge; bool diagnostics; size_t passing_tests; size_t required_tests; size_t blocker_count; } UmiNativeSelfHostEvidence;
/**
 * Represent the native self host assessment data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeSelfHostAssessment { UmiNativeSelfHostStage stage; size_t capability_count; size_t capability_total; double coverage; bool ready_for_default; } UmiNativeSelfHostAssessment;
/**
 * Provide the nc self host assess operation used by this module and its client
 * applications.
 */
UmiNativeSelfHostAssessment umi_nc_self_host_assess(const UmiNativeSelfHostEvidence *evidence);
#ifdef __cplusplus
}
#endif
#endif
