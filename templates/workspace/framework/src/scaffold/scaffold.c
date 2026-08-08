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

static int path_exists(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0;
}

static int is_directory(const char *path)
{
    struct stat info;
    if (path == NULL || stat(path, &info) != 0) return 0;
    return S_ISDIR(info.st_mode) != 0;
}

static void join_path(
    char *out,
    size_t capacity,
    const char *left,
    const char *right
)
{
    size_t length;

    if (out == NULL || capacity == 0U) return;
    if (left == NULL) left = "";
    if (right == NULL) right = "";

    length = strlen(left);
    if (length > 0U &&
        (left[length - 1U] == '/' || left[length - 1U] == '\\')) {
        (void)snprintf(out, capacity, "%s%s", left, right);
    } else {
        (void)snprintf(out, capacity, "%s/%s", left, right);
    }
}

static UmiStatus make_directory(const char *path, size_t *created)
{
    char work[UMI_PATH_CAPACITY];
    size_t index;
    size_t length;

    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)snprintf(work, sizeof(work), "%s", path);
    length = strlen(work);

    for (index = 1U; index < length; ++index) {
        if (work[index] == '/' || work[index] == '\\') {
            char saved = work[index];
            work[index] = '\0';

            if (work[0] != '\0' && !path_exists(work)) {
                if (UMI_MKDIR(work) != 0 && errno != EEXIST) {
                    return UMI_STATUS_IO_ERROR;
                }
                if (created != NULL) *created += 1U;
            }

            work[index] = saved;
        }
    }

    if (!path_exists(work)) {
        if (UMI_MKDIR(work) != 0 && errno != EEXIST) {
            return UMI_STATUS_IO_ERROR;
        }
        if (created != NULL) *created += 1U;
    }

    return UMI_STATUS_OK;
}

static UmiStatus copy_file(const char *source, const char *destination)
{
    FILE *input;
    FILE *output;
    unsigned char buffer[8192];
    size_t count;

    input = fopen(source, "rb");
    if (input == NULL) return UMI_STATUS_IO_ERROR;

    output = fopen(destination, "wb");
    if (output == NULL) {
        fclose(input);
        return UMI_STATUS_IO_ERROR;
    }

    while ((count = fread(buffer, 1U, sizeof(buffer), input)) > 0U) {
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

static UmiStatus copy_tree(const char *source, const char *destination)
{
    DIR *directory;
    struct dirent *entry;
    UmiStatus status;

    status = make_directory(destination, NULL);
    if (status != UMI_STATUS_OK) return status;

    directory = opendir(source);
    if (directory == NULL) return UMI_STATUS_IO_ERROR;

    while ((entry = readdir(directory)) != NULL) {
        char source_path[UMI_PATH_CAPACITY];
        char destination_path[UMI_PATH_CAPACITY];

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

        if (is_directory(source_path)) {
            status = copy_tree(source_path, destination_path);
        } else {
            status = copy_file(source_path, destination_path);
        }

        if (status != UMI_STATUS_OK) {
            closedir(directory);
            return status;
        }
    }

    closedir(directory);
    return UMI_STATUS_OK;
}

static UmiStatus read_text_file(
    const char *path,
    char *out_text,
    size_t capacity
)
{
    FILE *file;
    size_t count;

    if (path == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;

    count = fread(out_text, 1U, capacity - 1U, file);
    out_text[count] = '\0';

    if (!feof(file)) {
        fclose(file);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    fclose(file);
    return UMI_STATUS_OK;
}

static UmiStatus write_text_file(
    const char *path,
    const char *text,
    size_t *created
)
{
    FILE *file;

    if (path_exists(path)) return UMI_STATUS_ALREADY_EXISTS;

    file = fopen(path, "wb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;

    if (text != NULL && fputs(text, file) == EOF) {
        fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    fclose(file);
    if (created != NULL) *created += 1U;
    return UMI_STATUS_OK;
}

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

    if (text == NULL || token == NULL || replacement == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token_length = strlen(token);
    replacement_length = strlen(replacement);
    cursor = text;
    buffer[0] = '\0';

    while ((match = strstr(cursor, token)) != NULL) {
        size_t prefix_length = (size_t)(match - cursor);

        if (used + prefix_length + replacement_length + 1U > sizeof(buffer)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        memcpy(buffer + used, cursor, prefix_length);
        used += prefix_length;
        memcpy(buffer + used, replacement, replacement_length);
        used += replacement_length;
        cursor = match + token_length;
    }

    if (used + strlen(cursor) + 1U > sizeof(buffer)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(buffer + used, sizeof(buffer) - used, "%s", cursor);

    if (strlen(buffer) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(text, capacity, "%s", buffer);
    return UMI_STATUS_OK;
}

static void normalise_slug(
    const char *name,
    char *slug,
    size_t capacity
)
{
    size_t write_index = 0U;
    int previous_dash = 0;
    size_t index;

    if (capacity == 0U) return;

    for (index = 0U;
         name != NULL && name[index] != '\0' && write_index + 1U < capacity;
         ++index) {
        unsigned char value = (unsigned char)name[index];

        if (isalnum(value)) {
            slug[write_index++] = (char)tolower(value);
            previous_dash = 0;
        } else if (!previous_dash && write_index > 0U) {
            slug[write_index++] = '-';
            previous_dash = 1;
        }
    }

    while (write_index > 0U && slug[write_index - 1U] == '-') {
        --write_index;
    }

    slug[write_index] = '\0';
}

static void make_identifier(
    const char *slug,
    char *identifier,
    size_t capacity
)
{
    size_t index;
    size_t write_index = 0U;

    if (capacity == 0U) return;

    for (index = 0U;
         slug != NULL && slug[index] != '\0' && write_index + 1U < capacity;
         ++index) {
        identifier[write_index++] = slug[index] == '-' ? '_' : slug[index];
    }

    identifier[write_index] = '\0';
}

static void uppercase_copy(
    const char *source,
    char *destination,
    size_t capacity
)
{
    size_t index;

    if (capacity == 0U) return;

    for (index = 0U;
         source != NULL && source[index] != '\0' && index + 1U < capacity;
         ++index) {
        destination[index] = (char)toupper((unsigned char)source[index]);
    }

    destination[index] = '\0';
}

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
    if (status != UMI_STATUS_OK) return status;

    if ((status = replace_all(text, sizeof(text), "{{FRONTENDS_YAML}}", frontends_yaml)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{CONSOLE_CMAKE}}", console_cmake)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{GTK_CMAKE}}", gtk_cmake)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{WEB_CMAKE}}", web_cmake)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{NAME}}", request->application_name)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{SLUG}}", slug)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{IDENT}}", identifier)) != UMI_STATUS_OK) return status;
    if ((status = replace_all(text, sizeof(text), "{{IDENT_UPPER}}", identifier_upper)) != UMI_STATUS_OK) return status;

    return write_text_file(
        destination_path,
        text,
        report != NULL ? &report->files_created : NULL
    );
}

int umi_scaffold_is_workspace(const char *path)
{
    char marker[UMI_PATH_CAPACITY];

    if (path == NULL) return 0;

    join_path(marker, sizeof(marker), path, ".umicom-root");
    return path_exists(marker);
}

UmiStatus umi_scaffold_copy_workspace_template(
    const char *template_root,
    const char *destination_root
)
{
    char source[UMI_PATH_CAPACITY];

    if (template_root == NULL || destination_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (path_exists(destination_root)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    join_path(source, sizeof(source), template_root, "workspace");
    if (!is_directory(source)) return UMI_STATUS_NOT_FOUND;

    return copy_tree(source, destination_root);
}

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

    if (request == NULL || request->workspace_root == NULL ||
        request->application_name == NULL ||
        request->application_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_scaffold_is_workspace(request->workspace_root)) {
        return UMI_STATUS_NOT_FOUND;
    }

    normalise_slug(request->application_name, slug, sizeof(slug));
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

    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/include/umicom/%s", slug, identifier);
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/src", slug);
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/tests", slug);
    if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_CONSOLE) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/console", slug);
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(frontends_yaml + strlen(frontends_yaml), sizeof(frontends_yaml) - strlen(frontends_yaml), "\n  - console");
        console_cmake =
            "add_executable({{SLUG}}-console src/console/main.c)\n"
            "target_link_libraries({{SLUG}}-console PRIVATE {{IDENT}}_core)\n"
            "umicom_apply_warnings({{SLUG}}-console)\n\n";
    }

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_GTK4) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/gtk", slug);
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(frontends_yaml + strlen(frontends_yaml), sizeof(frontends_yaml) - strlen(frontends_yaml), "\n  - gtk");
        gtk_cmake =
            "if(UMICOM_BUILD_GTK4 AND TARGET PkgConfig::GTK4)\n"
            "    add_executable({{SLUG}}-gtk src/gtk/main.c)\n"
            "    target_link_libraries({{SLUG}}-gtk PRIVATE {{IDENT}}_core PkgConfig::GTK4)\n"
            "    umicom_apply_warnings({{SLUG}}-gtk)\n"
            "endif()\n\n";
    }

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_WEB) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/web", slug);
        if ((status = create_directory_relative(request->workspace_root, relative, &report)) != UMI_STATUS_OK) return status;
        (void)snprintf(relative, sizeof(relative), "applications/%s/web/static", slug);
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
    if ((status = render_template(request, &report, "application.h.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/src/application.c", slug);
    if ((status = render_template(request, &report, "application.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_CONSOLE) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/console/main.c", slug);
        if ((status = render_template(request, &report, "console_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_GTK4) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/gtk/main.c", slug);
        if ((status = render_template(request, &report, "gtk_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    if ((request->frontends & UMI_SCAFFOLD_FRONTEND_WEB) != 0U) {
        (void)snprintf(relative, sizeof(relative), "applications/%s/src/web/main.c", slug);
        if ((status = render_template(request, &report, "web_main.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
        (void)snprintf(relative, sizeof(relative), "applications/%s/web/static/index.html", slug);
        if ((status = render_template(request, &report, "index.html.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;
    }

    (void)snprintf(relative, sizeof(relative), "applications/%s/tests/test_application.c", slug);
    if ((status = render_template(request, &report, "test_application.c.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/application.umicom.yaml", slug);
    if ((status = render_template(request, &report, "application.umicom.yaml.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/CMakeLists.txt", slug);
    if ((status = render_template(request, &report, "CMakeLists.txt.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    (void)snprintf(relative, sizeof(relative), "applications/%s/README.md", slug);
    if ((status = render_template(request, &report, "README.md.in", relative, slug, identifier, identifier_upper, frontends_yaml, console_cmake, gtk_cmake, web_cmake)) != UMI_STATUS_OK) return status;

    if (out_report != NULL) *out_report = report;
    return UMI_STATUS_OK;
}
