/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/link_plan.h
 *
 * PURPOSE:
 *   Build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_LINK_PLAN_H
#define UMICOM_COMPILER_NATIVE_LINK_PLAN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_LINK_INPUTS 64U
#define UMI_NC_MAX_LINK_LIBRARIES 64U
#define UMI_NC_MAX_LINK_SEARCH_PATHS 32U
/**
 * List the named native link output kind values accepted by this public contract.
 */
typedef enum UmiNativeLinkOutputKind { UMI_NC_LINK_EXECUTABLE=1, UMI_NC_LINK_SHARED=2, UMI_NC_LINK_STATIC=3 } UmiNativeLinkOutputKind;
/**
 * Represent the native link plan data shared with callers of this public contract.
 */
typedef struct UmiNativeLinkPlan { char output[UMI_NC_PATH_CAPACITY]; UmiNativeLinkOutputKind kind; char inputs[UMI_NC_MAX_LINK_INPUTS][UMI_NC_PATH_CAPACITY]; size_t input_count; char libraries[UMI_NC_MAX_LINK_LIBRARIES][UMI_NC_NAME_CAPACITY]; size_t library_count; char search_paths[UMI_NC_MAX_LINK_SEARCH_PATHS][UMI_NC_PATH_CAPACITY]; size_t search_path_count; } UmiNativeLinkPlan;
/**
 * Initialise nc link plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_link_plan_init(UmiNativeLinkPlan *plan,const char *output,UmiNativeLinkOutputKind kind);
/**
 * Provide the nc link plan add input operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_link_plan_add_input(UmiNativeLinkPlan *plan,const char *path);
/**
 * Provide the nc link plan add library operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_link_plan_add_library(UmiNativeLinkPlan *plan,const char *library);
/**
 * Provide the nc link plan fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_link_plan_fingerprint(const UmiNativeLinkPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
