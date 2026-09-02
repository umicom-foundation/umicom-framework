/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/source_location.c
 *
 * PURPOSE:
 *   Provide source-span construction, advancement and containment helpers for diagnostics and syntax nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_location.h"
/*
 * Provide the nc source span make operation used by this module and its client
 * applications.
 */
UmiNativeSourceSpan umi_nc_source_span_make(uint32_t file_id,size_t offset,size_t length,uint32_t line,uint32_t column){ UmiNativeSourceSpan s={file_id,offset,length,line,column}; return s; }
/*
 * Provide the nc source span merge operation used by this module and its client
 * applications.
 */
UmiNativeSourceSpan umi_nc_source_span_merge(UmiNativeSourceSpan left,UmiNativeSourceSpan right){ /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(left.file_id!=right.file_id) return left; size_t start=left.offset<right.offset?left.offset:right.offset; size_t left_end=left.offset+left.length; size_t right_end=right.offset+right.length; size_t end=left_end>right_end?left_end:right_end; UmiNativeSourceSpan out=left; out.offset=start; out.length=end-start; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(right.offset<left.offset){out.line=right.line;out.column=right.column;} return out; }
/*
 * Provide the nc source span contains operation used by this module and its client
 * applications.
 */
bool umi_nc_source_span_contains(const UmiNativeSourceSpan *span,size_t offset){ return span!=NULL && offset>=span->offset && offset<span->offset+span->length; }
/*
 * Provide the nc source span advance operation used by this module and its client
 * applications.
 */
UmiNativeSourceSpan umi_nc_source_span_advance(UmiNativeSourceSpan span,char character){ span.offset++; span.length=0U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(character=='\n'){span.line++;span.column=1U;}/* Use this fallback path when the earlier condition does not apply. */ else span.column++; return span; }
