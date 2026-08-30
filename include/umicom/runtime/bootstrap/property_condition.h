/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/property_condition.h
 *
 * PURPOSE:
 *   Evaluate configuration-property equality and presence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_PROPERTY_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_PROPERTY_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


bool umi_bootstrap_property_condition_match(
    const UmiBootstrapPropertySet *properties,
    const char *key,
    const char *expected_value);

#ifdef __cplusplus
}
#endif

#endif
