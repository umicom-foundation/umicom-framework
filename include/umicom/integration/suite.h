/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/suite.h
 *
 * PURPOSE:
 *   Define required and optional members for an integration-aware application suite.
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

#ifndef UMICOM_INTEGRATION_SUITE_H
#define UMICOM_INTEGRATION_SUITE_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

/**
 * Represent the integration suite member data shared with callers of this public contract.
 */
typedef struct UmiIntegrationSuiteMember {
    char application_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationDependencyKind kind;
    unsigned preferred_frontend;
} UmiIntegrationSuiteMember;

/**
 * Represent the integration suite definition data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationSuiteDefinition {
    char id[UMI_INTEGRATION_ID_CAPACITY];
    char name[UMI_INTEGRATION_NAME_CAPACITY];
    UmiIntegrationSuiteMember members[UMI_INTEGRATION_MAX_MEMBERS];
    size_t member_count;
} UmiIntegrationSuiteDefinition;

/**
 * Initialise integration suite from caller-provided values so later operations receive a
 * known state.
 */
void umi_integration_suite_init(
    UmiIntegrationSuiteDefinition *suite,
    const char *id,
    const char *name);
/**
 * Provide the integration suite add member operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_suite_add_member(
    UmiIntegrationSuiteDefinition *suite,
    const char *application_id,
    UmiIntegrationDependencyKind kind,
    unsigned preferred_frontend);

#endif
