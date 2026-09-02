/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/feature_condition.h
 *
 * PURPOSE:
 *   Evaluate feature-pack presence without application-specific branches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_FEATURE_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_FEATURE_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap feature condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_feature_condition_match(const UmiBootstrapIdList *features,
                                           const char *feature_id);

#ifdef __cplusplus
}
#endif

#endif
