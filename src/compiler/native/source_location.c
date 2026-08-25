/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/source_location.c
 *
 * PURPOSE:
 *   Provide source-span construction, advancement and containment helpers for diagnostics and syntax nodes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_location.h"
UmiNativeSourceSpan umi_nc_source_span_make(uint32_t file_id,size_t offset,size_t length,uint32_t line,uint32_t column){ UmiNativeSourceSpan s={file_id,offset,length,line,column}; return s; }
UmiNativeSourceSpan umi_nc_source_span_merge(UmiNativeSourceSpan left,UmiNativeSourceSpan right){ if(left.file_id!=right.file_id) return left; size_t start=left.offset<right.offset?left.offset:right.offset; size_t left_end=left.offset+left.length; size_t right_end=right.offset+right.length; size_t end=left_end>right_end?left_end:right_end; UmiNativeSourceSpan out=left; out.offset=start; out.length=end-start; if(right.offset<left.offset){out.line=right.line;out.column=right.column;} return out; }
bool umi_nc_source_span_contains(const UmiNativeSourceSpan *span,size_t offset){ return span!=NULL && offset>=span->offset && offset<span->offset+span->length; }
UmiNativeSourceSpan umi_nc_source_span_advance(UmiNativeSourceSpan span,char character){ span.offset++; span.length=0U; if(character=='\n'){span.line++;span.column=1U;}else span.column++; return span; }
