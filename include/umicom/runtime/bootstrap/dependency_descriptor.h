/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/dependency_descriptor.h
 *
 * PURPOSE:
 *   Declare required, optional and collection dependencies between Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_DEPENDENCY_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_DEPENDENCY_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_dependency_descriptor_init(
    UmiBootstrapDependencyDescriptor *dependency,
    const char *service_id,
    const char *qualifier,
    bool required,
    bool collection);

#ifdef __cplusplus
}
#endif

#endif
