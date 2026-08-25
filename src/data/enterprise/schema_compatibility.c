/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_compatibility.c
 *
 * PURPOSE:
 *   Define release/migration policy for acceptable schema changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_compatibility.h"
#include <string.h>

/* Defaults favour bounded, reviewable behaviour over aggressive execution. */
void umi_data_schema_compatibility_default(UmiDataSchemaCompatibilityPolicy *policy) {
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->allow_additive = true;
    policy->allow_rebuild = true;
    policy->allow_breaking = true;
}

/* Evaluation is side-effect free so callers can preview decisions. */
UmiStatus umi_data_schema_compatibility_evaluate(const UmiDataSchemaCompatibilityPolicy *policy, UmiDataCompatibility compatibility, bool *out_allowed) {
    if (policy == NULL || out_allowed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_allowed = compatibility == UMI_DATA_COMPATIBLE || (compatibility == UMI_DATA_COMPATIBLE_WITH_REBUILD && policy->allow_rebuild) || (compatibility == UMI_DATA_BREAKING && policy->allow_breaking);
    return UMI_STATUS_OK;
}
