/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
int main(void){UmiFrontendBuildProfiles p;UmiFrontendBuildProfile v={0},out;(void)strcpy(v.id,"release");(void)strcpy(v.name,"Release");(void)strcpy(v.entry_file,"web/index.html");v.production=1;v.minify=1;assert(umi_frontend_build_profiles_init(&p)==UMI_STATUS_OK);assert(umi_frontend_build_profiles_add(&p,&v)==UMI_STATUS_OK);assert(umi_frontend_build_profiles_activate(&p,"release")==UMI_STATUS_OK);assert(umi_frontend_build_profiles_active(&p,&out)==UMI_STATUS_OK&&out.production&&out.minify);return 0;}
