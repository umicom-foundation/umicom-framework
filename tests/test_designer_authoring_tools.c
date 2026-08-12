/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_authoring_tools.c
 *
 * PURPOSE:
 *   Exercise Batch 25 reusable platform contracts with deterministic smoke coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/designer/property_schema.h"
#include "umicom/designer/signal_binding.h"
int main(void){UmiDesignerPropertySchemaRegistry*p=NULL;UmiDesignerPropertySchemaSnapshot x={0};UmiDesignerSignalBindingRegistry*s=NULL;UmiDesignerSignalBindingSnapshot y={0};if(umi_designer_property_schema_registry_create(&p)!=UMI_STATUS_OK)return 1;strcpy(x.id,"button.text");strcpy(x.component_type,"button");strcpy(x.property_name,"text");if(umi_designer_property_schema_registry_upsert(p,&x)!=UMI_STATUS_OK)return 2;if(umi_designer_signal_binding_registry_create(&s)!=UMI_STATUS_OK)return 3;strcpy(y.id,"button.clicked");strcpy(y.node_id,"button");strcpy(y.signal_name,"clicked");if(umi_designer_signal_binding_registry_upsert(s,&y)!=UMI_STATUS_OK)return 4;umi_designer_signal_binding_registry_destroy(s);umi_designer_property_schema_registry_destroy(p);return 0;}
