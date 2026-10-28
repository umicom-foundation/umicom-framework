/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/build_invocation.c
 * PURPOSE: Resolve one explicit native workflow without shell parsing or guessed installation directories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_invocation.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static UmiStatus invalid(char *message, size_t capacity, const char *reason)
{
    if (message && capacity) (void)snprintf(message, capacity, "%s", reason);
    return UMI_STATUS_INVALID_ARGUMENT;
}

/* Numeric options accept decimal digits only; atoi would turn a typo into an
 * unintended default or an unbounded number of parallel compiler processes. */
static int number(const char *text, uint32_t maximum, uint32_t *value)
{
    uint32_t parsed = 0U;
    if (!text || !*text) return 0;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p < '0' || *p > '9' || parsed > (maximum - (*p - '0')) / 10U) return 0;
        parsed = parsed * 10U + (*p - '0');
    }
    if (!parsed || parsed > maximum) return 0;
    *value = parsed;
    return 1;
}

static UmiStatus resolve(const char *text, const char *base, char *out)
{
    return umi_path_absolute(text, base, out, UMI_PATH_CAPACITY);
}

UmiStatus UmiBuildInvocationParse(UmiBuildAction action, int argc,
    const char *const *argv, const char *cwd, UmiBuildInvocation *inv,
    char *message, size_t capacity)
{
    if (!inv || argc < 0 || (argc && !argv) || !cwd || !umi_path_is_absolute(cwd))
        return invalid(message, capacity, "An absolute working directory and valid argument array are required.");
    memset(inv, 0, sizeof *inv);
    umi_build_request_init(&inv->request);
    const char *source = ".", *binary = NULL, *prefix = NULL, *executable = NULL, *log = NULL;
    const char *alias = NULL;
    uint32_t seen = 0U;
    int configure = action == UMI_BUILD_CONFIGURE || action == UMI_BUILD_MAKE || action == UMI_BUILD_DELIVER;
    /* Preserve the original action classification for comparison. Build-test
     * shares build/test options but never accepts configure options. */
#if 0
    int compile = action == UMI_BUILD_COMPILE || action == UMI_BUILD_MAKE || action == UMI_BUILD_DELIVER;
    int test = action == UMI_BUILD_TEST || action == UMI_BUILD_MAKE || action == UMI_BUILD_DELIVER;
#endif
    int compile = action == UMI_BUILD_COMPILE || action == UMI_BUILD_MAKE || action == UMI_BUILD_DELIVER || action == UMI_BUILD_COMPILE_TEST;
    int test = action == UMI_BUILD_TEST || action == UMI_BUILD_MAKE || action == UMI_BUILD_DELIVER || action == UMI_BUILD_COMPILE_TEST;
    int install = action == UMI_BUILD_INSTALL || action == UMI_BUILD_DELIVER;
    int package = action == UMI_BUILD_PACKAGE || action == UMI_BUILD_DELIVER;
    int run = action == UMI_BUILD_RUN || action == UMI_BUILD_COMMAND;
    static const char *const keys[] = {"--source", "--build", "--preset", "--target", "--jobs",
        "--executable", "--config", "--prefix", "--tests", "--component", "--package-target",
        "--timeout-ms", "--log", "--dry-run", "--clean"};
    for (int i = 0; i < argc; ++i) {
        const char *key = argv[i];
        if (!key || !*key) return invalid(message, capacity, "Empty option is not supported.");
        if (strcmp(key, "--") == 0) {
            if (!run) return invalid(message, capacity, "Arguments after -- are only valid for run or launch.");
            inv->request.run_arguments = argv + i + 1;
            inv->request.run_argument_count = (size_t)(argc - i - 1);
            for (size_t j = 0U; j < inv->request.run_argument_count; ++j)
                if (!inv->request.run_arguments[j]) return invalid(message, capacity, "Launch argument is NULL.");
            break;
        }
        if (strcmp(key, "--define") == 0) {
            if (!configure || i + 1 >= argc || !argv[i + 1] || inv->options.configure_definition_count == 16U)
                return invalid(message, capacity, "Configure accepts at most 16 --define NAME=VALUE options.");
            const char *definition = argv[++i];
            const char *equals = strchr(definition, '=');
            if (!equals || equals == definition || definition[0] == '-')
                return invalid(message, capacity, "Definition must have the form NAME=VALUE.");
            for (const unsigned char *p = (const unsigned char *)definition; p < (const unsigned char *)equals; ++p)
                if (!isalnum(*p) && *p != '_' && *p != ':' && *p != '.' && *p != '-')
                    return invalid(message, capacity, "Definition name contains an unsupported character.");
            inv->definitions[inv->options.configure_definition_count++] = definition;
            continue;
        }
        size_t k = 0U;
        for (; k < sizeof keys / sizeof keys[0]; ++k) if (strcmp(key, keys[k]) == 0) break;
        if (k == sizeof keys / sizeof keys[0]) {
            if (run && i == 0 && key[0] != '-') { alias = key; continue; }
            return invalid(message, capacity, "Unknown option. Use --help; put application arguments after --.");
        }
        if (seen & (UINT32_C(1) << k)) return invalid(message, capacity, "Duplicate option is not supported.");
        seen |= UINT32_C(1) << k;
        if (k == 13U) { inv->options.preview = 1; continue; }
        if (k == 14U) {
            if (!configure) return invalid(message, capacity, "--clean is only valid for configure, make or deliver.");
            inv->request.clean = 1; continue;
        }
        if (i + 1 >= argc || !argv[i + 1] || !argv[i + 1][0] || argv[i + 1][0] == '-')
            return invalid(message, capacity, "Option requires a non-empty value.");
        const char *value = argv[++i];
        uint32_t parsed;
        switch (k) {
            case 0: source = value; break;
            case 1: binary = value; break;
            case 2: inv->request.preset = value; break;
            case 3:
                if (!compile) return invalid(message, capacity, "--target requires build, make or deliver.");
                inv->request.target = value; break;
            case 4:
                if ((!compile && !test && !package) || !number(value, 1024U, &parsed))
                    return invalid(message, capacity, "--jobs requires a build/test/package action and an integer from 1 to 1024.");
                inv->request.jobs = (int)parsed; break;
            case 5:
                if (!run) return invalid(message, capacity, "--executable requires run or launch.");
                executable = value; break;
            case 6:
                if (!compile && !test && !install && !package && !run)
                    return invalid(message, capacity, "--config applies to build, test, install, package or launch.");
                inv->request.configuration = value; break;
            case 7:
                if (!install) return invalid(message, capacity, "--prefix requires install or deliver.");
                prefix = value; break;
            case 8:
                if (!test) return invalid(message, capacity, "--tests requires test, make or deliver.");
                inv->request.test_expression = value; break;
            case 9:
                if (!install) return invalid(message, capacity, "--component requires install or deliver.");
                inv->request.install_component = value; break;
            case 10:
                if (!package) return invalid(message, capacity, "--package-target requires package or deliver.");
                inv->request.package_target = value; break;
            case 11:
                if (!number(value, 86400000U, &parsed)) return invalid(message, capacity, "--timeout-ms must be 1 to 86400000 per phase.");
                inv->request.timeout_ms = parsed; break;
            case 12: log = value; break;
            default: return invalid(message, capacity, "Unsupported option.");
        }
    }
    if ((install && (!binary || !prefix)) || ((action == UMI_BUILD_CLEAN || inv->request.clean) && !binary))
        return invalid(message, capacity, "Install/deliver need --build and --prefix; clean needs --build.");
    if (action == UMI_BUILD_COMPILE_TEST && !binary)
        return invalid(message, capacity, "Build-test requires an existing explicit --build directory.");
    if (run && (inv->request.preset || inv->request.configuration))
        return invalid(message, capacity, "Launch uses --build pointing to the actual binary folder or --executable; no preset/config inference.");
    if (alias && executable) return invalid(message, capacity, "Choose either an application alias or --executable.");
    if (alias && !binary) return invalid(message, capacity, "Application aliases need an explicit --build directory.");
    UmiStatus status = resolve(source, cwd, inv->source);
    if (status == UMI_STATUS_OK && binary) status = resolve(binary, inv->source, inv->binary);
    if (status == UMI_STATUS_OK && prefix) status = resolve(prefix, inv->source, inv->prefix);
    if (status == UMI_STATUS_OK && log) status = resolve(log, inv->source, inv->log);
    if (status == UMI_STATUS_OK && executable) status = resolve(executable, inv->source, inv->executable);
    if (status != UMI_STATUS_OK) return invalid(message, capacity, "A selected path is invalid or too long.");
    if (alias) {
        const char *name = alias;
        if (strcmp(alias, "studio") == 0 || strcmp(alias, "ide") == 0) name = "umicom-studio-ide";
        else if (strcmp(alias, "desk") == 0 || strcmp(alias, "desktop") == 0) name = "umicom-desk";
        else if (strcmp(alias, "console") == 0) name = "umicom-studio-console";
        else if (strcmp(alias, "doctor") == 0) name = "umicom-studio-doctor";
        else if (strcmp(alias, "diagnostics") == 0) name = "umicom-studio-diagnostics";
        else if (strcmp(alias, "settings") == 0) name = "umicom-studio-settings";
        else if (strcmp(alias, "platform") == 0) name = "umicom-studio-platform";
        for (const unsigned char *p = (const unsigned char *)name; *p; ++p)
            if (!isalnum(*p) && *p != '-' && *p != '_') return invalid(message, capacity, "Use --executable for a path; aliases contain letters, digits, hyphens and underscores.");
        char relative[512];
#ifdef _WIN32
        const char *suffix = ".exe";
#else
        const char *suffix = "";
#endif
        int length = snprintf(relative, sizeof relative, "bin/%s%s%s", strncmp(name, "umicom-", 7U) ? "umicom-" : "", name, suffix);
        if (length < 0 || (size_t)length >= sizeof relative || resolve(relative, inv->binary, inv->executable) != UMI_STATUS_OK)
            return invalid(message, capacity, "Application executable path is too long.");
    }
    inv->request.source_root = inv->source;
    inv->request.build_directory = binary ? inv->binary : NULL;
    inv->request.executable = inv->executable[0] ? inv->executable : NULL;
    inv->request.install_prefix = prefix ? inv->prefix : NULL;
    inv->request.window_mode = run ? UMI_PROCESS_WINDOW_INHERIT : UMI_PROCESS_WINDOW_HIDDEN;
    inv->options.explicit_build_directory = binary != NULL;
    inv->options.configure_definitions = inv->definitions;
    if (!binary && !run && !inv->request.preset) {
#ifdef _WIN32
        inv->request.preset = "windows-ucrt64-headless-debug";
#else
        inv->request.preset = "headless-debug";
#endif
    }
    return umi_build_request_validate(action, &inv->request, message, capacity);
}
