/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/provenance.h
 *
 * PURPOSE:
 *   Record source revision, builder identity and build inputs for release provenance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Provenance links a package back to the exact source and build environment that created it.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PROVENANCE_H
#define INCLUDE_UMICOM_DELIVERY_PROVENANCE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProvenance {
    char source_revision[UMI_DELIVERY_ID_CAPACITY];
    char builder_id[UMI_DELIVERY_ID_CAPACITY];
    char build_preset[UMI_DELIVERY_ID_CAPACITY];
    char framework_version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t created_epoch_ms;
} UmiProvenance;

UmiStatus umi_provenance_init(UmiProvenance *provenance,
                              const char *source_revision,
                              const char *builder_id,
                              const char *build_preset);
UmiStatus umi_provenance_validate(const UmiProvenance *provenance);

#ifdef __cplusplus
}
#endif

#endif
