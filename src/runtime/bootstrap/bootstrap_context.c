/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_context.c
 *
 * PURPOSE:
 *   Implement the bootstrap context behavior for
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
 * File: src/runtime/bootstrap/bootstrap_context.c
 *
 * PURPOSE:
 *   Create the complete application bootstrap input owned by Framework runtime.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_context.h"
#include "../../base/value_construction_internal.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
/*
 * Initialise bootstrap context from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_context_init(UmiBootstrapContext *context,
                                     const char *application_id,
                                     const char *platform_id,
                                     bool dry_run) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || !umi_bootstrap_id_valid(application_id) ||
        !umi_bootstrap_id_valid(platform_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(context, 0, sizeof(*context));
    status = umi_bootstrap_copy_text(context->application_id,
        sizeof(context->application_id), application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(context->platform_id,
        sizeof(context->platform_id), platform_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context->dry_run = dry_run;
    umi_bootstrap_property_set_init(&context->properties);
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_context_init_build_value(UmiBootstrapContext *context,
                                     const char *application_id,
                                     const char *platform_id,
                                     bool dry_run) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || !umi_bootstrap_id_valid(application_id) ||
        !umi_bootstrap_id_valid(platform_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(context, 0, sizeof(*context));
    status = umi_bootstrap_copy_text(context->application_id,
        sizeof(context->application_id), application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(context->platform_id,
        sizeof(context->platform_id), platform_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context->dry_run = dry_run;
    umi_bootstrap_property_set_init(&context->properties);
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_context_init, UmiBootstrapContext, context,
    (UmiBootstrapContext *context, const char *application_id, const char *platform_id, bool dry_run),
    umi_bootstrap_context_init_build_value(candidate, application_id, platform_id, dry_run))
