/* Umicom Framework built-in language test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include "umicom/language/language.h"
int main(void){UmiLanguageService*s=NULL;UmiLanguageServiceSnapshot snap;assert(umi_language_service_create(&s)==UMI_STATUS_OK);assert(umi_language_service_snapshot(s,&snap)==UMI_STATUS_OK);assert(snap.definition_count==umi_language_builtin_definition_count());umi_language_service_destroy(s);return 0;}
