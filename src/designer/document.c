/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/document.c
 *
 * PURPOSE:
 *   Implement designer document ownership while keeping the declarative semantic tree authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/document.h"
#include <stdlib.h>
#include <string.h>
struct UmiDesignerDocument{UmiDeclDocument *document;int owns_document;uint64_t revision;int dirty;};
/*
 * Initialise designer document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_document_create(const char *id,UmiDesignerDocument **out){UmiDeclDocument *doc=NULL;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_create(id,&doc);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_designer_document_from_declarative(doc,1,out);}
/*
 * Provide the designer document from declarative operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_document_from_declarative(UmiDeclDocument *doc,int own,UmiDesignerDocument **out){UmiDesignerDocument *d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(doc==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;d=calloc(1U,sizeof(*d));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL)return UMI_STATUS_OUT_OF_MEMORY;d->document=doc;d->owns_document=own!=0;d->revision=1U;*out=d;return UMI_STATUS_OK;}
/*
 * Release or reset state held by designer document so the same storage can be reused
 * safely.
 */
void umi_designer_document_destroy(UmiDesignerDocument *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL)return;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->owns_document)umi_decl_document_destroy(d->document);free(d);}
/*
 * Provide the designer document declarative operation used by this module and its client
 * applications.
 */
UmiDeclDocument *umi_designer_document_declarative(UmiDesignerDocument *d){return d!=NULL?d->document:NULL;}
/*
 * Provide the designer document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_document_snapshot(const UmiDesignerDocument *d,UmiDesignerDocumentSnapshot *out){UmiDeclDocumentSnapshot ds;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_decl_document_snapshot(d->document,&ds)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;(void)memset(out,0,sizeof(*out));(void)umi_decl_copy_text(out->application_id,sizeof(out->application_id),ds.application_id);out->component_count=ds.node_count;out->revision=d->revision;out->dirty=d->dirty;return UMI_STATUS_OK;}
/*
 * Provide the designer document mark saved operation used by this module and its client
 * applications.
 */
void umi_designer_document_mark_saved(UmiDesignerDocument *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d!=NULL)d->dirty=0;}
/*
 * Provide the designer document mark changed operation used by this module and its client
 * applications.
 */
void umi_designer_document_mark_changed(UmiDesignerDocument *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d!=NULL){d->dirty=1;d->revision+=1U;}}
