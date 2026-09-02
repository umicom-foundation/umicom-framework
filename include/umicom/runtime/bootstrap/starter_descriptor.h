/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_descriptor.h
 *
 * PURPOSE:
 *   Describe reusable application starters built from feature packs and auto-configurations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap starter descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_starter_descriptor_init(
    UmiBootstrapStarterDescriptor *starter,
    const char *starter_id,
    const char *feature_pack_id,
    int32_t priority);
/**
 * Provide the bootstrap starter descriptor add auto configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_bootstrap_starter_descriptor_add_auto_configuration(
    UmiBootstrapStarterDescriptor *starter,
    const char *configuration_id);

#ifdef __cplusplus
}
#endif

#endif
