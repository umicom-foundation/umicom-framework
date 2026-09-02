/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/scaffold/scaffold.c
 *
 * PURPOSE:
 *   Implement the scaffold behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/scaffold/scaffold.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define UMI_MKDIR(path) _mkdir(path)
#else
#include <unistd.h>
#define UMI_MKDIR(path) mkdir((path), 0775)
#endif

#define UMI_PATH_CAPACITY 2048U
#define UMI_TEMPLATE_CAPACITY 131072U

/* Provide the path exists operation used by this module and its client applications. */
static int path_exists(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0;
}

/* Provide the is directory operation used by this module and its client applications. */
static int is_directory(const char *path)
{
    struct stat info;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || stat(path, &info) != 0) return 0;
    return S_ISDIR(info.st_mode) != 0;
}

/* Provide the join path operation used by this module and its client applications. */
static void join_path(
    char *out,
    size_t capacity,
    const char *left,
    const char *right
)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL) left = "";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) right = "";

    length = strlen(left);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U &&
        (left[length - 1U] == '/' || left[length - 1U] == '\\')) {
        (void)snprintf(out, capacity, "%s%s", left, right);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)snprintf(out, capacity, "%s/%s", left, right);
    }
}

/* Provide the make directory operation used by this module and its client applications. */
static UmiStatus make_directory(const char *path, size_t *created)
{
    char work[UMI_PATH_CAPACITY];
    size_t index;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)snprintf(work, sizeof(work), "%s", path);
    length = strlen(work);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (work[index] == '/' || work[index] == '\\') {
            char saved = work[index];
            work[index] = '\0';

            /* Apply this branch only when its contract condition is satisfied. */
            if (work[0] != '\0' && !path_exists(work)) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (UMI_MKDIR(work) != 0 && errno != EEXIST) {
                    return UMI_STATUS_IO_ERROR;
                }
                /*
                 * Protect caller-owned memory by checking that required state is available before it is
                 * used.
                 */
                if (created != NULL) *created += 1U;
            }

            work[index] = saved;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!path_exists(work)) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (UMI_MKDIR(work) != 0 && errno != EEXIST) {
            return UMI_STATUS_IO_ERROR;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (created != NULL) *created += 1U;
    }

    return UMI_STATUS_OK;
}

/* Provide the copy file operation used by this module and its client applications. */
static UmiStatus copy_file(const char *source, const char *destination)
{
    FILE *input;
    FILE *output;
    unsigned char buffer[8192];
    size_t count;

    input = fopen(source, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL) return UMI_STATUS_IO_ERROR;

    output = fopen(destination, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL) {
        fclose(input);
        return UMI_STATUS_IO_ERROR;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((count = fread(buffer, 1U, sizeof(buffer), input)) > 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (fwrite(buffer, 1U, count, output) != count) {
            fclose(input);
            fclose(output);
            return UMI_STATUS_IO_ERROR;
        }
    }

    fclose(input);
    fclose(output);
    return UMI_STATUS_OK;
}

/* Provide the copy tree operation used by this module and its client applications. */
static UmiStatus copy_tree(const char *source, const char *destination)
{
    DIR *directory;
    struct dirent *entry;
    UmiStatus status;

    status = make_directory(destination, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    directory = opendir(source);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (directory == NULL) return UMI_STATUS_IO_ERROR;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((entry = readdir(directory)) != NULL) {
        char source_path[UMI_PATH_CAPACITY];
        char destination_path[UMI_PATH_CAPACITY];

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        join_path(source_path, sizeof(source_path), source, entry->d_name);
        join_path(
            destination_path,
            sizeof(destination_path),
            destination,
            entry->d_name
        );

        /* Apply this branch only when its contract condition is satisfied. */
        if (is_directory(source_path)) {
            status = copy_tree(source_path, destination_path);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = copy_file(source_path, destination_path);
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            closedir(directory);
            return status;
        }
    }

    closedir(directory);
    return UMI_STATUS_OK;
}

/* Provide the read text file operation used by this module and its client applications. */
static UmiStatus read_text_file(
    const char *path,
    char *out_text,
    size_t capacity
)
{
    FILE *file;
    size_t count;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return UMI_STATUS_IO_ERROR;

    count = fread(out_text, 1U, capacity - 1U, file);
    out_text[count] = '\0';

    /* Apply this branch only when its contract condition is satisfied. */
    if (!feof(file)) {
        fclose(file);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    fclose(file);
    return UMI_STATUS_OK;
}

/* Provide the write text file operation used by this module and its client applications. */
static UmiStatus write_text_file(
    const char *path,
    const char *text,
    size_t *created
)
{
    FILE *file;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path_exists(path)) return UMI_STATUS_ALREADY_EXISTS;

    file = fopen(path, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return UMI_STATUS_IO_ERROR;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text != NULL && fputs(text, file) == EOF) {
        fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    fclose(file);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (created != NULL) *created += 1U;
    return UMI_STATUS_OK;
}

/* Provide the replace all operation used by this module and its client applications. */
static UmiStatus replace_all(
    char *text,
    size_t capacity,
    const char *token,
    const char *replacement
)
{
    char buffer[UMI_TEMPLATE_CAPACITY];
    char *cursor;
    char *match;
    size_t used = 0U;
    size_t token_length;
    size_t replacement_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || token == NULL || replacement == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token_length = strlen(token);
    replacement_length = strlen(replacement);
    cursor = text;
    buffer[0] = '\0';

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((match = strstr(cursor, token)) != NULL) {
        size_t prefix_length = (size_t)(match - cursor);

        /* Apply this branch only when its contract condition is satisfied. */
        if (used + prefix_length + replacement_length + 1U > sizeof(buffer)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        memcpy(buffer + used, cursor, prefix_length);
        used += prefix_length;
        memcpy(buffer + used, replacement, replacement_length);
        used += replacement_length;
        cursor = match + token_length;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (used + strlen(cursor) + 1U > sizeof(buffer)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(buffer + used, sizeof(buffer) - used, "%s", cursor);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(buffer) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(text, capacity, "%s", buffer);
    return UMI_STATUS_OK;
}

/* Provide the normalise slug operation used by this module and its client applications. */
static void normalise_slug(
    const char *name,
    char *slug,
    size_t capacity
)
{
    size_t write_index = 0U;
    int previous_dash = 0;
    size_t index;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         name != NULL && name[index] != '\0' && write_index + 1U < capacity;
         ++index) {
        unsigned char value = (unsigned char)name[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (isalnum(value)) {
            slug[write_index++] = (char)tolower(value);
            previous_dash = 0;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!previous_dash && write_index > 0U) {
            slug[write_index++] = '-';
            previous_dash = 1;
        }
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (write_index > 0U && slug[write_index - 1U] == '-') {
        --write_index;
    }

    slug[write_index] = '\0';
}

/* Provide the make identifier operation used by this module and its client applications. */
static void make_identifier(
    const char *slug,
    char *identifier,
    size_t capacity
)
{
    size_t index;
    size_t write_index = 0U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         slug != NULL && slug[index] != '\0' && write_index + 1U < capacity;
         ++index) {
        identifier[write_index++] = slug[index] == '-' ? '_' : slug[index];
    }

    identifier[write_index] = '\0';
}

/*
 * Copy uppercase into module-owned storage so callers keep ownership of their input
 * values.
 */
static void uppercase_copy(
    const char *source,
    char *destination,
    size_t capacity
)
{
    size_t index;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         source != NULL && source[index] != '\0' && index + 1U < capacity;
         ++index) {
        destination[index] = (char)toupper((unsigned char)source[index]);
    }

    destination[index] = '\0';
}

/*
 * Provide the create directory relative operation used by this module and its client
 * applications.
 */
static UmiStatus create_directory_relative(
    const char *root,
    const char *relative,
    UmiScaffoldReport *report
)
{
    char path[UMI_PATH_CAPACITY];

    join_path(path, sizeof(path), root, relative);
    return make_directory(
        path,
        report != NULL ? &report->directories_created : NULL
    );
}

/* Provide the render template operation used by this module and its client applications. */
static UmiStatus render_template(
    const UmiScaffoldRequest *request,
    UmiScaffoldReport *report,
    const char *template_name,
    const char *destination_relative,
    const char *slug,
    const char *identifier,
    const char *identifier_upper,
    const char *frontends_yaml,
    const char *console_cmake,
    const char *gtk_cmake,
    const char *web_cmake
)
{
    char template_path[UMI_PATH_CAPACITY];
    char destination_path[UMI_PATH_CAPACITY];
    char application_templates[UMI_PATH_CAPACITY];
    char text[UMI_TEMPLATE_CAPACITY];
    UmiStatus status;

    join_path(
        application_templates,
        sizeof(application_templates),
        request->workspace_root,
        "templates/application"
    );
    join_path(
        template_path,
        sizeof(template_path),
        application_templates,
        template_name
    );
    join_path(
        destination_path,
        sizeof(destination_path),
        request->workspace_root,
        destination_relative
    );

    status = read_text_file(template_path, text, sizeof(text));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{FRONTENDS_YAML}}", frontends_yaml)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{CONSOLE_CMAKE}}", console_cmake)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{GTK_CMAKE}}", gtk_cmake)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{WEB_CMAKE}}", web_cmake)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{NAME}}", request->application_name)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{SLUG}}", slug)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{IDENT}}", identifier)) != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = replace_all(text, sizeof(text), "{{IDENT_UPPER}}", identifier_upper)) != UMI_STATUS_OK) return status;

    return write_text_file(
        destination_path,
        text,
        report != NULL ? &report->files_created : NULL
    );
}

/*
 * Provide the scaffold is workspace operation used by this module and its client
 * applications.
 */
int umi_scaffold_is_workspace(const char *path)
{
    char marker[UMI_PATH_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return 0;

    join_path(marker, sizeof(marker), path, ".umicom-root");
    return path_exists(marker);
}

/*
 * Provide the scaffold copy workspace template operation used by this module and its
 * client applications.
 */
UmiStatus umi_scaffold_copy_workspace_template(
    const char *template_root,
    const char *destination_root
)
{
    char source[UMI_PATH_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (template_root == NULL || destination_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (path_exists(destination_root)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    join_path(source, sizeof(source), template_root, "workspace");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!is_directory(source)) return UMI_STATUS_NOT_FOUND;

    return copy_tree(source, destination_root);
}

/*
 * Provide the scaffold create application operation used by this module and its client
 * applications.
 */
UmiStatus umi_scaffold_create_application(
    const UmiScaffoldRequest *request,
    UmiScaffoldReport *out_report
)
{
    char slug[128];
    char identifier[128];
    char identifier_upper[128];
    char application_root[UMI_PATH_CAPACITY];
    char relative[UMI_PATH_CAPACITY];
    char frontends_yaml[256] = "";
    const char *console_cmake = "";
    const char *gtk_cmake = "";
    const char *web_cmake = "";
    UmiScaffoldReport report;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request->workspace_root == NULL ||
        request->application_name == NULL ||
        request->application_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_scaffold_is_workspace(request->workspace_root)) {
        return UMI_STATUS_NOT_FOUND;
    }

    normalise_slug(request->application_name, slug, sizeof(slug));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (slug[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;

    make_identifier(slug, identifier, sizeof(identifier));
    uppercase_copy(identifier, identifier_upper, sizeof(identifier_upper));

    (void)snprintf(
        relative,
        sizeof(relative),
        "applications/%s",
        slug
    );
    join_path(
        application_root,
        sizeof(application_root),
        request->workspace_root,
        relative
    );

    /* Apply this branch only when its contract condition is satisfied. */
    if (path_exists(application_root)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    memset(&report, 0, sizeof(report));
    (void)snprintf(
        report.application_id,
        sizeof(report.application_id),
        "org.umicom.%.116s",
        identifier
    );
    (void)snprintf(
        report.application_slug,
        sizeof(report.application_slug),
        "%s",
        slug
    );
    (void)snprintf(
        report.application_directory,
        sizeof(report.application_directory),
        "%s/applications/%s",
        request->workspace_root,
        slug
    );

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/include/umicom/%s", slug, identifier);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/src", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/tests", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_CONSOLE) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/console", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(frontends_yaml + strlen(frontends_yaml), sizeof(frontends_yaml) - strlen(frontends_yaml), "\n  - console");
        console_cmake =
            "add_executable({{SLUG}}-console src/console/main.c)\n"
            "target_link_libraries({{SLUG}}-console PRIVATE {{IDENT}}_core)\n"
            "umicom_apply_warnings({{SLUG}}-console)\n\n";
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_GTK4) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/gtk", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(frontends_yaml + strlen(frontends_yaml), sizeof(frontends_yaml) - strlen(frontends_yaml), "\n  - gtk");
        gtk_cmake =
            "if(UMICOM_BUILD_GTK4 AND TARGET PkgConfig::GTK4)\n"
            "    add_executable({{SLUG}}-gtk src/gtk/main.c)\n"
            "    target_link_libraries({{SLUG}}-gtk PRIVATE {{IDENT}}_core PkgConfig::GTK4)\n"
            "    umicom_apply_warnings({{SLUG}}-gtk)\n"
            "endif()\n\n";
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_WEB) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/web", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(relative, sizeof(relative), "applications/%s/web/static", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(frontends_yaml + strlen(frontends_yaml), sizeof(frontends_yaml) - strlen(frontends_yaml), "\n  - web");
        web_cmake =
            "add_executable({{SLUG}}-web src/web/main.c)\n"
            "target_link_libraries({{SLUG}}-web PRIVATE {{IDENT}}_core)\n"
            "if(WIN32)\n"
            "    target_link_libraries({{SLUG}}-web PRIVATE ws2_32)\n"
            "endif()\n"
            "umicom_apply_warnings({{SLUG}}-web)\n\n";
    }

    (void)snprintf(relative, sizeof(relative), "applications/%s/include/umicom/%s/application.h", slug, identifier);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "application.h.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/src/application.c", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "application.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_CONSOLE) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/console/main.c", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = render_template(request, &report, "console_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_GTK4) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/gtk/main.c", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = render_template(request, &report, "gtk_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_WEB) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/web/main.c", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = render_template(request, &report, "web_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
        (void)snprintf(relative, sizeof(relative), "applications/%s/web/static/index.html", slug);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = render_template(request, &report, "index.html.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    (void)snprintf(relative, sizeof(relative), "applications/%s/tests/test_application.c", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "test_application.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/application.umicom.yaml", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "application.umicom.yaml.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/CMakeLists.txt", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "CMakeLists.txt.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/README.md", slug);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((status = render_template(request, &report, "README.md.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_report != NULL) *out_report = report;
    return UMI_STATUS_OK;
}
