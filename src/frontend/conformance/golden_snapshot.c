/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/golden_snapshot.c
 *
 * PURPOSE:
 *   versioned golden semantic snapshot metadata and acceptance rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/golden_snapshot.h"

UmiStatus umi_fc_golden_snapshot_make(const char *id,uint32_t schema_version,uint64_t fingerprint,double minimum_similarity,UmiFcGoldenSnapshot *out_golden){UmiStatus st;if(out_golden==NULL||schema_version==0U)return UMI_STATUS_INVALID_ARGUMENT;*out_golden=(UmiFcGoldenSnapshot){0};st=umi_fc_copy_text(out_golden->id,sizeof(out_golden->id),id);if(st!=UMI_STATUS_OK)return st;out_golden->schema_version=schema_version;out_golden->fingerprint=fingerprint;out_golden->minimum_similarity=umi_fc_clamp_score(minimum_similarity);return UMI_STATUS_OK;}
bool umi_fc_golden_snapshot_accepts(const UmiFcGoldenSnapshot *golden,uint64_t fingerprint,double similarity){if(golden==NULL)return false;return fingerprint==golden->fingerprint||similarity>=golden->minimum_similarity;}
