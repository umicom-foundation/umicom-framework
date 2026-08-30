/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/schema_version.h
 *
 * PURPOSE:
 *   Declare storage and retrieval of the current Data Server schema version independently from any particular database adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_SCHEMA_VERSION_H
#define UMICOM_DATA_SCHEMA_VERSION_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/store.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_schema_version_get(const UmiStore *store,
                                 uint32_t *out_version);
UmiStatus umi_schema_version_set(UmiStore *store, uint32_t version);

#ifdef __cplusplus
}
#endif

#endif
