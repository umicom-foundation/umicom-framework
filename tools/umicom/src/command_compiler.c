/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_compiler.c
 *
 * PURPOSE:
 *   Present shared compiler plans as JSON without discovering or executing a toolchain.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/compiler/compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Help now describes explicit PIC intent and its admitted action families. The previous implementation is retained for review. */
#if 0
static int CompilerHelp(int code)
{
    FILE *stream = code == 0 ? stdout : stderr;
    (void)fputs("Usage: umicom compiler plan --provider gcc|clang|msvc --executable PATH\n"
        "  --language c|cpp --action compile|check --standard NAME --source PATH\n"
        "  --working-directory PATH [--output PATH] [--include PATH ...]\n"
        "  [--define TEXT ...] [--warnings-as-errors] [--debug]\n"
        "Compile requires --output. Check refuses --output and --debug.\n"
        "Options use separate tokens. Each singleton may occur only once.\n"
        "Produces UTF-8 JSON on stdout. Does not read files, find a compiler,\n"
        "run a process, build a project or confirm compiler/standard support.\n"
        "No apply/run mode or raw driver options are accepted. Exit 0: preview;\n"
        "2: input, planning, allocation or output failure. See the compiler plan guide.\n",stream);
    return code;
}
#endif
static int CompilerHelp(int code)
{
    FILE *stream = code == 0 ? stdout : stderr;
    (void)fputs("Usage: umicom compiler plan --provider gcc|clang|msvc --executable PATH\n"
        "  --language c|cpp --action compile|check --standard NAME --source PATH\n"
        "  --working-directory PATH [--output PATH] [--include PATH ...]\n"
        "  [--define TEXT ...] [--warnings-as-errors] [--debug] [--position-independent]\n"
        "Compile requires --output. Check refuses --output, --debug and --position-independent.\n"
        "Position-independent code is mapped only for GCC/Clang C/C++ compile plans.\n"
        "Options use separate tokens. Each singleton may occur only once.\n"
        "Produces UTF-8 JSON on stdout. Does not read files, find a compiler,\n"
        "run a process, build a project or confirm compiler/standard support.\n"
        "No apply/run mode or raw driver options are accepted. Exit 0: preview;\n"
        "2: input, planning, allocation or output failure. See the compiler plan guide.\n",stream);
    return code;
}

static bool SetOnce(const char **field,const char *value)
{
    if (*field != NULL) return false;
    *field = value;
    return true;
}

static bool CopyField(char *destination,size_t capacity,const char *text)
{
    /* The CLI maps text fields only. Framework owns the actual request and plan
     * validation; this bounded copy never silently shortens a supplied option. */
    if (text == NULL) return false;
    for (size_t i = 0U; i < capacity; ++i) {
        if (text[i] == '\0') {
            (void)memcpy(destination,text,i + 1U);
            return true;
        }
    }
    return false;
}

/* The thin CLI projects one additional boolean into the shared Framework request. The previous implementation is retained for review. */
#if 0
int umi_cli_command_compiler(int argc,char **argv)
{
    if (argc < 0 || argc > 1024 || (argc != 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (strcmp(argv[0],"--help") == 0 || strcmp(argv[0],"help") == 0))
        return CompilerHelp(0);
    if (argc == 2 && strcmp(argv[0],"plan") == 0 && strcmp(argv[1],"--help") == 0)
        return CompilerHelp(0);
    if (argc < 1 || strcmp(argv[0],"plan") != 0) return CompilerHelp(2);
    const char *family = NULL, *driver = NULL, *language = NULL, *action = NULL;
    const char *standard = NULL, *source = NULL, *directory = NULL, *output = NULL;
    const char *includes[UMI_COMPILER_MAX_OPTIONS], *definitions[UMI_COMPILER_MAX_OPTIONS];
    size_t include_count = 0U, definition_count = 0U;
    bool debug = false, warnings = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i],"--debug") == 0) {
            if (debug) return CompilerHelp(2);
            debug = true; continue;
        }
        if (strcmp(argv[i],"--warnings-as-errors") == 0) {
            if (warnings) return CompilerHelp(2);
            warnings = true; continue;
        }
        const char *option = argv[i];
        if (++i >= argc || strncmp(argv[i],"--",2U) == 0) return CompilerHelp(2);
        const char *value = argv[i];
        bool accepted = false;
        if (strcmp(option,"--provider") == 0) accepted = SetOnce(&family,value);
        else if (strcmp(option,"--executable") == 0) accepted = SetOnce(&driver,value);
        else if (strcmp(option,"--language") == 0) accepted = SetOnce(&language,value);
        else if (strcmp(option,"--action") == 0) accepted = SetOnce(&action,value);
        else if (strcmp(option,"--standard") == 0) accepted = SetOnce(&standard,value);
        else if (strcmp(option,"--source") == 0) accepted = SetOnce(&source,value);
        else if (strcmp(option,"--working-directory") == 0) accepted = SetOnce(&directory,value);
        else if (strcmp(option,"--output") == 0) accepted = SetOnce(&output,value);
        else if (strcmp(option,"--include") == 0 && include_count < UMI_COMPILER_MAX_OPTIONS) {
            includes[include_count++] = value; accepted = true;
        } else if (strcmp(option,"--define") == 0 && definition_count < UMI_COMPILER_MAX_OPTIONS) {
            definitions[definition_count++] = value; accepted = true;
        }
        if (!accepted) return CompilerHelp(2);
    }
    if (family == NULL || driver == NULL || language == NULL || action == NULL ||
        standard == NULL || source == NULL || directory == NULL) return CompilerHelp(2);
    UmiCompilerLanguage kind = strcmp(language,"c") == 0 ? UMI_COMPILER_LANGUAGE_C :
        strcmp(language,"cpp") == 0 ? UMI_COMPILER_LANGUAGE_CPP : UMI_COMPILER_LANGUAGE_UNKNOWN;
    bool check = strcmp(action,"check") == 0;
    if (kind == UMI_COMPILER_LANGUAGE_UNKNOWN || (!check && strcmp(action,"compile") != 0) ||
        (check && (output != NULL || debug)) || (!check && output == NULL)) return CompilerHelp(2);
    UmiCompilerProvider provider;
    if (strcmp(family,"gcc") == 0) provider = umi_compiler_gcc_provider(driver,NULL);
    else if (strcmp(family,"clang") == 0) provider = umi_compiler_clang_provider(driver,NULL);
    else if (strcmp(family,"msvc") == 0) provider = umi_compiler_msvc_provider(driver,NULL);
    else return CompilerHelp(2);
    if (provider.revision == 0U) return CompilerHelp(2);
    /* Large public inline owners live on the heap. No process callback, discovery
     * context or working-directory mutation is part of this presentation path. */
    UmiCompilerRequest *request = malloc(sizeof(*request));
    UmiCompilerPlan *plan = malloc(sizeof(*plan));
    char *json = malloc(UMI_COMPILER_PLAN_JSON_CAPACITY);
    if (request == NULL || plan == NULL || json == NULL) {
        free(request); free(plan); free(json);
        (void)fputs("Compiler plan: allocation failed.\n",stderr); return 2;
    }
    /* CHECK retains the request API's required output field, but its planner
     * does not emit an output switch. This sentinel is never opened or printed. */
    UmiStatus status = umi_compiler_request_init(request,"cli","unit",kind,source,
                                               check ? "syntax-check-unused" : output);
    if (status == UMI_STATUS_OK &&
        (!CopyField(request->standard,sizeof(request->standard),standard) ||
         !CopyField(request->working_directory,sizeof(request->working_directory),directory)))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        request->action = check ? UMI_COMPILER_ACTION_CHECK : UMI_COMPILER_ACTION_COMPILE;
        request->debug_information = debug;
        request->warnings_as_errors = warnings;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < include_count; ++i)
        status = umi_compiler_request_add_include(request,includes[i]);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < definition_count; ++i)
        status = umi_compiler_request_add_definition(request,definitions[i]);
    if (status == UMI_STATUS_OK) status = umi_compiler_provider_plan(&provider,request,plan);
    if (status == UMI_STATUS_OK) status = umi_compiler_plan_json(plan,json,UMI_COMPILER_PLAN_JSON_CAPACITY);
    int result = 2;
    if (status != UMI_STATUS_OK) (void)fprintf(stderr,"Compiler plan refused (Framework status %d).\n",(int)status);
    else if (fputs(json,stdout) != EOF && fflush(stdout) == 0) result = 0;
    else (void)fputs("Compiler plan: output failed.\n",stderr);
    free(request); free(plan); free(json);
    return result;
}
#endif
int umi_cli_command_compiler(int argc,char **argv)
{
    if (argc < 0 || argc > 1024 || (argc != 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (strcmp(argv[0],"--help") == 0 || strcmp(argv[0],"help") == 0))
        return CompilerHelp(0);
    if (argc == 2 && strcmp(argv[0],"plan") == 0 && strcmp(argv[1],"--help") == 0)
        return CompilerHelp(0);
    if (argc < 1 || strcmp(argv[0],"plan") != 0) return CompilerHelp(2);
    const char *family = NULL, *driver = NULL, *language = NULL, *action = NULL;
    const char *standard = NULL, *source = NULL, *directory = NULL, *output = NULL;
    const char *includes[UMI_COMPILER_MAX_OPTIONS], *definitions[UMI_COMPILER_MAX_OPTIONS];
    size_t include_count = 0U, definition_count = 0U;
    bool debug = false, warnings = false, position_independent = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i],"--debug") == 0) {
            if (debug) return CompilerHelp(2);
            debug = true; continue;
        }
        if (strcmp(argv[i],"--warnings-as-errors") == 0) {
            if (warnings) return CompilerHelp(2);
            warnings = true; continue;
        }
        if (strcmp(argv[i],"--position-independent") == 0) {
            if (position_independent) return CompilerHelp(2);
            position_independent = true; continue;
        }
        const char *option = argv[i];
        if (++i >= argc || strncmp(argv[i],"--",2U) == 0) return CompilerHelp(2);
        const char *value = argv[i];
        bool accepted = false;
        if (strcmp(option,"--provider") == 0) accepted = SetOnce(&family,value);
        else if (strcmp(option,"--executable") == 0) accepted = SetOnce(&driver,value);
        else if (strcmp(option,"--language") == 0) accepted = SetOnce(&language,value);
        else if (strcmp(option,"--action") == 0) accepted = SetOnce(&action,value);
        else if (strcmp(option,"--standard") == 0) accepted = SetOnce(&standard,value);
        else if (strcmp(option,"--source") == 0) accepted = SetOnce(&source,value);
        else if (strcmp(option,"--working-directory") == 0) accepted = SetOnce(&directory,value);
        else if (strcmp(option,"--output") == 0) accepted = SetOnce(&output,value);
        else if (strcmp(option,"--include") == 0 && include_count < UMI_COMPILER_MAX_OPTIONS) {
            includes[include_count++] = value; accepted = true;
        } else if (strcmp(option,"--define") == 0 && definition_count < UMI_COMPILER_MAX_OPTIONS) {
            definitions[definition_count++] = value; accepted = true;
        }
        if (!accepted) return CompilerHelp(2);
    }
    if (family == NULL || driver == NULL || language == NULL || action == NULL ||
        standard == NULL || source == NULL || directory == NULL) return CompilerHelp(2);
    UmiCompilerLanguage kind = strcmp(language,"c") == 0 ? UMI_COMPILER_LANGUAGE_C :
        strcmp(language,"cpp") == 0 ? UMI_COMPILER_LANGUAGE_CPP : UMI_COMPILER_LANGUAGE_UNKNOWN;
    bool check = strcmp(action,"check") == 0;
    if (kind == UMI_COMPILER_LANGUAGE_UNKNOWN || (!check && strcmp(action,"compile") != 0) ||
        (check && (output != NULL || debug || position_independent)) || (!check && output == NULL)) return CompilerHelp(2);
    UmiCompilerProvider provider;
    if (strcmp(family,"gcc") == 0) provider = umi_compiler_gcc_provider(driver,NULL);
    else if (strcmp(family,"clang") == 0) provider = umi_compiler_clang_provider(driver,NULL);
    else if (strcmp(family,"msvc") == 0) provider = umi_compiler_msvc_provider(driver,NULL);
    else return CompilerHelp(2);
    if (provider.revision == 0U) return CompilerHelp(2);
    /* Large public inline owners live on the heap. No process callback, discovery
     * context or working-directory mutation is part of this presentation path. */
    UmiCompilerRequest *request = malloc(sizeof(*request));
    UmiCompilerPlan *plan = malloc(sizeof(*plan));
    char *json = malloc(UMI_COMPILER_PLAN_JSON_CAPACITY);
    if (request == NULL || plan == NULL || json == NULL) {
        free(request); free(plan); free(json);
        (void)fputs("Compiler plan: allocation failed.\n",stderr); return 2;
    }
    /* CHECK retains the request API's required output field, but its planner
     * does not emit an output switch. This sentinel is never opened or printed. */
    UmiStatus status = umi_compiler_request_init(request,"cli","unit",kind,source,
                                               check ? "syntax-check-unused" : output);
    if (status == UMI_STATUS_OK &&
        (!CopyField(request->standard,sizeof(request->standard),standard) ||
         !CopyField(request->working_directory,sizeof(request->working_directory),directory)))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        request->action = check ? UMI_COMPILER_ACTION_CHECK : UMI_COMPILER_ACTION_COMPILE;
        request->debug_information = debug;
        request->warnings_as_errors = warnings;
        request->position_independent = position_independent;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < include_count; ++i)
        status = umi_compiler_request_add_include(request,includes[i]);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < definition_count; ++i)
        status = umi_compiler_request_add_definition(request,definitions[i]);
    if (status == UMI_STATUS_OK) status = umi_compiler_provider_plan(&provider,request,plan);
    if (status == UMI_STATUS_OK) status = umi_compiler_plan_json(plan,json,UMI_COMPILER_PLAN_JSON_CAPACITY);
    int result = 2;
    if (status != UMI_STATUS_OK) (void)fprintf(stderr,"Compiler plan refused (Framework status %d).\n",(int)status);
    else if (fputs(json,stdout) != EOF && fflush(stdout) == 0) result = 0;
    else (void)fputs("Compiler plan: output failed.\n",stderr);
    free(request); free(plan); free(json);
    return result;
}
