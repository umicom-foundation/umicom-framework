/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/property_path.c
 *
 * PURPOSE:
 *   Parse and validate dotted property paths used by declarative bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/property_path.h"
#include <string.h>
/* Parse a dotted property path and count non-empty segments. */
UmiStatus umi_ui_reactive_property_path_parse(UmiUiReactivePropertyPath *item, const char *text) {
    size_t i, seg=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL || text==NULL || text[0]=='\0' || strlen(text)>=sizeof item->text) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item); memcpy(item->text,text,strlen(text)+1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (text[0]=='.') return UMI_STATUS_PARSE_ERROR;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;text[i]!='\0';++i){ /* Preserve the original failure result so the caller can respond to the correct cause. */ if(text[i]=='.'){ /* Preserve the original failure result so the caller can respond to the correct cause. */ if(i==0U || text[i-1U]=='.' || text[i+1U]=='\0') return UMI_STATUS_PARSE_ERROR; ++seg; } }
    item->segment_count=seg; return UMI_STATUS_OK;
}
/* Report whether the parsed path contains at least one segment. */
int umi_ui_reactive_property_path_valid(const UmiUiReactivePropertyPath *item){ return item!=NULL && item->text[0]!='\0' && item->segment_count>0U; }
