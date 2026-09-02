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
/* Provide the ui ent copy text operation used by this module and its client applications. */
UmiStatus umi_ui_ent_copy_text(char *dst, size_t cap, const char *src){size_t n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!dst||cap==0U||!src)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(src);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(dst,src,n+1U);return UMI_STATUS_OK;}
/* Check that ui ent id satisfies its contract before another service relies on it. */
int umi_ui_ent_id_valid(const char *id){return id&&id[0]!='\0';}
/* Provide the ui ent span end operation used by this module and its client applications. */
size_t umi_ui_ent_span_end(UmiUiEntSpan span){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(span.count>SIZE_MAX-span.first)return SIZE_MAX;return span.first+span.count;}
/*
 * Provide the ui ent span contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_span_contains(UmiUiEntSpan span,size_t index){size_t end=umi_ui_ent_span_end(span);return index>=span.first&&index<end;}
/* Return the number of records represented by ui ent clamp without changing their state. */
size_t umi_ui_ent_clamp_size(size_t value,size_t minimum,size_t maximum){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(maximum<minimum)return minimum;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value<minimum)return minimum;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value>maximum)return maximum;return value;}
