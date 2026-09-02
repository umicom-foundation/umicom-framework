/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_key.c
 *
 * PURPOSE:
 *   Implement the service key behavior for
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
 * File: src/runtime/bootstrap/service_key.c
 *
 * PURPOSE:
 *   Create and compare qualified service keys used for explicit dependency lookup.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

/*
 * Initialise bootstrap service key from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_bootstrap_service_key_init(UmiBootstrapServiceKey *key,
                                         const char *service_id,
                                         const char *qualifier) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || !umi_bootstrap_id_valid(service_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_bootstrap_copy_text(key->service_id, sizeof(key->service_id), service_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_bootstrap_copy_text(key->qualifier, sizeof(key->qualifier),
                                   qualifier != NULL ? qualifier : "");
}

/*
 * Provide the bootstrap service key equal operation used by this module and its client
 * applications.
 */
bool umi_bootstrap_service_key_equal(const UmiBootstrapServiceKey *left,
                                     const UmiBootstrapServiceKey *right) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->service_id, right->service_id) == 0 &&
           strcmp(left->qualifier, right->qualifier) == 0;
}

/*
 * Provide the bootstrap service key hash operation used by this module and its client
 * applications.
 */
uint64_t umi_bootstrap_service_key_hash(const UmiBootstrapServiceKey *key) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL) return 0U;
    return umi_bootstrap_hash_text(key->service_id) ^
           (umi_bootstrap_hash_text(key->qualifier) << 1U);
}
