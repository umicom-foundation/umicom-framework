/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/headless.c
 *
 * PURPOSE:
 *   Provide a deterministic frontend renderer for tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/headless.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct Headless{char *capture;size_t capacity;}Headless;
static UmiStatus render(void *p,const UmiFrontendPlan *plan,const char *root){Headless *h=(Headless*)p;int n;(void)root;n=snprintf(h->capture,h->capacity,"frontend=%s app=%s pages=%zu routes=%zu assets=%zu",umi_frontend_kind_text(plan->kind),plan->application_id,plan->page_count,plan->route_count,plan->asset_count);return n<0||(size_t)n>=h->capacity?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
static void destroy(void *p){free(p);}
UmiStatus umi_frontend_headless_create(char *capture,size_t cap,UmiFrontendRenderer *out){Headless *h;if(capture==NULL||cap==0U||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;h=(Headless*)calloc(1U,sizeof(*h));if(h==NULL)return UMI_STATUS_OUT_OF_MEMORY;h->capture=capture;h->capacity=cap;out->instance=h;out->kind=UMI_FRONTEND_KIND_HEADLESS;out->render=render;out->destroy=destroy;return UMI_STATUS_OK;}
