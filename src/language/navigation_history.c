/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/navigation_history.c
 *
 * PURPOSE:
 *   Implement the navigation history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language navigation history. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/navigation_history.h"
#include <stdlib.h>
struct UmiLanguageNavigationHistory{UmiLanguageNavigationLocation items[UMI_LANGUAGE_NAVIGATION_CAPACITY];size_t count;size_t cursor;uint64_t revision;};
UmiStatus umi_language_navigation_history_create(UmiLanguageNavigationHistory**out){UmiLanguageNavigationHistory*h;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;h=calloc(1U,sizeof(*h));if(h==NULL)return UMI_STATUS_OUT_OF_MEMORY;h->revision=1U;*out=h;return UMI_STATUS_OK;}
void umi_language_navigation_history_destroy(UmiLanguageNavigationHistory*h){free(h);}
UmiStatus umi_language_navigation_history_push(UmiLanguageNavigationHistory*h,const UmiLanguageNavigationLocation*l){if(h==NULL||l==NULL||l->uri[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(h->cursor<h->count)h->count=h->cursor;if(h->count>=UMI_LANGUAGE_NAVIGATION_CAPACITY){size_t i;for(i=1U;i<h->count;++i)h->items[i-1U]=h->items[i];h->count-=1U;if(h->cursor>0U)h->cursor-=1U;}h->items[h->count]=*l;h->items[h->count].uri[1023U]='\0';h->items[h->count].symbol[255U]='\0';h->revision+=1U;h->items[h->count].revision=h->revision;h->count+=1U;h->cursor=h->count;return UMI_STATUS_OK;}
UmiStatus umi_language_navigation_history_back(UmiLanguageNavigationHistory*h,UmiLanguageNavigationLocation*out){if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->cursor<=1U)return UMI_STATUS_NOT_FOUND;h->cursor-=1U;*out=h->items[h->cursor-1U];return UMI_STATUS_OK;}
UmiStatus umi_language_navigation_history_forward(UmiLanguageNavigationHistory*h,UmiLanguageNavigationLocation*out){if(h==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->cursor>=h->count)return UMI_STATUS_NOT_FOUND;*out=h->items[h->cursor];h->cursor+=1U;return UMI_STATUS_OK;}
size_t umi_language_navigation_history_count(const UmiLanguageNavigationHistory*h){return h!=NULL?h->count:0U;}
