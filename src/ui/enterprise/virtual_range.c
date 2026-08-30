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
UmiUiEntSpan umi_ui_ent_virtual_range_intersection(UmiUiEntSpan a,UmiUiEntSpan b){size_t s=a.first>b.first?a.first:b.first;size_t ae=umi_ui_ent_span_end(a),be=umi_ui_ent_span_end(b),e=ae<be?ae:be;return (UmiUiEntSpan){s,e>s?e-s:0U};}
UmiUiEntSpan umi_ui_ent_virtual_range_union(UmiUiEntSpan a,UmiUiEntSpan b){if(a.count==0U)return b;if(b.count==0U)return a;size_t s=a.first<b.first?a.first:b.first;size_t ae=umi_ui_ent_span_end(a),be=umi_ui_ent_span_end(b),e=ae>be?ae:be;return (UmiUiEntSpan){s,e-s};}
UmiUiEntSpan umi_ui_ent_virtual_range_clamp(UmiUiEntSpan v,size_t total){if(v.first>=total)return (UmiUiEntSpan){total,0U};size_t e=umi_ui_ent_span_end(v);if(e>total)e=total;return (UmiUiEntSpan){v.first,e-v.first};}
