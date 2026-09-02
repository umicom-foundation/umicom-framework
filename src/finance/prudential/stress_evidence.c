/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_evidence.c
 *
 * PURPOSE:
 *   Fingerprint immutable stress-test evidence by source and sequence.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_evidence.h"

#include <string.h>
/*
 * Initialise pru stress evidence from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_pru_stress_evidence_init(UmiPrudentialStressEvidence *evidence,uint64_t sequence,const char *source) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(evidence==NULL||source==NULL||sequence==0U)return UMI_STATUS_INVALID_ARGUMENT; memset(evidence,0,sizeof *evidence); s=umi_pru_copy_text(evidence->source,sizeof evidence->source,source); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; evidence->sequence=sequence; evidence->fingerprint=umi_pru_hash_text(source); return evidence->fingerprint==0U?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK; }
