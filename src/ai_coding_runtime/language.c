/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/language.c
 *
 * PURPOSE:
 *   Implement extension/name based source-language detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static const char *basename_of(const char *path)
{
    const char *base = path;
    const char *cursor;

    if (path == NULL) return NULL;

    for (cursor = path; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') base = cursor + 1;
    }

    return base;
}

const char *umi_ai_coding_runtime_language_for_path(const char *path)
{
    const char *extension;
    const char *base;
    size_t index;

    if (path == NULL) return "plaintext";

    base = basename_of(path);
    if (base != NULL) {
        if (strcmp(base, "CMakeLists.txt") == 0) return "cmake";
        if (strcmp(base, "Makefile") == 0 ||
            strcmp(base, "GNUmakefile") == 0) return "makefile";
        if (strcmp(base, "Dockerfile") == 0) return "dockerfile";
    }

    extension = umi_ai_coding_runtime_path_extension(path);
    if (extension == NULL) return "plaintext";

    for (index = 0U; index < sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]);
         ++index) {
        if (strcmp(extension, EXTENSIONS[index].extension) == 0) {
            return EXTENSIONS[index].language;
        }
    }

    return "plaintext";
}

int umi_ai_coding_runtime_path_is_text_source(const char *path)
{
    const char *language = umi_ai_coding_runtime_language_for_path(path);
    return strcmp(language, "plaintext") != 0 ||
        (umi_ai_coding_runtime_path_extension(path) != NULL &&
         strcmp(umi_ai_coding_runtime_path_extension(path), ".txt") == 0);
}
