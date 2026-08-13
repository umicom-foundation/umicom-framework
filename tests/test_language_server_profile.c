/* Umicom Framework language server profile test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/language/server_profile.h"
int main(void){UmiLanguageServerProfileRegistry*r=NULL;UmiLanguageServerProfile p={0},o;strcpy(p.id,"clangd");strcpy(p.executable,"clangd");p.capabilities=UMI_LANGUAGE_CAP_COMPLETION|UMI_LANGUAGE_CAP_HOVER;p.enabled=1;assert(umi_language_server_profile_registry_create(&r)==UMI_STATUS_OK);assert(umi_language_server_profile_registry_upsert(r,&p)==UMI_STATUS_OK);assert(umi_language_server_profile_registry_find(r,"clangd",&o)==UMI_STATUS_OK);assert(umi_language_server_profile_supports(&o,UMI_LANGUAGE_CAP_HOVER));umi_language_server_profile_registry_destroy(r);return 0;}
