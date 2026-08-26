/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/countercyclical_buffer.c
 *
 * PURPOSE:
 *   Calculate a countercyclical capital buffer amount from RWA and configured rate.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/countercyclical_buffer.h"

#include <string.h>
UmiStatus umi_pru_countercyclical_buffer_calculate(UmiPrudentialCountercyclicalBuffer *result,const char *buffer_id,double required_ratio,double rwa) { UmiStatus s; double amount; if(result==NULL||buffer_id==NULL||required_ratio<0.0)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(rwa,required_ratio,&amount); if(s!=UMI_STATUS_OK)return s; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->buffer_id,sizeof result->buffer_id,buffer_id); if(s!=UMI_STATUS_OK)return s; result->required_ratio=required_ratio; result->rwa=rwa; result->required_amount=amount; return UMI_STATUS_OK; }
