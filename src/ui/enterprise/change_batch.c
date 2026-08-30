/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/change_batch.c
 *
 * PURPOSE:
 *   Implement monotonic bounded change batches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/change_batch.h"
#include <string.h>
void umi_ui_ent_change_batch_init(UmiUiEntChangeBatch *b){if(b)memset(b,0,sizeof *b);}
UmiStatus umi_ui_ent_change_batch_add(UmiUiEntChangeBatch *b,const UmiUiEntDataDelta *d){if(!b||!umi_ui_ent_data_delta_validate(d)||d->sequence==0U)return UMI_STATUS_INVALID_ARGUMENT;if(b->count&&d->sequence<=b->last_sequence)return UMI_STATUS_INVALID_STATE;if(b->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;if(b->count==0U)b->first_sequence=d->sequence;b->items[b->count++]=*d;b->last_sequence=d->sequence;return UMI_STATUS_OK;}
