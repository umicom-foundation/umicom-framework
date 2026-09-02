/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/language.c
 *
 * PURPOSE:
 *   Implement extension/name based source-language detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/language.h"

#include <string.h>

typedef struct LanguageExtension {
    const char *extension;
    const char *language;
} LanguageExtension;

static const LanguageExtension EXTENSIONS[] = {
    {".c", "c"}, {".h", "c"}, {".cc", "cpp"}, {".cpp", "cpp"},
    {".cxx", "cpp"}, {".hpp", "cpp"}, {".hh", "cpp"}, {".hxx", "cpp"},
    {".s", "assembly"}, {".S", "assembly"}, {".asm", "assembly"},
    {".py", "python"}, {".pyi", "python"}, {".rs", "rust"},
    {".zig", "zig"}, {".java", "java"}, {".php", "php"},
    {".js", "javascript"}, {".jsx", "javascriptreact"},
    {".ts", "typescript"}, {".tsx", "typescriptreact"},
    {".html", "html"}, {".htm", "html"}, {".css", "css"},
    {".scss", "scss"}, {".less", "less"}, {".md", "markdown"},
    {".markdown", "markdown"}, {".json", "json"}, {".jsonc", "jsonc"},
    {".yaml", "yaml"}, {".yml", "yaml"}, {".sh", "bash"},
    {".bash", "bash"}, {".sql", "sql"}, {".xml", "xml"},
    {".toml", "toml"}, {".ini", "ini"}, {".txt", "plaintext"},
    {".cmake", "cmake"}, {".ps1", "powershell"}, {".bat", "bat"},
    {".cmd", "bat"}, {".cs", "csharp"}, {".go", "go"},
    {".swift", "swift"}, {".kt", "kotlin"}, {".kts", "kotlin"},
    {".lua", "lua"}, {".rb", "ruby"}, {".r", "r"}
};

/* Provide the basename of operation used by this module and its client applications. */
static const char *basename_of(const char *path)
{
    const char *base = path;
    const char *cursor;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = path; *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\') base = cursor + 1;
    }

    return base;
}

/*
 * Provide the ai coding runtime language for path operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_runtime_language_for_path(const char *path)
{
    const char *extension;
    const char *base;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return "plaintext";

    base = basename_of(path);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base != NULL) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(base, "CMakeLists.txt") == 0) return "cmake";
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(base, "Makefile") == 0 ||
            strcmp(base, "GNUmakefile") == 0) return "makefile";
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(base, "Dockerfile") == 0) return "dockerfile";
    }

    extension = umi_ai_coding_runtime_path_extension(path);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (extension == NULL) return "plaintext";

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(extension, EXTENSIONS[index].extension) == 0) {
            return EXTENSIONS[index].language;
        }
    }

    return "plaintext";
}

/*
 * Provide the ai coding runtime path is text source operation used by this module and its
 * client applications.
 */
int umi_ai_coding_runtime_path_is_text_source(const char *path)
{
    const char *language = umi_ai_coding_runtime_language_for_path(path);
    return strcmp(language, "plaintext") != 0 ||
        (umi_ai_coding_runtime_path_extension(path) != NULL &&
         strcmp(umi_ai_coding_runtime_path_extension(path), ".txt") == 0);
}
