/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_request.c
 *
 * PURPOSE:
 *   Implement the resolution request behavior for
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
 * File: src/runtime/bootstrap/resolution_request.c
 *
 * PURPOSE:
 *   Describe one dependency-resolution request including scope and optionality.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_request.h"
#include "umicom/runtime/bootstrap/scope_kind.h"


#include <string.h>
UmiStatus umi_bootstrap_resolution_request_init(
    UmiBootstrapResolutionRequest *request,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapScopeKind scope,
    bool required) {
    if (request == NULL || key == NULL || !umi_bootstrap_id_valid(key->service_id) ||
        umi_bootstrap_scope_kind_rank(scope) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(request, 0, sizeof(*request));
    request->key = *key;
    request->scope = scope;
    request->required = required;
    return UMI_STATUS_OK;
}
