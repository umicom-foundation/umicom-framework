/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/catalogue.h
 *
 * PURPOSE:
 *   Expose stable Integration Fabric capability identifiers for application manifests.
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

#ifndef UMICOM_INTEGRATION_CATALOGUE_H
#define UMICOM_INTEGRATION_CATALOGUE_H

#include <stddef.h>

size_t umi_integration_catalogue_count(void);
const char *umi_integration_catalogue_at(size_t index);

#endif
