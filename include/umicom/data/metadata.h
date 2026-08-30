/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/metadata.h
 *
 * PURPOSE:
 *   Define persistent metadata describing record type, version, source and creation/update timestamps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_METADATA_H
#define UMICOM_DATA_METADATA_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/repository.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiMetadata {
    char record_id[128];
    char record_type[128];
    uint32_t version;
    char source[160];
    uint64_t created_at_nanoseconds;
    uint64_t updated_at_nanoseconds;
} UmiMetadata;

UmiStatus umi_metadata_save(UmiRepository *repository,
                            const UmiMetadata *metadata);
UmiStatus umi_metadata_load(const UmiRepository *repository,
                            const char *record_id,
                            UmiMetadata *out_metadata);

#ifdef __cplusplus
}
#endif

#endif
