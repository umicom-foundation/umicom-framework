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
typedef enum UmiNativeSelfHostStage { UMI_NC_SELFHOST_EXPERIMENTAL=0, UMI_NC_SELFHOST_BOOTSTRAP=1, UMI_NC_SELFHOST_CANDIDATE=2, UMI_NC_SELFHOST_SELFHOST=3 } UmiNativeSelfHostStage;
typedef struct UmiNativeSelfHostEvidence { bool lexer; bool preprocessor; bool parser; bool semantics; bool ir; bool optimizer; bool codegen; bool object_writer; bool linker_bridge; bool diagnostics; size_t passing_tests; size_t required_tests; size_t blocker_count; } UmiNativeSelfHostEvidence;
typedef struct UmiNativeSelfHostAssessment { UmiNativeSelfHostStage stage; size_t capability_count; size_t capability_total; double coverage; bool ready_for_default; } UmiNativeSelfHostAssessment;
UmiNativeSelfHostAssessment umi_nc_self_host_assess(const UmiNativeSelfHostEvidence *evidence);
#ifdef __cplusplus
}
#endif
#endif
