/* Umicom Framework built-in language definitions. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_LANGUAGE_BUILTINS_H
#define UMICOM_LANGUAGE_BUILTINS_H
#include "umicom/language/definition.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_register_builtin_definitions(UmiLanguageDefinitionRegistry *registry);
size_t umi_language_builtin_definition_count(void);
#ifdef __cplusplus
}
#endif
#endif
