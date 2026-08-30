/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/types.c
 *
 * PURPOSE:
 *   Implement shared enterprise-view identifier and range helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/types.h"
#include <string.h>
UmiStatus umi_ui_ent_copy_text(char *dst, size_t cap, const char *src){size_t n;if(!dst||cap==0U||!src)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(src);if(n>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(dst,src,n+1U);return UMI_STATUS_OK;}
int umi_ui_ent_id_valid(const char *id){return id&&id[0]!='\0';}
size_t umi_ui_ent_span_end(UmiUiEntSpan span){if(span.count>SIZE_MAX-span.first)return SIZE_MAX;return span.first+span.count;}
int umi_ui_ent_span_contains(UmiUiEntSpan span,size_t index){size_t end=umi_ui_ent_span_end(span);return index>=span.first&&index<end;}
size_t umi_ui_ent_clamp_size(size_t value,size_t minimum,size_t maximum){if(maximum<minimum)return minimum;if(value<minimum)return minimum;if(value>maximum)return maximum;return value;}
