/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/starter_dependency.h
 *
 * PURPOSE:
 *   Declare explicit dependencies between reusable application starters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_STARTER_DEPENDENCY_H
#define UMICOM_RUNTIME_BOOTSTRAP_STARTER_DEPENDENCY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_starter_dependency_init(
    UmiBootstrapStarterDependency *dependency,
    const char *starter_id,
    const char *requires_starter_id,
    bool required);

#ifdef __cplusplus
}
#endif

#endif
