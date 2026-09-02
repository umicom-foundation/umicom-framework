/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/schema_catalogue.c
 *
 * PURPOSE:
 *   Implement the schema catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration schema catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/schema_catalogue.h"
#include <string.h>

/*
 * Find integration schema catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiIntegrationSchemaDescriptor *umi_integration_schema_catalogue_find(const UmiIntegrationSchemaCatalogue *catalogue,const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(catalogue->items[index].id,id) == 0) return &catalogue->items[index];
    return NULL;
}

/*
 * Add integration schema catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_schema_catalogue_register(UmiIntegrationSchemaCatalogue *catalogue,const UmiIntegrationSchemaDescriptor *schema)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || schema == NULL || schema->id[0] == '\0' || schema->major == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_schema_catalogue_find(catalogue,schema->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_INTEGRATION_DESIGNER_MAX_SCHEMAS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count] = *schema;
    catalogue->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration schema compatible operation used by this module and its client
 * applications.
 */
bool umi_integration_schema_compatible(const UmiIntegrationSchemaDescriptor *provider,const UmiIntegrationSchemaDescriptor *consumer)
{
    return provider != NULL && consumer != NULL && provider->kind == consumer->kind && provider->major == consumer->major;
}
