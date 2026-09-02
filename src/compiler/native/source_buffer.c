/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/source_buffer.c
 *
 * PURPOSE:
 *   Own a bounded source snapshot with deterministic revision and line-count tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/source_buffer.h"
#include <string.h>
/*
 * Initialise nc source buffer from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_source_buffer_init(UmiNativeSourceBuffer *buffer){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer!=NULL) memset(buffer,0,sizeof(*buffer)); }
/*
 * Copy nc source buffer into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_nc_source_buffer_set(UmiNativeSourceBuffer *buffer,const char *path,const char *text){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer==NULL||path==NULL||text==NULL) return UMI_STATUS_INVALID_ARGUMENT; size_t n=strlen(text); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>=sizeof(buffer->text)) return UMI_STATUS_CAPACITY_EXCEEDED; UmiNativeSourceBuffer next; umi_nc_source_buffer_init(&next); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(next.path,sizeof(next.path),path)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; memcpy(next.text,text,n+1U); next.length=n; next.revision=buffer->revision+1U; *buffer=next; return UMI_STATUS_OK; }
/* Add nc source buffer only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_source_buffer_append(UmiNativeSourceBuffer *buffer,const char *text){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer==NULL||text==NULL) return UMI_STATUS_INVALID_ARGUMENT; size_t n=strlen(text); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer->length+n>=sizeof(buffer->text)) return UMI_STATUS_CAPACITY_EXCEEDED; memcpy(buffer->text+buffer->length,text,n+1U); buffer->length+=n; buffer->revision++; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by nc source buffer line without changing their
 * state.
 */
size_t umi_nc_source_buffer_line_count(const UmiNativeSourceBuffer *buffer){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer==NULL||buffer->length==0U) return 0U; size_t lines=1U; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<buffer->length;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer->text[i]=='\n') lines++; return lines; }
/*
 * Provide the nc source buffer fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_source_buffer_fingerprint(const UmiNativeSourceBuffer *buffer){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(buffer==NULL) return 0U; uint64_t h=umi_nc_hash_text(buffer->path); h^=umi_nc_hash_bytes(buffer->text,buffer->length)+(h<<6U)+(h>>2U); return h; }
