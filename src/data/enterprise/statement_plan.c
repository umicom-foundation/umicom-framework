/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/statement_plan.c
 *
 * PURPOSE:
 *   Describe a prepared statement contract and its query/schema fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/statement_plan.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_statement_plan_init(UmiDataStatementPlan *item, const char *statement_id, uint64_t query_fingerprint, uint64_t schema_fingerprint, size_t parameter_count, bool read_only) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->statement_id,sizeof(item->statement_id),statement_id);if(s!=UMI_STATUS_OK)return s;item->query_fingerprint=query_fingerprint;item->schema_fingerprint=schema_fingerprint;item->parameter_count=parameter_count;item->read_only=read_only;
    return umi_data_statement_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_statement_plan_validate(const UmiDataStatementPlan *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->statement_id[0] != '\0' && item->query_fingerprint != 0U && item->schema_fingerprint != 0U && item->parameter_count <= UMI_DATA_ENTERPRISE_MAX_ITEMS)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
