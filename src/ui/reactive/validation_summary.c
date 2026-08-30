/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_summary.c
 *
 * PURPOSE:
 *   Summarise validation counts into a stable health score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_summary.h"
#include <string.h>
/* Initialise an empty validation summary. */
void umi_ui_reactive_validation_summary_init(UmiUiReactiveValidationSummary *item){if(item)memset(item,0,sizeof *item);}
/* Compute a 0..100 validation health score weighted toward blocking errors. */
unsigned umi_ui_reactive_validation_summary_score(const UmiUiReactiveValidationSummary *item){ size_t total; unsigned penalty; if(!item)return 0U;total=item->valid_count+item->warning_count+item->error_count;if(total==0U)return 100U;penalty=(unsigned)((item->warning_count*25U+item->error_count*100U)/total);return penalty>=100U?0U:100U-penalty; }
