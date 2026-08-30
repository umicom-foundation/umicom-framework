/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/golden_snapshot.h
 *
 * PURPOSE:
 *   versioned golden semantic snapshot metadata and acceptance rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_GOLDEN_SNAPSHOT_H
#define UMICOM_FRONTEND_CONFORMANCE_GOLDEN_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcGoldenSnapshot { char id[UMI_FC_ID_CAPACITY]; uint32_t schema_version; uint64_t fingerprint; double minimum_similarity; } UmiFcGoldenSnapshot;
UmiStatus umi_fc_golden_snapshot_make(const char *id,uint32_t schema_version,uint64_t fingerprint,double minimum_similarity,UmiFcGoldenSnapshot *out_golden);
bool umi_fc_golden_snapshot_accepts(const UmiFcGoldenSnapshot *golden,uint64_t fingerprint,double similarity);

#ifdef __cplusplus
}
#endif
#endif
