/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/catalogue.c
 *
 * PURPOSE:
 *   Register the initial toolkit-neutral component catalogue used by declarative applications and the visual designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/catalogue.h"

/*
 * Provide the register component operation used by this module and its client
 * applications.
 */
static UmiStatus register_component(UmiDeclComponentRegistry *r,const char *type,const char *name,const char *category,int container){UmiDeclComponentDescriptor d;UmiDeclPropertyDescriptor p;UmiStatus s=umi_decl_component_descriptor_init(&d,type,name,category,container);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;(void)umi_decl_property_descriptor_init(&p,"title",UMI_DECL_VALUE_STRING,"",0);(void)umi_decl_component_add_property(&d,&p);(void)umi_decl_property_descriptor_init(&p,"visible",UMI_DECL_VALUE_BOOLEAN,"true",0);(void)umi_decl_component_add_property(&d,&p);return umi_decl_component_registry_register(r,&d);}
/*
 * Provide the decl catalogue register defaults operation used by this module and its
 * client applications.
 */
UmiStatus umi_decl_catalogue_register_defaults(UmiDeclComponentRegistry *r){static const struct{const char *type,*name,*category;int container;} items[]={
{"window","Window","Containers",1},{"split","Split","Containers",1},{"stack","Stack","Containers",1},{"tabs","Tabs","Containers",1},{"pane","Pane","Containers",1},{"editor","Editor","Development",0},{"tree","Tree","Data",0},{"table","Table","Data",0},{"chart","Chart","Data",0},{"terminal","Terminal","Development",0},{"output","Output","Development",0},{"problems","Problems","Development",0},{"button","Button","Controls",0},{"label","Label","Controls",0},{"text","Text","Controls",0},{"image","Image","Media",0},{"menu","Menu","Navigation",1},{"toolbar","Toolbar","Navigation",1},{"status","Status","Navigation",1},{"browser","Browser","Web",0}};size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<sizeof(items)/sizeof(items[0]);++i){s=register_component(r,items[i].type,items[i].name,items[i].category,items[i].container);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
