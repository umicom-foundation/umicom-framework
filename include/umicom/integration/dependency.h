/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/dependency.h
 *
 * PURPOSE:
 *   Represent required and optional application/capability dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_DEPENDENCY_H
#define UMICOM_INTEGRATION_DEPENDENCY_H

#include "umicom/integration/types.h"

typedef enum UmiIntegrationDependencyTarget {
    UMI_INTEGRATION_TARGET_APPLICATION = 0,
    UMI_INTEGRATION_TARGET_CAPABILITY = 1
} UmiIntegrationDependencyTarget;

typedef struct UmiIntegrationDependency {
    UmiIntegrationDependencyTarget target_type;
    UmiIntegrationDependencyKind kind;
    char target[UMI_INTEGRATION_ID_CAPACITY];
} UmiIntegrationDependency;

#endif
