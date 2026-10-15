/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_profile.h
 *
 * PURPOSE:
 *   Group application starters into reusable environment/application profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_PROFILE_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_PROFILE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap starter profile from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_starter_profile_init(UmiBootstrapStarterProfile *profile,
                                             const char *profile_id);
/**
 * Add bootstrap starter profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_profile_add(
    UmiBootstrapStarterProfile *profile,
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id);

#ifdef __cplusplus
}
#endif

#endif
