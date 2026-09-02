/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/catalogue.h
 *
 * PURPOSE:
 *   Expose stable Integration Fabric capability identifiers for application manifests.
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

#ifndef UMICOM_INTEGRATION_CATALOGUE_H
#define UMICOM_INTEGRATION_CATALOGUE_H

#include <stddef.h>

/**
 * Return the number of records represented by integration catalogue without changing their
 * state.
 */
size_t umi_integration_catalogue_count(void);
/**
 * Find integration catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const char *umi_integration_catalogue_at(size_t index);

#endif
