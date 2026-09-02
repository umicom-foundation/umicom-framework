/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/reference.c
 *
 * PURPOSE:
 *   Publish canonical reference profiles for Studio, Designer, Trader, TMS and Media Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/reference.h"
static const UmiProductFrontend dev_frontends[]={{UMI_PRODUCT_FRONTEND_CONSOLE,1},{UMI_PRODUCT_FRONTEND_GTK4,1},{UMI_PRODUCT_FRONTEND_WEB,1}};
static const UmiProductFrontend desktop_frontends[]={{UMI_PRODUCT_FRONTEND_CONSOLE,1},{UMI_PRODUCT_FRONTEND_GTK4,1}};
static const UmiProductCapability studio_caps[]={{"umicom.runtime",1},{"umicom.ui",1},{"umicom.build",1},{"umicom.protocol",1},{"umicom.vcs",1},{"umicom.ai",0}};
static const UmiProductCapability designer_caps[]={{"umicom.runtime",1},{"umicom.ui",1},{"umicom.declarative",1},{"umicom.designer",1}};
static const UmiProductCapability trader_caps[]={{"umicom.runtime",1},{"umicom.finance",1},{"umicom.trading",1},{"umicom.data",1},{"umicom.messaging",1},{"umicom.chart",1},{"umicom.web",0},{"umicom.ai",0}};
static const UmiProductCapability tms_caps[]={{"umicom.runtime",1},{"umicom.data",1},{"umicom.messaging",1},{"umicom.security",1},{"umicom.web",0}};
static const UmiProductCapability media_caps[]={{"umicom.runtime",1},{"umicom.ui",1},{"umicom.ai",1},{"umicom.delivery",1},{"umicom.web",0}};
static const UmiProductFeature studio_features[]={{"editing","Source editing and project development",1},{"build","Build and test orchestration",1}};
static const UmiProductFeature designer_features[]={{"visual-design","Declarative visual application authoring",1}};
static const UmiProductFeature trader_features[]={{"market-analysis","Market data, charting and trading research",1}};
static const UmiProductFeature tms_features[]={{"treasury","Trade lifecycle and treasury operations",1}};
static const UmiProductFeature media_features[]={{"media-ai","AI-assisted video, image and music creation",1}};
static const UmiProductDescriptor studio_desc={"org.umicom.studio",UMI_PRODUCT_DEVELOPMENT,{0U,14U,0U},{"Umicom Studio IDE","Umicom Foundation","umicom-studio-ide"}};
static const UmiProductDescriptor designer_desc={"org.umicom.designer",UMI_PRODUCT_DESIGN,{0U,1U,0U},{"Umicom Designer","Umicom Foundation","umicom-designer"}};
static const UmiProductDescriptor trader_desc={"org.umicom.trader",UMI_PRODUCT_TRADING,{0U,1U,0U},{"Umicom Trader","Umicom Foundation","umicom-trader"}};
static const UmiProductDescriptor tms_desc={"org.umicom.tms",UMI_PRODUCT_TREASURY,{0U,1U,0U},{"Umicom Treasury Management System","Umicom Foundation","umicom-tms"}};
static const UmiProductDescriptor media_desc={"org.umicom.media-studio",UMI_PRODUCT_MEDIA,{0U,1U,0U},{"Umicom Media Studio","Umicom Foundation","umicom-media-studio"}};
static const UmiProductProfile studio={&studio_desc,studio_caps,sizeof(studio_caps)/sizeof(studio_caps[0]),studio_features,1U,dev_frontends,sizeof(dev_frontends)/sizeof(dev_frontends[0])};
static const UmiProductProfile designer={&designer_desc,designer_caps,sizeof(designer_caps)/sizeof(designer_caps[0]),designer_features,1U,desktop_frontends,sizeof(desktop_frontends)/sizeof(desktop_frontends[0])};
static const UmiProductProfile trader={&trader_desc,trader_caps,sizeof(trader_caps)/sizeof(trader_caps[0]),trader_features,1U,dev_frontends,sizeof(dev_frontends)/sizeof(dev_frontends[0])};
static const UmiProductProfile tms={&tms_desc,tms_caps,sizeof(tms_caps)/sizeof(tms_caps[0]),tms_features,1U,dev_frontends,sizeof(dev_frontends)/sizeof(dev_frontends[0])};
static const UmiProductProfile media={&media_desc,media_caps,sizeof(media_caps)/sizeof(media_caps[0]),media_features,1U,dev_frontends,sizeof(dev_frontends)/sizeof(dev_frontends[0])};
/*
 * Provide the reference product studio operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_reference_product_studio(void){return &studio;}
/*
 * Provide the reference product designer operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_reference_product_designer(void){return &designer;}
/*
 * Provide the reference product trader operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_reference_product_trader(void){return &trader;}
/*
 * Provide the reference product tms operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_reference_product_tms(void){return &tms;}
/*
 * Provide the reference product media studio operation used by this module and its client
 * applications.
 */
const UmiProductProfile *umi_reference_product_media_studio(void){return &media;}
