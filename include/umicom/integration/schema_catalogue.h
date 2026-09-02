/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/schema_catalogue.h
 *
 * PURPOSE:
 *   Publish the public schema catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_SCHEMA_CATALOGUE_H
#define UMICOM_INTEGRATION_SCHEMA_CATALOGUE_H
#include "umicom/integration/designer_types.h"
/**
 * Represent the integration schema descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationSchemaDescriptor {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerSchemaKind kind;
    unsigned major;
    unsigned minor;
    char fingerprint[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char source_uri[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
} UmiIntegrationSchemaDescriptor;
/**
 * Represent the integration schema catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationSchemaCatalogue {
    UmiIntegrationSchemaDescriptor items[UMI_INTEGRATION_DESIGNER_MAX_SCHEMAS];
    size_t count;
} UmiIntegrationSchemaCatalogue;
/**
 * Add integration schema catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_schema_catalogue_register(UmiIntegrationSchemaCatalogue *catalogue,const UmiIntegrationSchemaDescriptor *schema);
/**
 * Find integration schema catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiIntegrationSchemaDescriptor *umi_integration_schema_catalogue_find(const UmiIntegrationSchemaCatalogue *catalogue,const char *id);
/**
 * Provide the integration schema compatible operation used by this module and its client
 * applications.
 */
bool umi_integration_schema_compatible(const UmiIntegrationSchemaDescriptor *provider,const UmiIntegrationSchemaDescriptor *consumer);
#endif
