/*-----------------------------------------------------------------------------
 * Umicom Framework built-in language catalogue.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/language/builtins.h"
#include <string.h>
typedef struct Builtin {const char*id;const char*name;const char*extensions;const char*mimes;const char*server;const char*formatter;} Builtin;
static const Builtin BUILTINS[]={
 {"c","C",".c;.h","text/x-c;text/x-chdr","clangd","clang-format"},
 {"cpp","C++",".cc;.cpp;.cxx;.hh;.hpp;.hxx","text/x-c++","clangd","clang-format"},
 {"asm","Assembly",".s;.S;.asm","text/x-asm","clangd","clang-format"},
 {"cmake","CMake","CMakeLists.txt;.cmake","text/x-cmake","cmake-language-server","cmake-format"},
 {"python","Python",".py;.pyi","text/x-python","pylsp","black"},
 {"json","JSON",".json;.jsonc","application/json","vscode-json-language-server","prettier"},
 {"yaml","YAML",".yaml;.yml","application/yaml","yaml-language-server","prettier"},
 {"markdown","Markdown",".md;.markdown","text/markdown","marksman","prettier"},
 {"html","HTML",".html;.htm","text/html","vscode-html-language-server","prettier"},
 {"css","CSS",".css;.scss;.less","text/css","vscode-css-language-server","prettier"},
 {"javascript","JavaScript",".js;.mjs;.cjs;.jsx","text/javascript","typescript-language-server","prettier"},
 {"typescript","TypeScript",".ts;.tsx;.mts;.cts","text/typescript","typescript-language-server","prettier"}
};
UmiStatus umi_language_register_builtin_definitions(UmiLanguageDefinitionRegistry*r){size_t i;UmiStatus s;if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<sizeof(BUILTINS)/sizeof(BUILTINS[0]);++i){UmiLanguageDefinitionSnapshot d={0};(void)strcpy(d.id,BUILTINS[i].id);(void)strcpy(d.name,BUILTINS[i].name);(void)strcpy(d.file_extensions,BUILTINS[i].extensions);(void)strcpy(d.mime_types,BUILTINS[i].mimes);(void)strcpy(d.language_server,BUILTINS[i].server);(void)strcpy(d.formatter,BUILTINS[i].formatter);d.enabled=1;s=umi_language_definition_registry_upsert(r,&d);if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
size_t umi_language_builtin_definition_count(void){return sizeof(BUILTINS)/sizeof(BUILTINS[0]);}
