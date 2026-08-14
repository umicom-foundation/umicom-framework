/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/renderer.c
 *
 * PURPOSE:
 *   Drive frontend adapters from one immutable application plan and provide a deterministic textual renderer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/renderer.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_decl_render(const UmiDeclApplicationPlan *p,const UmiDeclRenderer *r){size_t i;UmiStatus s;if(p==NULL||r==NULL||r->begin==NULL||r->component==NULL||r->end==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=r->begin(r->instance,p);for(i=0U;s==UMI_STATUS_OK&&i<p->component_count;++i)s=r->component(r->instance,&p->components[i]);if(s==UMI_STATUS_OK)s=r->end(r->instance);return s;}
UmiStatus umi_decl_headless_render(const UmiDeclApplicationPlan *p,char *out,size_t cap){size_t i,used=0U;if(p==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;out[0]='\0';for(i=0U;i<p->component_count;++i){char line[512];int n=snprintf(line,sizeof(line),"%s:%s parent=%s\n",p->components[i].node_id,p->components[i].component_type,p->components[i].parent_id[0]?p->components[i].parent_id:"-");if(n<0||(size_t)n+used+1U>cap)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out+used,line,(size_t)n);used+=(size_t)n;out[used]='\0';}return UMI_STATUS_OK;}
