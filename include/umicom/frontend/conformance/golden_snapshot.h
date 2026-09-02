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

/**
 * Represent the fc golden snapshot data shared with callers of this public contract.
 */
typedef struct UmiFcGoldenSnapshot { char id[UMI_FC_ID_CAPACITY]; uint32_t schema_version; uint64_t fingerprint; double minimum_similarity; } UmiFcGoldenSnapshot;
/**
 * Provide the fc golden snapshot make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_golden_snapshot_make(const char *id,uint32_t schema_version,uint64_t fingerprint,double minimum_similarity,UmiFcGoldenSnapshot *out_golden);
/**
 * Provide the fc golden snapshot accepts operation used by this module and its client
 * applications.
 */
bool umi_fc_golden_snapshot_accepts(const UmiFcGoldenSnapshot *golden,uint64_t fingerprint,double similarity);

#ifdef __cplusplus
}
#endif
#endif
