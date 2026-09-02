/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/tools/umicom/src/main.c
 *
 * PURPOSE:
 *   Implement the main behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"
#include "wizard.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define UMI_GETCWD _getcwd
#define UMI_ACCESS _access
#define UMI_EXE ".exe"
#else
#include <unistd.h>
#define UMI_GETCWD getcwd
#define UMI_ACCESS access
#define UMI_EXE ""
#endif

#ifndef UMICOM_TEMPLATE_ROOT
#define UMICOM_TEMPLATE_ROOT "templates"
#endif

#define PATH_CAPACITY 2048U

/* Provide the exists operation used by this module and its client applications. */
static int exists(const char *path) { return UMI_ACCESS(path, 0) == 0; }

/* Provide the join path operation used by this module and its client applications. */
static void join_path(char *out, size_t capacity, const char *left, const char *right)
{
    size_t length = strlen(left);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U && (left[length - 1U] == '/' || left[length - 1U] == '\\'))
        (void)snprintf(out, capacity, "%s%s", left, right);
    /* Use this fallback path when the earlier condition does not apply. */
    else
        (void)snprintf(out, capacity, "%s/%s", left, right);
}

/* Provide the find root operation used by this module and its client applications. */
static int find_root(char *root, size_t capacity)
{
    char current[PATH_CAPACITY];
    char marker[PATH_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (UMI_GETCWD(current, sizeof(current)) == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        join_path(marker, sizeof(marker), current, ".umicom-root");
        /* Use the optional file only when it is present in this checkout. */
        if (exists(marker)) {
            (void)snprintf(root, capacity, "%s", current);
            return 1;
        }
        {
            char *slash1 = strrchr(current, '/');
            char *slash2 = strrchr(current, '\\');
            char *slash = slash1 > slash2 ? slash1 : slash2;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (slash == NULL) break;
#ifdef _WIN32
            /* Apply this branch only when its contract condition is satisfied. */
            if (slash == current + 2 && current[1] == ':') slash[1] = '\0';
            /* Use this fallback path when the earlier condition does not apply. */
            else *slash = '\0';
#else
            /* Apply this branch only when its contract condition is satisfied. */
            if (slash == current) current[1] = '\0';
            /* Use this fallback path when the earlier condition does not apply. */
            else *slash = '\0';
#endif
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strlen(current) <= 1U) break;
        }
    }
    return 0;
}

/* Provide the run command operation used by this module and its client applications. */
static int run_command(const char *command)
{
    int code;
    printf("\n> %s\n", command);
    fflush(stdout);
    code = system(command);
    /* Apply this branch only when its contract condition is satisfied. */
    if (code != 0) fprintf(stderr, "Command failed with code %d\n", code);
    return code;
}

/* Provide the help operation used by this module and its client applications. */
static void help(void)
{
    puts("Umicom Framework 0.4.0");
    puts("");
    puts("Creation commands:");
    puts("  umicom framework");
    puts("  umicom framework init [destination]");
    puts("  umicom framework create <application name> [console] [gtk] [web]");
    puts("  umicom framework <application name> [console] [gtk] [web]");
    puts("  umicom framework wizard");
    puts("");
    puts("Development commands:");
    puts("  umicom framework check");
    puts("  umicom framework configure [--no-gtk]");
    puts("  umicom framework build");
    puts("  umicom framework test");
    puts("  umicom framework install [prefix]");
    puts("  umicom framework tree");
}

/* Provide the parse frontends operation used by this module and its client applications. */
static unsigned parse_frontends(int argc, char **argv, int start)
{
    unsigned frontends = 0U;
    int i;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = start; i < argc; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(argv[i], "console") == 0 || strcmp(argv[i], "--console") == 0)
            frontends |= UMI_SCAFFOLD_FRONTEND_CONSOLE;
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(argv[i], "gtk") == 0 || strcmp(argv[i], "gtk4") == 0 || strcmp(argv[i], "--gtk") == 0)
            frontends |= UMI_SCAFFOLD_FRONTEND_GTK4;
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(argv[i], "web") == 0 || strcmp(argv[i], "--web") == 0)
            frontends |= UMI_SCAFFOLD_FRONTEND_WEB;
    }
    return frontends == 0U ? UMI_SCAFFOLD_FRONTEND_GTK4 : frontends;
}

/*
 * Initialise command from caller-provided values so later operations receive a known
 * state.
 */
static int command_init(const char *destination)
{
    UmiStatus status = umi_scaffold_copy_workspace_template(UMICOM_TEMPLATE_ROOT, destination);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Workspace creation failed: %s\n", umi_status_text(status));
        return 1;
    }
    printf("Created Umicom workspace: %s\n", destination);
    return 0;
}

/*
 * Initialise command from caller-provided values so later operations receive a known
 * state.
 */
static int command_create(const char *root, const char *name, unsigned frontends)
{
    UmiScaffoldRequest request;
    UmiScaffoldReport report;
    UmiStatus status;
    request.workspace_root = root;
    request.application_name = name;
    request.frontends = frontends;
    status = umi_scaffold_create_application(&request, &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Application creation failed: %s\n", umi_status_text(status));
        return 1;
    }
    printf("Created application: %s\n", report.application_slug);
    printf("Directory: %s\n", report.application_directory);
    printf("Directories created: %zu\n", report.directories_created);
    printf("Files created: %zu\n", report.files_created);
    return 0;
}

/* Provide the command check operation used by this module and its client applications. */
static int command_check(void)
{
    int failures = 0;
    failures += run_command("git --version") != 0;
    failures += run_command("gh --version") != 0;
    failures += run_command("cmake --version") != 0;
    failures += run_command("ninja --version") != 0;
    failures += run_command("clang --version") != 0;
    failures += run_command("pkg-config --modversion gtk4") != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (failures == 0) puts("\nCHECK PASSED");
    /* Use this fallback path when the earlier condition does not apply. */
    else puts("\nCHECK FOUND ONE OR MORE MISSING TOOLS");
    return failures == 0 ? 0 : 1;
}

/* Provide the command configure operation used by this module and its client applications. */
static int command_configure(const char *root, int no_gtk)
{
    char command[PATH_CAPACITY * 2U];
    (void)snprintf(command, sizeof(command),
        "cmake -S \"%s\" -B \"%s/build/windows-debug\" -G Ninja "
        "-DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang "
        "-DBUILD_TESTING=ON -DUMICOM_BUILD_GTK4=%s",
        root, root, no_gtk ? "OFF" : "ON");
    return run_command(command);
}

/* Provide the command build operation used by this module and its client applications. */
static int command_build(const char *root)
{
    char command[PATH_CAPACITY * 2U];
    (void)snprintf(command, sizeof(command),
                   "cmake --build \"%s/build/windows-debug\" --parallel", root);
    return run_command(command);
}

/* Provide the command test operation used by this module and its client applications. */
static int command_test(const char *root)
{
    char command[PATH_CAPACITY * 2U];
    (void)snprintf(command, sizeof(command),
                   "ctest --test-dir \"%s/build/windows-debug\" --output-on-failure", root);
    return run_command(command);
}

/* Provide the command install operation used by this module and its client applications. */
static int command_install(const char *root, const char *prefix)
{
    char command[PATH_CAPACITY * 2U];
    (void)snprintf(command, sizeof(command),
                   "cmake --install \"%s/build/windows-debug\" --prefix \"%s\"",
                   root, prefix);
    return run_command(command);
}

/* Provide the print tree operation used by this module and its client applications. */
static int print_tree(const char *root)
{
    char path[PATH_CAPACITY];
    FILE *file;
    char line[1024];
    join_path(path, sizeof(path), root, "TREE.txt");
    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (fgets(line, sizeof(line), file) != NULL) fputs(line, stdout);
    fclose(file);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(int argc, char **argv)
{
    char root[PATH_CAPACITY];
    char current[PATH_CAPACITY];
    int command_index;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0) {
        help();
        return 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[1], "framework") != 0) {
        fprintf(stderr, "Expected the first command to be: framework\n");
        help();
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (argc == 2) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (find_root(root, sizeof(root))) {
            printf("Umicom Framework workspace: %s\n", root);
            help();
            return 0;
        }
        return command_init("Umicom");
    }

    command_index = 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "wizard") == 0)
        return umi_wizard_run(argc, argv, UMICOM_TEMPLATE_ROOT);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "check") == 0)
        return command_check();
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "init") == 0) {
        const char *destination = argc > 3 ? argv[3] : "Umicom";
        return command_init(destination);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!find_root(root, sizeof(root))) {
        fprintf(stderr, "No .umicom-root was found. Create or enter an Umicom workspace first.\n");
        return 1;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "create") == 0) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc < 4) { fputs("Application name is required.\n", stderr); return 1; }
        return command_create(root, argv[3], parse_frontends(argc, argv, 4));
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "configure") == 0) {
        int no_gtk = argc > 3 && strcmp(argv[3], "--no-gtk") == 0;
        return command_configure(root, no_gtk);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "build") == 0) return command_build(root);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "test") == 0) return command_test(root);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "install") == 0) {
        const char *prefix;
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc > 3) prefix = argv[3];
        /* Use this fallback path when the earlier condition does not apply. */
        else {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (UMI_GETCWD(current, sizeof(current)) == NULL) return 1;
            join_path(current, sizeof(current), root, "install/windows-debug");
            prefix = current;
        }
        return command_install(root, prefix);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[command_index], "tree") == 0) return print_tree(root);

    /* Compact form: umicom framework "Application Name" console gtk web */
    return command_create(root, argv[command_index], parse_frontends(argc, argv, command_index + 1));
}
