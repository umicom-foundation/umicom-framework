/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/random_sequence.c
 *
 * PURPOSE:
 *   Generate deterministic reproducible uniform pseudo-random sequences.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/random_sequence.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant random sequence from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_quant_random_sequence_init(UmiQuantRandomSequence *sequence,uint64_t seed){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL||seed==0U)return UMI_STATUS_INVALID_ARGUMENT;sequence->state=seed;return UMI_STATUS_OK;}
/*
 * Provide the quant random sequence next operation used by this module and its client
 * applications.
 */
double umi_quant_random_sequence_next(UmiQuantRandomSequence *sequence){uint64_t x;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL||sequence->state==0U)return 0.0;x=sequence->state;x^=x>>12;x^=x<<25;x^=x>>27;sequence->state=x;x*=UINT64_C(2685821657736338717);return (double)(x>>11)*(1.0/9007199254740992.0);}
