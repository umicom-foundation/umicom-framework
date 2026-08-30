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

const UmiIntegrationSchemaDescriptor *umi_integration_schema_catalogue_find(const UmiIntegrationSchemaCatalogue *catalogue,const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].id,id) == 0) return &catalogue->items[index];
    return NULL;
}

UmiStatus umi_integration_schema_catalogue_register(UmiIntegrationSchemaCatalogue *catalogue,const UmiIntegrationSchemaDescriptor *schema)
{
    if (catalogue == NULL || schema == NULL || schema->id[0] == '\0' || schema->major == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_integration_schema_catalogue_find(catalogue,schema->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_INTEGRATION_DESIGNER_MAX_SCHEMAS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count] = *schema;
    catalogue->count += 1U;
    return UMI_STATUS_OK;
}

bool umi_integration_schema_compatible(const UmiIntegrationSchemaDescriptor *provider,const UmiIntegrationSchemaDescriptor *consumer)
{
    return provider != NULL && consumer != NULL && provider->kind == consumer->kind && provider->major == consumer->major;
}
