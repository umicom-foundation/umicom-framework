/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/loop_prevention.c
 *
 * PURPOSE:
 *   Remember recent propagation fingerprints and reject immediate loops.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/loop_prevention.h"
#include <string.h>
/* Initialise recent-propagation fingerprint memory. */
void umi_ui_reactive_loop_prevention_init(UmiUiReactiveLoopPrevention *item){/* Apply this operation only while the related capability or state is available. */ if(item)memset(item,0,sizeof *item);}
/* Remember a fingerprint; return ALREADY_EXISTS when it would immediately loop. */
UmiStatus umi_ui_reactive_loop_prevention_remember(UmiUiReactiveLoopPrevention *item,uint64_t fp){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!item)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<item->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(item->fingerprints[i]==fp)return UMI_STATUS_ALREADY_EXISTS;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(item->count<64U)item->fingerprints[item->count++]=fp;/* Use this fallback path when the earlier condition does not apply. */ else{memmove(&item->fingerprints[0],&item->fingerprints[1],63U*sizeof(uint64_t));item->fingerprints[63]=fp;}return UMI_STATUS_OK;}
