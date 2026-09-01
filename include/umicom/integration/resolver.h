/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/resolver.h
 *
 * PURPOSE:
 *   Resolve required and optional dependencies against discovered applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_RESOLVER_H
#define UMICOM_INTEGRATION_RESOLVER_H

#include "umicom/base/status.h"
#include "umicom/integration/dependency.h"
#include "umicom/integration/registry.h"

typedef struct UmiIntegrationResolution {
    size_t satisfied_required;
    size_t satisfied_optional;
    size_t missing_required;
    size_t missing_optional;
    char first_missing_required[UMI_INTEGRATION_ID_CAPACITY];
} UmiIntegrationResolution;

UmiStatus umi_integration_resolve(
    const UmiIntegrationRegistry *registry,
    const UmiIntegrationDependency *dependencies,
    size_t dependency_count,
    UmiIntegrationResolution *out_resolution);

#endif
