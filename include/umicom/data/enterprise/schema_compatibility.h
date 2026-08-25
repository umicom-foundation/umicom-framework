/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_compatibility.h
 *
 * PURPOSE:
 *   Define release/migration policy for acceptable schema changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_COMPATIBILITY_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_COMPATIBILITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSchemaCompatibilityPolicy {
    bool allow_additive;
    bool allow_rebuild;
    bool allow_breaking;
} UmiDataSchemaCompatibilityPolicy;

/* Initialise conservative Framework defaults for this policy. */
void umi_data_schema_compatibility_default(UmiDataSchemaCompatibilityPolicy *policy);
/* Evaluate the policy against operation evidence without performing I/O. */
UmiStatus umi_data_schema_compatibility_evaluate(const UmiDataSchemaCompatibilityPolicy *policy, UmiDataCompatibility compatibility, bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
