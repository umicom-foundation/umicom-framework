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
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
UmiStatus umi_bootstrap_context_init(UmiBootstrapContext *context,
                                     const char *application_id,
                                     const char *platform_id,
                                     bool dry_run) {
    UmiStatus status;
    if (context == NULL || !umi_bootstrap_id_valid(application_id) ||
        !umi_bootstrap_id_valid(platform_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(context, 0, sizeof(*context));
    status = umi_bootstrap_copy_text(context->application_id,
        sizeof(context->application_id), application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(context->platform_id,
        sizeof(context->platform_id), platform_id);
    if (status != UMI_STATUS_OK) return status;
    context->dry_run = dry_run;
    umi_bootstrap_property_set_init(&context->properties);
    return UMI_STATUS_OK;
}
