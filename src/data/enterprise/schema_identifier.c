/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_identifier.c
 *
 * PURPOSE:
 *   Represent a qualified schema object identifier without binding to a specific SQL engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_identifier.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_identifier_init(UmiDataSchemaIdentifier *item, const char *catalog, const char *schema, const char *name) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->catalog,sizeof(item->catalog),catalog); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (schema != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->schema,sizeof(item->schema),schema); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    return umi_data_enterprise_copy_text(item->name,sizeof(item->name),name);
    return umi_data_schema_identifier_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_identifier_validate(const UmiDataSchemaIdentifier *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
