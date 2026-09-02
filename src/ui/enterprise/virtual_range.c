/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/virtual_range.c
 *
 * PURPOSE:
 *   Implement overflow-safe virtual range algebra.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/virtual_range.h"
/*
 * Provide the ui ent virtual range intersection operation used by this module and its
 * client applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_intersection(UmiUiEntSpan a,UmiUiEntSpan b){size_t s=a.first>b.first?a.first:b.first;size_t ae=umi_ui_ent_span_end(a),be=umi_ui_ent_span_end(b),e=ae<be?ae:be;return (UmiUiEntSpan){s,e>s?e-s:0U};}
/*
 * Provide the ui ent virtual range union operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_union(UmiUiEntSpan a,UmiUiEntSpan b){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(a.count==0U)return b;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(b.count==0U)return a;size_t s=a.first<b.first?a.first:b.first;size_t ae=umi_ui_ent_span_end(a),be=umi_ui_ent_span_end(b),e=ae>be?ae:be;return (UmiUiEntSpan){s,e-s};}
/*
 * Provide the ui ent virtual range clamp operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_clamp(UmiUiEntSpan v,size_t total){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v.first>=total)return (UmiUiEntSpan){total,0U};size_t e=umi_ui_ent_span_end(v);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(e>total)e=total;return (UmiUiEntSpan){v.first,e-v.first};}
