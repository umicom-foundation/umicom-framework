/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/compiler_session.h
 *
 * PURPOSE:
 *   Coordinate one native compilation session across target, provider, pipeline and deterministic fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_COMPILER_SESSION_H
#define UMICOM_COMPILER_NATIVE_COMPILER_SESSION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/compilation_pipeline.h"
#include "umicom/compiler/native/compile_fingerprint.h"
#include "umicom/compiler/native/optimization_profile.h"
#include "umicom/compiler/native/provider_bridge.h"
#include "umicom/compiler/native/target_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeCompilerSession { char session_id[UMI_NC_ID_CAPACITY]; UmiNativeTargetProfile target; UmiNativeOptimizationProfile optimization; UmiNativeProviderPlan provider_plan; UmiNativeCompilationPipeline pipeline; uint64_t compile_fingerprint; size_t diagnostics; size_t errors; bool active; bool completed; } UmiNativeCompilerSession;
UmiStatus umi_nc_compiler_session_begin(UmiNativeCompilerSession *session,const char *session_id,const char *target_triple,UmiNativeOptimizationLevel optimization,const UmiNativeProviderAvailability *providers,bool self_host_ready,uint64_t source_hash,uint64_t options_hash,uint64_t dependency_hash);
UmiStatus umi_nc_compiler_session_stage(UmiNativeCompilerSession *session,UmiNativeCompileStage stage,bool passed);
UmiStatus umi_nc_compiler_session_finish(UmiNativeCompilerSession *session);
#ifdef __cplusplus
}
#endif
#endif
