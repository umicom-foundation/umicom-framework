/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/builtins.c
 *
 * PURPOSE:
 *   Implement the builtins behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
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
 {"assembly","Assembly",".s;.S;.asm","text/x-asm","clangd","clang-format"},
 {"cmake","CMake","CMakeLists.txt;.cmake","text/x-cmake","cmake-language-server","cmake-format"},
 {"python","Python",".py;.pyi","text/x-python","pylsp","black"},
 {"json","JSON",".json;.jsonc","application/json","vscode-json-language-server","prettier"},
 {"yaml","YAML",".yaml;.yml","application/yaml","yaml-language-server","prettier"},
 {"markdown","Markdown",".md;.markdown","text/markdown","marksman","prettier"},
 {"html","HTML",".html;.htm","text/html","vscode-html-language-server","prettier"},
 {"css","CSS",".css;.scss;.less","text/css","vscode-css-language-server","prettier"},
 {"javascript","JavaScript",".js;.mjs;.cjs;.jsx","text/javascript","typescript-language-server","prettier"},
 {"typescript","TypeScript",".ts;.tsx;.mts;.cts","text/typescript","typescript-language-server","prettier"},
 {"rust","Rust",".rs","text/rust","rust-analyzer","rustfmt"},
 {"zig","Zig",".zig;.zon","text/zig","zls","zig fmt"},
 {"java","Java",".java","text/x-java-source","jdtls","google-java-format"},
 {"php","PHP",".php;.phtml","application/x-httpd-php","intelephense","php-cs-fixer"},
 {"bash","Bash",".sh;.bash","application/x-sh","bash-language-server","shfmt"},
 {"powershell","PowerShell",".ps1;.psm1;.psd1","text/x-powershell","PowerShellEditorServices","Invoke-Formatter"},
 {"toml","TOML",".toml","application/toml","taplo","taplo fmt"},
 {"make","Make",".mk;.mak","text/x-makefile","",""},
 {"umicc","Umicc",".umc;.umicc","text/x-umicc","umicc-lsp","umicc format"},
 {"uai","Umicom AI Language",".uai","text/x-uai","uai-lsp","uai format"}
};
/*
 * Provide the language register builtin definitions operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_register_builtin_definitions(UmiLanguageDefinitionRegistry*r){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<sizeof(BUILTINS)/sizeof(BUILTINS[0]);++i){UmiLanguageDefinitionSnapshot d={0};(void)strcpy(d.id,BUILTINS[i].id);(void)strcpy(d.name,BUILTINS[i].name);(void)strcpy(d.file_extensions,BUILTINS[i].extensions);(void)strcpy(d.mime_types,BUILTINS[i].mimes);(void)strcpy(d.language_server,BUILTINS[i].server);(void)strcpy(d.formatter,BUILTINS[i].formatter);d.enabled=1;s=umi_language_definition_registry_upsert(r,&d);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
/*
 * Return the number of records represented by language builtin definition without changing
 * their state.
 */
size_t umi_language_builtin_definition_count(void){return sizeof(BUILTINS)/sizeof(BUILTINS[0]);}
