/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/loop_prevention.c
 *
 * PURPOSE:
 *   Remember recent propagation fingerprints and reject immediate loops.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/loop_prevention.h"
#include <string.h>
/* Initialise recent-propagation fingerprint memory. */
void umi_ui_reactive_loop_prevention_init(UmiUiReactiveLoopPrevention *item){if(item)memset(item,0,sizeof *item);}
/* Remember a fingerprint; return ALREADY_EXISTS when it would immediately loop. */
UmiStatus umi_ui_reactive_loop_prevention_remember(UmiUiReactiveLoopPrevention *item,uint64_t fp){size_t i;if(!item)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<item->count;i++)if(item->fingerprints[i]==fp)return UMI_STATUS_ALREADY_EXISTS;if(item->count<64U)item->fingerprints[item->count++]=fp;else{memmove(&item->fingerprints[0],&item->fingerprints[1],63U*sizeof(uint64_t));item->fingerprints[63]=fp;}return UMI_STATUS_OK;}
