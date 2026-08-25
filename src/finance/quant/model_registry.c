/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/model_registry.c
 *
 * PURPOSE:
 *   Register bounded named quantitative models with deterministic priorities.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/model_registry.h"

#include <math.h>
#include <string.h>

void umi_quant_model_registry_init(UmiQuantModelRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
UmiStatus umi_quant_model_registry_add(UmiQuantModelRegistry *registry,const char *name,UmiQuantModelKind kind,int32_t priority,int enabled){size_t i;UmiStatus status;if(registry==NULL||name==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(registry->count>=UMI_QUANT_MAX_MODELS)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0U;i<registry->count;++i)if(strcmp(registry->models[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS;status=umi_quant_copy_text(registry->models[registry->count].name,sizeof registry->models[registry->count].name,name);if(status!=UMI_STATUS_OK)return status;registry->models[registry->count].kind=kind;registry->models[registry->count].priority=priority;registry->models[registry->count].enabled=enabled?1:0;registry->count++;return UMI_STATUS_OK;}
UmiStatus umi_quant_model_registry_select(const UmiQuantModelRegistry *registry,UmiQuantModelKind kind,const UmiQuantModelDescriptor **out_model){size_t i;const UmiQuantModelDescriptor *best=NULL;if(registry==NULL||out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<registry->count;++i)if(registry->models[i].enabled&&registry->models[i].kind==kind&&(best==NULL||registry->models[i].priority>best->priority))best=&registry->models[i];if(best==NULL)return UMI_STATUS_NOT_FOUND;*out_model=best;return UMI_STATUS_OK;}
