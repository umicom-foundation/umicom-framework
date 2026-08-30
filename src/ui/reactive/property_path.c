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
    if (item==NULL || text==NULL || text[0]=='\0' || strlen(text)>=sizeof item->text) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item); memcpy(item->text,text,strlen(text)+1U);
    if (text[0]=='.') return UMI_STATUS_PARSE_ERROR;
    for(i=0U;text[i]!='\0';++i){ if(text[i]=='.'){ if(i==0U || text[i-1U]=='.' || text[i+1U]=='\0') return UMI_STATUS_PARSE_ERROR; ++seg; } }
    item->segment_count=seg; return UMI_STATUS_OK;
}
/* Report whether the parsed path contains at least one segment. */
int umi_ui_reactive_property_path_valid(const UmiUiReactivePropertyPath *item){ return item!=NULL && item->text[0]!='\0' && item->segment_count>0U; }
