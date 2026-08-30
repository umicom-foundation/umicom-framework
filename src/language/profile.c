/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/profile.c
 * PURPOSE: Implement the universal, provider-neutral language profile registry.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language/profile.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiLanguageProfileRegistry {
    UmiLanguageProfileSnapshot items[UMI_LANGUAGE_PROFILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

typedef struct BuiltinLanguageProfile {
    const char *id;
    const char *name;
    UmiLanguageExecutionKind execution_kind;
    UmiLanguageCapabilityFlags capabilities;
    const char *file_extensions;
    const char *file_names;
    const char *mime_types;
    const char *default_standard;
    const char *compiler_family;
    const char *language_server;
    const char *debug_adapter;
    const char *formatter;
    const char *linter;
    const char *test_runner;
    const char *package_manager;
    const char *runtime;
    const char *transpiler;
} BuiltinLanguageProfile;

#define EDITING_CAPABILITIES \
    (UMI_LANGUAGE_CAPABILITY_EDIT | UMI_LANGUAGE_CAPABILITY_SYNTAX | \
     UMI_LANGUAGE_CAPABILITY_SEMANTIC | UMI_LANGUAGE_CAPABILITY_COMPLETION | \
     UMI_LANGUAGE_CAPABILITY_FORMAT | UMI_LANGUAGE_CAPABILITY_LINT | \
     UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER | \
     UMI_LANGUAGE_CAPABILITY_DOCUMENTATION)
#define NATIVE_CAPABILITIES \
    (EDITING_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_BUILD | \
     UMI_LANGUAGE_CAPABILITY_RUN | UMI_LANGUAGE_CAPABILITY_DEBUG | \
     UMI_LANGUAGE_CAPABILITY_TEST | UMI_LANGUAGE_CAPABILITY_PACKAGE | \
     UMI_LANGUAGE_CAPABILITY_DEBUG_ADAPTER | \
     UMI_LANGUAGE_CAPABILITY_MACHINE_CODE)
#define SCRIPT_CAPABILITIES \
    (EDITING_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_RUN | \
     UMI_LANGUAGE_CAPABILITY_DEBUG | UMI_LANGUAGE_CAPABILITY_TEST | \
     UMI_LANGUAGE_CAPABILITY_PACKAGE | UMI_LANGUAGE_CAPABILITY_REPL | \
     UMI_LANGUAGE_CAPABILITY_DEBUG_ADAPTER)
#define DOCUMENT_CAPABILITIES \
    (UMI_LANGUAGE_CAPABILITY_EDIT | UMI_LANGUAGE_CAPABILITY_SYNTAX | \
     UMI_LANGUAGE_CAPABILITY_FORMAT | UMI_LANGUAGE_CAPABILITY_LINT | \
     UMI_LANGUAGE_CAPABILITY_DOCUMENTATION | UMI_LANGUAGE_CAPABILITY_PREVIEW)

static const BuiltinLanguageProfile builtin_profiles[] = {
    {"c", "C", UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED, NATIVE_CAPABILITIES,
     ".c;.h", "", "text/x-c;text/x-chdr", "C23", "Clang;GCC;MSVC",
     "clangd", "GDB;LLDB;cppvsdbg", "clang-format", "clang-tidy;cppcheck",
     "CTest", "CPack", "native", ""},
    {"cpp", "C++", UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED, NATIVE_CAPABILITIES,
     ".cc;.cpp;.cxx;.c++;.hh;.hpp;.hxx;.h++", "", "text/x-c++src;text/x-c++hdr",
     "C++23", "Clang;GCC;MSVC", "clangd", "GDB;LLDB;cppvsdbg",
     "clang-format", "clang-tidy;cppcheck", "CTest", "CPack", "native", ""},
    {"assembly", "Assembly", UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED,
     NATIVE_CAPABILITIES, ".s;.S;.asm;.inc", "", "text/x-asm", "target-default",
     "GNU as;LLVM MC;NASM;MASM", "asm-lsp", "GDB;LLDB;cppvsdbg", "asmfmt",
     "", "CTest", "CPack", "native", ""},
    {"umicc", "Umicc", UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C,
     NATIVE_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_TRANSPILE_TO_C,
     ".umc;.umicc", "", "text/x-umicc", "Umicc 1", "Umicc", "umicc-lsp",
     "GDB;LLDB", "umicc format", "umicc check", "umicc test", "CPack",
     "native", "umicc transpile"},
    {"uai", "Umicom AI Language", UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C,
     NATIVE_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_TRANSPILE_TO_C,
     ".uai", "", "text/x-uai", "UAI 1", "UAI compiler", "uai-lsp",
     "GDB;LLDB", "uai format", "uai check", "uai test", "CPack", "native",
     "uai transpile"},
    {"python", "Python / CPython", UMI_LANGUAGE_EXECUTION_BYTECODE_VM,
     SCRIPT_CAPABILITIES, ".py;.pyw;.pyi", "", "text/x-python", "Python 3",
     "CPython", "pyright;pylsp", "debugpy", "black;ruff format", "ruff;pylint",
     "pytest;unittest", "pip;build", "python", "Cython;Nuitka"},
    {"rust", "Rust", UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED, NATIVE_CAPABILITIES,
     ".rs", "", "text/rust", "Rust 2024", "rustc", "rust-analyzer",
     "CodeLLDB;GDB", "rustfmt", "clippy", "cargo test", "cargo", "native", ""},
    {"zig", "Zig", UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED, NATIVE_CAPABILITIES,
     ".zig;.zon", "", "text/zig", "latest", "zig", "zls", "GDB;LLDB",
     "zig fmt", "zig ast-check", "zig test", "zig build", "native", ""},
    {"java", "Java", UMI_LANGUAGE_EXECUTION_BYTECODE_VM, SCRIPT_CAPABILITIES |
     UMI_LANGUAGE_CAPABILITY_BUILD, ".java", "", "text/x-java-source", "Java 21",
     "javac", "jdtls", "java-debug", "google-java-format", "Checkstyle;SpotBugs",
     "JUnit", "Maven;Gradle", "JVM", ""},
    {"php", "PHP", UMI_LANGUAGE_EXECUTION_INTERPRETED, SCRIPT_CAPABILITIES,
     ".php;.phtml", "", "application/x-httpd-php", "PHP 8", "php",
     "intelephense;phpactor", "Xdebug", "php-cs-fixer", "PHPStan;Psalm",
     "PHPUnit", "Composer", "php", ""},
    {"javascript", "JavaScript", UMI_LANGUAGE_EXECUTION_INTERPRETED,
     SCRIPT_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_PREVIEW, ".js;.mjs;.cjs;.jsx",
     "", "text/javascript", "ECMAScript 2024", "", "typescript-language-server",
     "js-debug", "prettier", "eslint", "node --test;Jest;Vitest", "npm;pnpm;yarn",
     "Node.js;browser", ""},
    {"typescript", "TypeScript", UMI_LANGUAGE_EXECUTION_BYTECODE_VM,
     SCRIPT_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_BUILD, ".ts;.mts;.cts;.tsx", "",
     "text/typescript", "TypeScript 5", "tsc", "typescript-language-server",
     "js-debug", "prettier", "eslint", "Jest;Vitest", "npm;pnpm;yarn",
     "Node.js;browser", "tsc"},
    {"html", "HTML", UMI_LANGUAGE_EXECUTION_MARKUP, DOCUMENT_CAPABILITIES |
     UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER, ".html;.htm;.xhtml", "",
     "text/html;application/xhtml+xml", "HTML5", "", "html-language-server",
     "", "prettier", "htmlhint", "", "", "browser", ""},
    {"css", "CSS", UMI_LANGUAGE_EXECUTION_MARKUP, DOCUMENT_CAPABILITIES |
     UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER, ".css;.scss;.sass;.less", "",
     "text/css", "CSS", "", "css-language-server", "", "prettier",
     "stylelint", "", "npm", "browser", ""},
    {"markdown", "Markdown", UMI_LANGUAGE_EXECUTION_MARKUP, DOCUMENT_CAPABILITIES,
     ".md;.markdown;.mdown", "README;CHANGELOG;CONTRIBUTING", "text/markdown",
     "CommonMark", "", "marksman", "", "prettier", "markdownlint", "", "",
     "", ""},
    {"bash", "Bash", UMI_LANGUAGE_EXECUTION_SHELL, SCRIPT_CAPABILITIES,
     ".sh;.bash", ".bashrc;.bash_profile", "application/x-sh", "POSIX / Bash",
     "", "bash-language-server", "bashdb", "shfmt", "shellcheck", "bats", "",
     "bash", ""},
    {"powershell", "PowerShell", UMI_LANGUAGE_EXECUTION_SHELL,
     SCRIPT_CAPABILITIES, ".ps1;.psm1;.psd1", "", "text/x-powershell",
     "PowerShell 7", "", "PowerShellEditorServices", "PowerShell debugger",
     "Invoke-Formatter", "PSScriptAnalyzer", "Pester", "PowerShellGet", "pwsh", ""},
    {"cmake", "CMake", UMI_LANGUAGE_EXECUTION_BUILD_CONFIGURATION,
     EDITING_CAPABILITIES | UMI_LANGUAGE_CAPABILITY_BUILD |
     UMI_LANGUAGE_CAPABILITY_TEST | UMI_LANGUAGE_CAPABILITY_PACKAGE,
     ".cmake", "CMakeLists.txt", "text/x-cmake", "CMake 3", "cmake",
     "cmake-language-server", "", "cmake-format", "cmake-lint", "ctest", "cpack",
     "cmake", ""},
    {"make", "Make", UMI_LANGUAGE_EXECUTION_BUILD_CONFIGURATION,
     UMI_LANGUAGE_CAPABILITY_EDIT | UMI_LANGUAGE_CAPABILITY_SYNTAX |
     UMI_LANGUAGE_CAPABILITY_BUILD, ".mk;.mak", "Makefile;GNUmakefile", "text/x-makefile",
     "POSIX Make", "make", "", "", "", "checkmake", "", "", "make", ""},
    {"json", "JSON", UMI_LANGUAGE_EXECUTION_DATA, DOCUMENT_CAPABILITIES |
     UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER, ".json;.jsonc;.json5", "",
     "application/json", "RFC 8259", "", "json-language-server", "", "prettier",
     "jsonlint", "", "", "", ""},
    {"yaml", "YAML", UMI_LANGUAGE_EXECUTION_DATA, DOCUMENT_CAPABILITIES |
     UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER, ".yaml;.yml", "",
     "application/yaml;text/yaml", "YAML 1.2", "", "yaml-language-server", "",
     "prettier", "yamllint", "", "", "", ""},
    {"toml", "TOML", UMI_LANGUAGE_EXECUTION_DATA, DOCUMENT_CAPABILITIES,
     ".toml", "", "application/toml", "TOML 1.0", "", "taplo", "", "taplo fmt",
     "taplo lint", "", "", "", ""}
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static int ascii_equal_nocase(const char *left, const char *right, size_t length)
{
    size_t index;
    for (index = 0U; index < length; ++index) {
        if (tolower((unsigned char)left[index]) !=
            tolower((unsigned char)right[index])) return 0;
    }
    return 1;
}

static const char *path_basename(const char *path)
{
    const char *base = path;
    const char *cursor;
    for (cursor = path; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') base = cursor + 1;
    }
    return base;
}

static size_t list_match_length(const char *list, const char *value, int suffix)
{
    const char *cursor = list;
    size_t value_length = strlen(value);
    size_t best = 0U;
    while (cursor != NULL && *cursor != '\0') {
        const char *end;
        size_t token_length;
        while (*cursor == ';' || *cursor == ',' || isspace((unsigned char)*cursor)) ++cursor;
        if (*cursor == '\0') break;
        end = cursor;
        while (*end != '\0' && *end != ';' && *end != ',') ++end;
        while (end > cursor && isspace((unsigned char)end[-1])) --end;
        token_length = (size_t)(end - cursor);
        if (token_length > 0U && value_length >= token_length) {
            const char *candidate = suffix ? value + value_length - token_length : value;
            if ((!suffix && value_length == token_length) || suffix) {
                if (ascii_equal_nocase(candidate, cursor, token_length) &&
                    token_length > best) best = token_length;
            }
        }
        cursor = *end == '\0' ? NULL : end + 1;
    }
    return best;
}

static size_t find_index(const UmiLanguageProfileRegistry *registry,
                         const char *profile_id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, profile_id) == 0) return index;
    }
    return SIZE_MAX;
}

void umi_language_profile_init(UmiLanguageProfileSnapshot *profile)
{
    if (profile == NULL) return;
    memset(profile, 0, sizeof(*profile));
    profile->struct_size = (uint32_t)sizeof(*profile);
    profile->api_version = UMI_LANGUAGE_PROFILE_API_VERSION;
    profile->enabled = 1;
    profile->revision = 1U;
}

UmiStatus umi_language_profile_registry_create(
    UmiLanguageProfileRegistry **out_registry)
{
    UmiLanguageProfileRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiLanguageProfileRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_language_profile_registry_destroy(UmiLanguageProfileRegistry *registry)
{
    free(registry);
}

UmiStatus umi_language_profile_registry_upsert(
    UmiLanguageProfileRegistry *registry,
    const UmiLanguageProfileSnapshot *profile)
{
    size_t index;
    UmiLanguageProfileSnapshot copy;
    if (registry == NULL || profile == NULL || profile->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, profile->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_LANGUAGE_PROFILE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    copy = *profile;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_LANGUAGE_PROFILE_API_VERSION;
    copy.revision = registry->items[index].revision + 1U;
    if (copy.revision == 1U) copy.revision = 1U;
    registry->items[index] = copy;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_profile_registry_remove(
    UmiLanguageProfileRegistry *registry, const char *profile_id)
{
    size_t index;
    if (registry == NULL || profile_id == NULL || profile_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, profile_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    memset(&registry->items[registry->count], 0, sizeof(registry->items[0]));
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_profile_registry_find(
    const UmiLanguageProfileRegistry *registry, const char *profile_id,
    UmiLanguageProfileSnapshot *out_profile)
{
    size_t index;
    if (registry == NULL || profile_id == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, profile_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_language_profile_registry_find_for_path(
    const UmiLanguageProfileRegistry *registry, const char *path,
    UmiLanguageProfileSnapshot *out_profile)
{
    const char *base;
    size_t best_index = SIZE_MAX;
    size_t best_length = 0U;
    size_t index;
    if (registry == NULL || path == NULL || path[0] == '\0' || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    base = path_basename(path);
    for (index = 0U; index < registry->count; ++index) {
        size_t length;
        if (registry->items[index].enabled == 0) continue;
        length = list_match_length(registry->items[index].file_names, base, 0);
        if (length > 0U) length += UMI_LANGUAGE_PROFILE_LIST_CAPACITY;
        if (length == 0U)
            length = list_match_length(registry->items[index].file_extensions, base, 1);
        if (length > best_length) {
            best_length = length;
            best_index = index;
        }
    }
    if (best_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[best_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_language_profile_registry_at(
    const UmiLanguageProfileRegistry *registry, size_t index,
    UmiLanguageProfileSnapshot *out_profile)
{
    if (registry == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_language_profile_registry_count(const UmiLanguageProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

UmiStatus umi_language_profile_registry_snapshot(
    const UmiLanguageProfileRegistry *registry,
    UmiLanguageProfileRegistrySnapshot *out_snapshot)
{
    size_t index;
    if (registry == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_LANGUAGE_PROFILE_API_VERSION;
    out_snapshot->profile_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        const UmiLanguageProfileSnapshot *profile = &registry->items[index];
        if (profile->enabled != 0) out_snapshot->enabled_count += 1U;
        if (profile->execution_kind == UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED)
            out_snapshot->native_compiled_count += 1U;
        if (profile->execution_kind == UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C)
            out_snapshot->transpiled_to_c_count += 1U;
        if ((profile->capabilities & UMI_LANGUAGE_CAPABILITY_LANGUAGE_SERVER) != 0U)
            out_snapshot->language_server_count += 1U;
        if ((profile->capabilities & UMI_LANGUAGE_CAPABILITY_DEBUG_ADAPTER) != 0U)
            out_snapshot->debug_adapter_count += 1U;
    }
    return UMI_STATUS_OK;
}

int umi_language_profile_supports(const UmiLanguageProfileSnapshot *profile,
                                  UmiLanguageCapabilityFlags required_capabilities)
{
    return profile != NULL && profile->enabled != 0 &&
        (profile->capabilities & required_capabilities) == required_capabilities;
}

UmiStatus umi_language_profile_register_builtins(UmiLanguageProfileRegistry *registry)
{
    size_t index;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_language_profile_builtin_count(); ++index) {
        const BuiltinLanguageProfile *builtin = &builtin_profiles[index];
        UmiLanguageProfileSnapshot profile;
        UmiStatus status;
        umi_language_profile_init(&profile);
        copy_text(profile.id, sizeof(profile.id), builtin->id);
        copy_text(profile.name, sizeof(profile.name), builtin->name);
        profile.execution_kind = builtin->execution_kind;
        profile.capabilities = builtin->capabilities;
        copy_text(profile.file_extensions, sizeof(profile.file_extensions), builtin->file_extensions);
        copy_text(profile.file_names, sizeof(profile.file_names), builtin->file_names);
        copy_text(profile.mime_types, sizeof(profile.mime_types), builtin->mime_types);
        copy_text(profile.default_standard, sizeof(profile.default_standard), builtin->default_standard);
        copy_text(profile.compiler_family, sizeof(profile.compiler_family), builtin->compiler_family);
        copy_text(profile.language_server, sizeof(profile.language_server), builtin->language_server);
        copy_text(profile.debug_adapter, sizeof(profile.debug_adapter), builtin->debug_adapter);
        copy_text(profile.formatter, sizeof(profile.formatter), builtin->formatter);
        copy_text(profile.linter, sizeof(profile.linter), builtin->linter);
        copy_text(profile.test_runner, sizeof(profile.test_runner), builtin->test_runner);
        copy_text(profile.package_manager, sizeof(profile.package_manager), builtin->package_manager);
        copy_text(profile.runtime, sizeof(profile.runtime), builtin->runtime);
        copy_text(profile.transpiler, sizeof(profile.transpiler), builtin->transpiler);
        status = umi_language_profile_registry_upsert(registry, &profile);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

size_t umi_language_profile_builtin_count(void)
{
    return sizeof(builtin_profiles) / sizeof(builtin_profiles[0]);
}

const char *umi_language_execution_kind_text(UmiLanguageExecutionKind kind)
{
    switch (kind) {
    case UMI_LANGUAGE_EXECUTION_NATIVE_COMPILED: return "native-compiled";
    case UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C: return "transpiled-to-c";
    case UMI_LANGUAGE_EXECUTION_BYTECODE_VM: return "bytecode-vm";
    case UMI_LANGUAGE_EXECUTION_INTERPRETED: return "interpreted";
    case UMI_LANGUAGE_EXECUTION_MARKUP: return "markup";
    case UMI_LANGUAGE_EXECUTION_DATA: return "data";
    case UMI_LANGUAGE_EXECUTION_BUILD_CONFIGURATION: return "build-configuration";
    case UMI_LANGUAGE_EXECUTION_SHELL: return "shell";
    case UMI_LANGUAGE_EXECUTION_UNKNOWN:
    default: return "unknown";
    }
}
