/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/toolchain_binding.c
 * PURPOSE: Implement deterministic language-to-toolchain resolution.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/toolchain_binding.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperToolchainBindingRegistry {
    UmiDeveloperToolchainBindingSnapshot
        items[UMI_DEVELOPER_TOOLCHAIN_BINDING_CAPACITY];
    size_t count;
    uint64_t revision;
};

typedef struct BuiltinToolchainBinding {
    const char *id;
    const char *language_id;
    const char *platform;
    const char *architecture;
    const char *compiler;
    const char *linker;
    const char *debugger;
    const char *runtime;
    const char *formatter;
    const char *linter;
    const char *test_runner;
    const char *package_tool;
    UmiLanguageCapabilityFlags capabilities;
    uint32_t priority;
} BuiltinToolchainBinding;

#define NATIVE_TOOLS \
    (UMI_LANGUAGE_CAPABILITY_BUILD | UMI_LANGUAGE_CAPABILITY_RUN | \
     UMI_LANGUAGE_CAPABILITY_DEBUG | UMI_LANGUAGE_CAPABILITY_TEST | \
     UMI_LANGUAGE_CAPABILITY_PACKAGE | UMI_LANGUAGE_CAPABILITY_FORMAT | \
     UMI_LANGUAGE_CAPABILITY_LINT | UMI_LANGUAGE_CAPABILITY_MACHINE_CODE)
#define SCRIPT_TOOLS \
    (UMI_LANGUAGE_CAPABILITY_RUN | UMI_LANGUAGE_CAPABILITY_DEBUG | \
     UMI_LANGUAGE_CAPABILITY_TEST | UMI_LANGUAGE_CAPABILITY_PACKAGE | \
     UMI_LANGUAGE_CAPABILITY_FORMAT | UMI_LANGUAGE_CAPABILITY_LINT | \
     UMI_LANGUAGE_CAPABILITY_REPL)
#define DOCUMENT_TOOLS \
    (UMI_LANGUAGE_CAPABILITY_FORMAT | UMI_LANGUAGE_CAPABILITY_LINT | \
     UMI_LANGUAGE_CAPABILITY_PREVIEW)

static const BuiltinToolchainBinding builtin_bindings[] = {
    {"c.clang", "c", "*", "*", "clang", "clang", "lldb;gdb", "native",
     "clang-format", "clang-tidy", "ctest", "cpack", NATIVE_TOOLS, 100U},
    {"c.gcc", "c", "*", "*", "gcc", "gcc", "gdb", "native",
     "clang-format", "cppcheck", "ctest", "cpack", NATIVE_TOOLS, 90U},
    {"c.msvc", "c", "windows", "*", "cl", "link", "cppvsdbg", "native",
     "clang-format", "clang-tidy", "ctest", "cpack", NATIVE_TOOLS, 110U},
    {"cpp.clang", "cpp", "*", "*", "clang++", "clang++", "lldb;gdb", "native",
     "clang-format", "clang-tidy", "ctest", "cpack", NATIVE_TOOLS, 100U},
    {"cpp.gcc", "cpp", "*", "*", "g++", "g++", "gdb", "native",
     "clang-format", "cppcheck", "ctest", "cpack", NATIVE_TOOLS, 90U},
    {"cpp.msvc", "cpp", "windows", "*", "cl", "link", "cppvsdbg", "native",
     "clang-format", "clang-tidy", "ctest", "cpack", NATIVE_TOOLS, 110U},
    {"assembly.llvm", "assembly", "*", "*", "clang", "clang", "lldb;gdb",
     "native", "asmfmt", "", "ctest", "cpack", NATIVE_TOOLS, 100U},
    {"assembly.nasm", "assembly", "*", "x86_64", "nasm", "ld", "gdb",
     "native", "asmfmt", "", "ctest", "cpack", NATIVE_TOOLS, 95U},
    {"umicc.default", "umicc", "*", "*", "umicc compile", "umicc link", "gdb;lldb",
     "native", "umicc format", "umicc check", "umicc test", "cpack",
     NATIVE_TOOLS | UMI_LANGUAGE_CAPABILITY_TRANSPILE_TO_C, 100U},
    {"uai.default", "uai", "*", "*", "uai compile", "uai link", "gdb;lldb",
     "native", "uai format", "uai check", "uai test", "cpack",
     NATIVE_TOOLS | UMI_LANGUAGE_CAPABILITY_TRANSPILE_TO_C, 100U},
    {"python.cpython", "python", "*", "*", "python -m compileall", "", "debugpy",
     "python", "black;ruff format", "ruff;pylint", "pytest", "python -m build;pip",
     SCRIPT_TOOLS | UMI_LANGUAGE_CAPABILITY_BUILD, 100U},
    {"rust.cargo", "rust", "*", "*", "cargo build", "rustc", "lldb;gdb",
     "native", "rustfmt", "clippy", "cargo test", "cargo package", NATIVE_TOOLS, 100U},
    {"zig.default", "zig", "*", "*", "zig build", "zig", "lldb;gdb", "native",
     "zig fmt", "zig ast-check", "zig test", "zig build", NATIVE_TOOLS, 100U},
    {"java.jdk", "java", "*", "*", "javac", "jar", "java-debug", "java",
     "google-java-format", "Checkstyle", "JUnit", "Maven;Gradle",
     SCRIPT_TOOLS | UMI_LANGUAGE_CAPABILITY_BUILD, 100U},
    {"php.default", "php", "*", "*", "php -l", "", "Xdebug", "php",
     "php-cs-fixer", "PHPStan;Psalm", "PHPUnit", "Composer", SCRIPT_TOOLS, 100U},
    {"javascript.node", "javascript", "*", "*", "", "", "js-debug", "node",
     "prettier", "eslint", "node --test", "npm;pnpm;yarn", SCRIPT_TOOLS, 100U},
    {"typescript.node", "typescript", "*", "*", "tsc", "", "js-debug", "node",
     "prettier", "eslint", "vitest", "npm;pnpm;yarn",
     SCRIPT_TOOLS | UMI_LANGUAGE_CAPABILITY_BUILD, 100U},
    {"html.web", "html", "*", "*", "", "", "", "browser", "prettier",
     "htmlhint", "", "", DOCUMENT_TOOLS, 100U},
    {"css.web", "css", "*", "*", "", "", "", "browser", "prettier",
     "stylelint", "", "npm", DOCUMENT_TOOLS, 100U},
    {"markdown.default", "markdown", "*", "*", "", "", "", "", "prettier",
     "markdownlint", "", "", DOCUMENT_TOOLS, 100U},
    {"bash.default", "bash", "*", "*", "", "", "bashdb", "bash", "shfmt",
     "shellcheck", "bats", "", SCRIPT_TOOLS, 100U},
    {"powershell.default", "powershell", "*", "*", "", "", "PowerShell debugger",
     "pwsh", "Invoke-Formatter", "PSScriptAnalyzer", "Pester", "PowerShellGet",
     SCRIPT_TOOLS, 100U},
    {"cmake.default", "cmake", "*", "*", "cmake --build", "cmake", "", "cmake",
     "cmake-format", "cmake-lint", "ctest", "cpack",
     UMI_LANGUAGE_CAPABILITY_BUILD | UMI_LANGUAGE_CAPABILITY_TEST |
     UMI_LANGUAGE_CAPABILITY_PACKAGE | UMI_LANGUAGE_CAPABILITY_FORMAT |
     UMI_LANGUAGE_CAPABILITY_LINT, 100U},
    {"make.default", "make", "*", "*", "make", "", "", "make", "", "checkmake",
     "", "", UMI_LANGUAGE_CAPABILITY_BUILD | UMI_LANGUAGE_CAPABILITY_LINT, 100U},
    {"json.default", "json", "*", "*", "", "", "", "", "prettier", "jsonlint",
     "", "", DOCUMENT_TOOLS, 100U},
    {"yaml.default", "yaml", "*", "*", "", "", "", "", "prettier", "yamllint",
     "", "", DOCUMENT_TOOLS, 100U},
    {"toml.default", "toml", "*", "*", "", "", "", "", "taplo fmt", "taplo lint",
     "", "", DOCUMENT_TOOLS, 100U}
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

static size_t find_index(const UmiDeveloperToolchainBindingRegistry *registry,
                         const char *binding_id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, binding_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int selector_matches(const char *selector, const char *requested)
{
    return selector[0] == '\0' || strcmp(selector, "*") == 0 ||
        requested == NULL || requested[0] == '\0' ||
        strcmp(selector, requested) == 0;
}

void umi_developer_toolchain_binding_init(
    UmiDeveloperToolchainBindingSnapshot *binding)
{
    if (binding == NULL) return;
    memset(binding, 0, sizeof(*binding));
    binding->struct_size = (uint32_t)sizeof(*binding);
    binding->api_version = UMI_DEVELOPER_TOOLCHAIN_BINDING_API_VERSION;
    copy_text(binding->platform, sizeof(binding->platform), "*");
    copy_text(binding->architecture, sizeof(binding->architecture), "*");
    binding->enabled = 1;
    binding->revision = 1U;
}

UmiStatus umi_developer_toolchain_binding_registry_create(
    UmiDeveloperToolchainBindingRegistry **out_registry)
{
    UmiDeveloperToolchainBindingRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDeveloperToolchainBindingRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_toolchain_binding_registry_destroy(
    UmiDeveloperToolchainBindingRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_toolchain_binding_registry_upsert(
    UmiDeveloperToolchainBindingRegistry *registry,
    const UmiDeveloperToolchainBindingSnapshot *binding)
{
    size_t index;
    UmiDeveloperToolchainBindingSnapshot copy;
    if (registry == NULL || binding == NULL || binding->id[0] == '\0' ||
        binding->language_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, binding->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_DEVELOPER_TOOLCHAIN_BINDING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    copy = *binding;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_DEVELOPER_TOOLCHAIN_BINDING_API_VERSION;
    copy.revision = registry->items[index].revision + 1U;
    if (copy.revision == 1U) copy.revision = 1U;
    registry->items[index] = copy;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_toolchain_binding_registry_remove(
    UmiDeveloperToolchainBindingRegistry *registry, const char *binding_id)
{
    size_t index;
    if (registry == NULL || binding_id == NULL || binding_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, binding_id);
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

UmiStatus umi_developer_toolchain_binding_registry_find(
    const UmiDeveloperToolchainBindingRegistry *registry, const char *binding_id,
    UmiDeveloperToolchainBindingSnapshot *out_binding)
{
    size_t index;
    if (registry == NULL || binding_id == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, binding_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_binding = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_toolchain_binding_registry_resolve(
    const UmiDeveloperToolchainBindingRegistry *registry,
    const char *language_id, const char *platform, const char *architecture,
    UmiLanguageCapabilityFlags required_capabilities,
    UmiDeveloperToolchainBindingSnapshot *out_binding)
{
    size_t index;
    size_t best_index = SIZE_MAX;
    uint64_t best_score = 0U;
    if (registry == NULL || language_id == NULL || language_id[0] == '\0' ||
        out_binding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index) {
        const UmiDeveloperToolchainBindingSnapshot *candidate = &registry->items[index];
        uint64_t score;
        if (candidate->enabled == 0 ||
            strcmp(candidate->language_id, language_id) != 0 ||
            !selector_matches(candidate->platform, platform) ||
            !selector_matches(candidate->architecture, architecture) ||
            (candidate->capabilities & required_capabilities) != required_capabilities)
            continue;
        score = candidate->priority;
        if (platform != NULL && platform[0] != '\0' &&
            strcmp(candidate->platform, platform) == 0) score += UINT64_C(1000000);
        if (architecture != NULL && architecture[0] != '\0' &&
            strcmp(candidate->architecture, architecture) == 0) score += UINT64_C(100000);
        if (best_index == SIZE_MAX || score > best_score) {
            best_index = index;
            best_score = score;
        }
    }
    if (best_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_binding = registry->items[best_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_toolchain_binding_registry_at(
    const UmiDeveloperToolchainBindingRegistry *registry, size_t index,
    UmiDeveloperToolchainBindingSnapshot *out_binding)
{
    if (registry == NULL || out_binding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_binding = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_toolchain_binding_registry_snapshot(
    const UmiDeveloperToolchainBindingRegistry *registry,
    UmiDeveloperToolchainBindingRegistrySnapshot *out_snapshot)
{
    size_t index;
    if (registry == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_TOOLCHAIN_BINDING_API_VERSION;
    out_snapshot->binding_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        const UmiDeveloperToolchainBindingSnapshot *binding = &registry->items[index];
        if (binding->enabled != 0) out_snapshot->enabled_count += 1U;
        if ((binding->capabilities & UMI_LANGUAGE_CAPABILITY_BUILD) != 0U)
            out_snapshot->build_capable_count += 1U;
        if ((binding->capabilities & UMI_LANGUAGE_CAPABILITY_DEBUG) != 0U)
            out_snapshot->debug_capable_count += 1U;
        if ((binding->capabilities & UMI_LANGUAGE_CAPABILITY_TEST) != 0U)
            out_snapshot->test_capable_count += 1U;
        if ((binding->capabilities & UMI_LANGUAGE_CAPABILITY_PACKAGE) != 0U)
            out_snapshot->package_capable_count += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_toolchain_binding_register_builtins(
    UmiDeveloperToolchainBindingRegistry *registry)
{
    size_t index;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_developer_toolchain_binding_builtin_count(); ++index) {
        const BuiltinToolchainBinding *builtin = &builtin_bindings[index];
        UmiDeveloperToolchainBindingSnapshot binding;
        UmiStatus status;
        umi_developer_toolchain_binding_init(&binding);
        copy_text(binding.id, sizeof(binding.id), builtin->id);
        copy_text(binding.language_id, sizeof(binding.language_id), builtin->language_id);
        copy_text(binding.platform, sizeof(binding.platform), builtin->platform);
        copy_text(binding.architecture, sizeof(binding.architecture), builtin->architecture);
        copy_text(binding.environment_profile, sizeof(binding.environment_profile), "system");
        copy_text(binding.compiler, sizeof(binding.compiler), builtin->compiler);
        copy_text(binding.linker, sizeof(binding.linker), builtin->linker);
        copy_text(binding.debugger, sizeof(binding.debugger), builtin->debugger);
        copy_text(binding.runtime, sizeof(binding.runtime), builtin->runtime);
        copy_text(binding.formatter, sizeof(binding.formatter), builtin->formatter);
        copy_text(binding.linter, sizeof(binding.linter), builtin->linter);
        copy_text(binding.test_runner, sizeof(binding.test_runner), builtin->test_runner);
        copy_text(binding.package_tool, sizeof(binding.package_tool), builtin->package_tool);
        binding.capabilities = builtin->capabilities;
        binding.priority = builtin->priority;
        status = umi_developer_toolchain_binding_registry_upsert(registry, &binding);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

size_t umi_developer_toolchain_binding_builtin_count(void)
{
    return sizeof(builtin_bindings) / sizeof(builtin_bindings[0]);
}
