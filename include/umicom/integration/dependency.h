/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/dependency.h
 *
 * PURPOSE:
 *   Represent required and optional application/capability dependencies.
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

#ifndef UMICOM_INTEGRATION_DEPENDENCY_H
#define UMICOM_INTEGRATION_DEPENDENCY_H

#include "umicom/integration/types.h"

/**
 * List the named integration dependency target values accepted by this public contract.
 */
typedef enum UmiIntegrationDependencyTarget {
    UMI_INTEGRATION_TARGET_APPLICATION = 0,
    UMI_INTEGRATION_TARGET_CAPABILITY = 1
} UmiIntegrationDependencyTarget;

/**
 * Represent the integration dependency data shared with callers of this public contract.
 */
typedef struct UmiIntegrationDependency {
    UmiIntegrationDependencyTarget target_type;
    UmiIntegrationDependencyKind kind;
    char target[UMI_INTEGRATION_ID_CAPACITY];
} UmiIntegrationDependency;

#endif
