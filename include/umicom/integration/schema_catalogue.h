/* Umicom Framework | Integration schema catalogue | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_INTEGRATION_SCHEMA_CATALOGUE_H
#define UMICOM_INTEGRATION_SCHEMA_CATALOGUE_H
#include "umicom/integration/designer_types.h"
typedef struct UmiIntegrationSchemaDescriptor {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerSchemaKind kind;
    unsigned major;
    unsigned minor;
    char fingerprint[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char source_uri[UMI_INTEGRATION_DESIGNER_URI_CAPACITY];
} UmiIntegrationSchemaDescriptor;
typedef struct UmiIntegrationSchemaCatalogue {
    UmiIntegrationSchemaDescriptor items[UMI_INTEGRATION_DESIGNER_MAX_SCHEMAS];
    size_t count;
} UmiIntegrationSchemaCatalogue;
UmiStatus umi_integration_schema_catalogue_register(UmiIntegrationSchemaCatalogue *catalogue,const UmiIntegrationSchemaDescriptor *schema);
const UmiIntegrationSchemaDescriptor *umi_integration_schema_catalogue_find(const UmiIntegrationSchemaCatalogue *catalogue,const char *id);
bool umi_integration_schema_compatible(const UmiIntegrationSchemaDescriptor *provider,const UmiIntegrationSchemaDescriptor *consumer);
#endif
