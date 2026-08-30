/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_fingerprint.h
 *
 * PURPOSE:
 *   Produce deterministic schema fingerprints for migration checkpoints, caches and release evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_FINGERPRINT_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_FINGERPRINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/schema_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Fingerprint a schema snapshot using stable identifiers and portable column metadata. */
uint64_t umi_data_schema_fingerprint_compute(const UmiDataSchemaSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
