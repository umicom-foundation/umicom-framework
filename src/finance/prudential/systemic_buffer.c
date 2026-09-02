/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/systemic_buffer.c
 *
 * PURPOSE:
 *   Calculate a systemic capital buffer amount from RWA and configured rate.
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

#include "umicom/finance/prudential/systemic_buffer.h"

#include <string.h>
/*
 * Provide the pru systemic buffer calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_pru_systemic_buffer_calculate(UmiPrudentialSystemicBuffer *result,const char *buffer_id,double required_ratio,double rwa) { UmiStatus s; double amount; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||buffer_id==NULL||required_ratio<0.0)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(rwa,required_ratio,&amount); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->buffer_id,sizeof result->buffer_id,buffer_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; result->required_ratio=required_ratio; result->rwa=rwa; result->required_amount=amount; return UMI_STATUS_OK; }
