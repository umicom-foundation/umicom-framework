/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_source.c
 *
 * PURPOSE:
 *   Implement the configuration source behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_source.c
 *
 * PURPOSE:
 *   Describe trusted configuration sources and their precedence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_source.h"
#include "../../base/value_construction_internal.h"


#include <string.h>
/*
 * Initialise bootstrap configuration source from caller-provided values so later
 * operations receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_configuration_source_init(
    UmiBootstrapConfigurationSource *source,
    const char *source_id,
    int32_t precedence,
    bool trusted) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || !umi_bootstrap_id_valid(source_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(source, 0, sizeof(*source));
    status = umi_bootstrap_copy_text(source->source_id, sizeof(source->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    source->precedence = precedence;
    source->trusted = trusted;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_configuration_source_init_build_value(
    UmiBootstrapConfigurationSource *source,
    const char *source_id,
    int32_t precedence,
    bool trusted) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || !umi_bootstrap_id_valid(source_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(source, 0, sizeof(*source));
    status = umi_bootstrap_copy_text(source->source_id, sizeof(source->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    source->precedence = precedence;
    source->trusted = trusted;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_configuration_source_init, UmiBootstrapConfigurationSource, source,
    (UmiBootstrapConfigurationSource *source, const char *source_id, int32_t precedence, bool trusted),
    umi_bootstrap_configuration_source_init_build_value(candidate, source_id, precedence, trusted))
