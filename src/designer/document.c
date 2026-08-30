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

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/document.h"
#include <stdlib.h>
#include <string.h>
struct UmiDesignerDocument{UmiDeclDocument *document;int owns_document;uint64_t revision;int dirty;};
UmiStatus umi_designer_document_create(const char *id,UmiDesignerDocument **out){UmiDeclDocument *doc=NULL;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_create(id,&doc);if(s!=UMI_STATUS_OK)return s;return umi_designer_document_from_declarative(doc,1,out);}
UmiStatus umi_designer_document_from_declarative(UmiDeclDocument *doc,int own,UmiDesignerDocument **out){UmiDesignerDocument *d;if(doc==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;d=calloc(1U,sizeof(*d));if(d==NULL)return UMI_STATUS_OUT_OF_MEMORY;d->document=doc;d->owns_document=own!=0;d->revision=1U;*out=d;return UMI_STATUS_OK;}
void umi_designer_document_destroy(UmiDesignerDocument *d){if(d==NULL)return;if(d->owns_document)umi_decl_document_destroy(d->document);free(d);}
UmiDeclDocument *umi_designer_document_declarative(UmiDesignerDocument *d){return d!=NULL?d->document:NULL;}
UmiStatus umi_designer_document_snapshot(const UmiDesignerDocument *d,UmiDesignerDocumentSnapshot *out){UmiDeclDocumentSnapshot ds;if(d==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_decl_document_snapshot(d->document,&ds)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;(void)memset(out,0,sizeof(*out));(void)umi_decl_copy_text(out->application_id,sizeof(out->application_id),ds.application_id);out->component_count=ds.node_count;out->revision=d->revision;out->dirty=d->dirty;return UMI_STATUS_OK;}
void umi_designer_document_mark_saved(UmiDesignerDocument *d){if(d!=NULL)d->dirty=0;}
void umi_designer_document_mark_changed(UmiDesignerDocument *d){if(d!=NULL){d->dirty=1;d->revision+=1U;}}
