/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_source.h
 *
 * PURPOSE:
 *   Describe trusted configuration sources and their precedence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_SOURCE_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_SOURCE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap configuration source from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_bootstrap_configuration_source_init(
    UmiBootstrapConfigurationSource *source,
    const char *source_id,
    int32_t precedence,
    bool trusted);

#ifdef __cplusplus
}
#endif

#endif
