/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/portfolio.c
 *
 * PURPOSE:
 *   Preserve whole-portfolio experience, recipe and thin-module source conformance checks.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SC_PORTFOLIO_MAX 64U
#define SC_PORTFOLIO_TEXT 256U
static const char recipeRoot[] = "framework/src/application/component/recipes/";

typedef struct ScExperience {
    char id[SC_PORTFOLIO_TEXT];
    char getter[SC_PORTFOLIO_TEXT];
} ScExperience;
typedef struct ScAlias { char alias[SC_PORTFOLIO_TEXT], canonical[SC_PORTFOLIO_TEXT]; } ScAlias;

static int readSource(const char *root, const char *relative, ScText *out,
    UmiSourceContractReport *report)
{
    char *path = ScJoin(root, relative), *text = path != NULL ? ScRead(path) : NULL;
    bool cmake = strstr(relative, "CMakeLists.txt") != NULL || strstr(relative, ".cmake") != NULL;
    int result = text != NULL ? ScLex(out, text, cmake) : 2;
    if (result != 0) ScFinding(report, relative, 1U, "portfolio.input", "Required portfolio source is unavailable or malformed", true);
    else ++report->files_checked;
    free(path); free(text); return result;
}
static void require(bool condition, const char *path, const char *detail,
    UmiSourceContractReport *report)
{
    ++report->checks;
    if (!condition) ScFinding(report, path, 1U, "all_24_products_have_catalogued_experiences_and_standard_recipes", detail, false);
}
static bool tokenString(const ScText *text, size_t token, char *out, size_t capacity)
{
    if (token >= text->count || text->tokens[token].kind != '"') return false;
    ScToken t = text->tokens[token];
    size_t n = t.end - t.begin - 2U;
    /* These are canonical ASCII identifiers, not arbitrary user strings. */
    if (n == 0U || n >= capacity || memchr(text->data + t.begin + 1U, '\\', n) != NULL) return false;
    memcpy(out, text->data + t.begin + 1U, n); out[n] = '\0'; return true;
}
static bool scalar(const char *text, const char *key, char *out, size_t capacity)
{
    for (const char *line = text; line != NULL && *line != '\0';) {
        const char *end = strchr(line, '\n'); if (end == NULL) end = line + strlen(line);
        const char *p = line;
        while (p < end && (*p == ' ' || *p == '\t')) ++p;
        size_t n = strlen(key);
        if ((size_t)(end - p) > n && memcmp(p, key, n) == 0 && p[n] == ':') {
            p += n + 1U; while (p < end && isspace((unsigned char)*p)) ++p;
            const char *last = end;
            while (last > p && isspace((unsigned char)last[-1])) --last;
            size_t bytes = (size_t)(last - p);
            if (bytes == 0U || bytes >= capacity) return false;
            memcpy(out, p, bytes); out[bytes] = '\0'; return true;
        }
        line = *end == '\n' ? end + 1 : NULL;
    }
    return false;
}
static bool dedicated(const char *name)
{
    static const char *const entries[] = {"desktop","studio","trader","bank","tms","music"};
    for (size_t i = 0U; i < sizeof(entries) / sizeof(entries[0]); ++i)
        if (strcmp(entries[i], name) == 0) return true;
    return false;
}
static bool safeName(const char *name)
{
    if (name[0] == '\0') return false;
    for (const unsigned char *p = (const unsigned char *)name; *p != '\0'; ++p)
        if (!isalnum(*p) && *p != '-' && *p != '_') return false;
    return true;
}
int ScPortfolio(const char *root, UmiSourceContractReport *report)
{
    size_t before = report->findings;
    ScText catalogue = {0}, includes = {0}, records = {0}, modules = {0}, extended = {0};
    ScExperience experiences[SC_PORTFOLIO_MAX] = {0}; size_t experienceCount = 0U;
    ScAlias aliases[SC_PORTFOLIO_MAX] = {0}; size_t aliasCount = 0U;
    ScFiles manifests = {0};
    const char *cat = "framework/src/application/experience_catalogue.c";
    if (readSource(root, cat, &catalogue, report) != 0) goto done;
    size_t getters = ScIndex(&catalogue, "GETTERS [ ] = {", 0U);
    require(getters != SIZE_MAX, cat, "Missing canonical experience getter array", report);
    if (getters == SIZE_MAX) goto done;
    size_t end = ScMatching(&catalogue, getters + 4U);
    if (end == SIZE_MAX) { ScFinding(report, cat, 1U, "portfolio.getters", "Unbalanced getter array", true); goto done; }
    for (size_t i = getters + 5U; i < end; ++i) {
        const char *prefix = "umi_application_experience_";
        ScToken token = catalogue.tokens[i]; size_t prefixLength = strlen(prefix);
        if (!ScIdentifier(&catalogue, i) || token.end - token.begin <= prefixLength || memcmp(catalogue.data + token.begin, prefix, prefixLength) != 0) continue;
        if (experienceCount == SC_PORTFOLIO_MAX) { ScFinding(report, cat, 1U, "portfolio.capacity", "Experience capacity exceeded", true); goto done; }
        ScExperience *entry = &experiences[experienceCount++];
        size_t n = token.end - token.begin - prefixLength;
        if (n >= sizeof(entry->getter)) { ScFinding(report, cat, 1U, "portfolio.getter", "Getter name too long", true); goto done; }
        memcpy(entry->getter, catalogue.data + token.begin + prefixLength, n);
        entry->getter[n] = '\0';
        char relative[512];
        (void)snprintf(relative, sizeof(relative), "framework/src/application/experiences/%s.c", entry->getter);
        ScText experience = {0};
        if (readSource(root, relative, &experience, report) != 0) continue;
        size_t def = ScIndex(&experience, "DEFINITION = {", 0U);
        size_t close = def == SIZE_MAX ? SIZE_MAX : ScMatching(&experience, def + 2U);
        char values[3][SC_PORTFOLIO_TEXT] = {{0}}; size_t strings = 0U;
        if (close != SIZE_MAX) {
            for (size_t j = def + 3U; j < close && strings < 3U; ++j)
                if (experience.tokens[j].kind == '"') {
                    if (!tokenString(&experience, j, values[strings], sizeof(values[strings]))) break;
                    ++strings;
                }
        }
        require(strings == 3U, relative, "Experience needs identity, title and default layout", report);
        if (strings == 3U) {
            memcpy(entry->id, values[0], sizeof(entry->id));
            bool layoutFound = false;
            for (size_t j = 0U; j < experience.count; ++j) {
                if (ScTokenEquals(&experience, j, "UmiExperienceLayoutDefinition") &&
                    ScTokenEquals(&experience, j + 1U, ")") && ScTokenEquals(&experience, j + 2U, ",")) {
                    char layout[SC_PORTFOLIO_TEXT];
                    if (tokenString(&experience, j + 3U, layout, sizeof(layout)) && strcmp(layout, values[2]) == 0) layoutFound = true;
                }
            }
            require(layoutFound, relative, "Default layout must be declared in this experience", report);
            for (size_t j = 0U; j + 1U < experienceCount; ++j)
                require(strcmp(experiences[j].id, entry->id) != 0, relative, "Duplicate experience identity", report);
        }
        require(ScContains(&experience, "sizeof(UmiExperiencePanelDefinition)"), relative, "Experience has no panel definition", report);
        ScTextFree(&experience);
    }
    for (size_t i = 0U; i + 4U < catalogue.count; ++i) {
        char a[SC_PORTFOLIO_TEXT], b[SC_PORTFOLIO_TEXT];
        if (ScTokenEquals(&catalogue, i, "{") && tokenString(&catalogue, i + 1U, a, sizeof(a)) &&
            ScTokenEquals(&catalogue, i + 2U, ",") && tokenString(&catalogue, i + 3U, b, sizeof(b)) &&
            ScTokenEquals(&catalogue, i + 4U, "}") && strncmp(a, "org.umicom.", 11U) == 0 && strncmp(b, "org.umicom.", 11U) == 0) {
            if (aliasCount == SC_PORTFOLIO_MAX) { ScFinding(report, cat, 1U, "portfolio.aliases", "Alias capacity exceeded", true); goto done; }
            memcpy(aliases[aliasCount].alias, a, sizeof(a)); memcpy(aliases[aliasCount++].canonical, b, sizeof(b));
        }
    }
    if (readSource(root, "framework/src/application/component/recipes/recipes.inc", &includes, report) != 0 ||
        readSource(root, "framework/src/application/component/recipes/recipe_records.inc", &records, report) != 0 ||
        readSource(root, "CMakeLists.txt", &modules, report) != 0 ||
        readSource(root, "cmake/UmicomExtendedApplicationModules.cmake", &extended, report) != 0) goto done;
    char *applicationRoot = ScJoin(root, "applications");
    if (applicationRoot == NULL || ScCollectManifests(applicationRoot, &manifests) != 0) {
        ScFinding(report, "applications", 1U, "portfolio.manifests", "Cannot enumerate application manifests", true);
        free(applicationRoot); goto done;
    }
    free(applicationRoot);
    require(manifests.count == 24U, "applications", "Expected the existing 24-product manifest portfolio", report);
    size_t sharedCount = 0U;
    for (size_t i = 0U; i < manifests.count; ++i) {
        const char *manifestPath = manifests.items[i];
        char *text = ScRead(manifestPath);
        char identity[SC_PORTFOLIO_TEXT] = {0};
        char executable[SC_PORTFOLIO_TEXT] = {0};
        char title[SC_PORTFOLIO_TEXT] = {0};
        if (text == NULL) { ScFinding(report, manifestPath, 1U, "portfolio.read", "Cannot read manifest", true); continue; }
        ++report->files_checked;
        bool fields = scalar(text, "id", identity, sizeof(identity)) && scalar(text, "name", title, sizeof(title)) && scalar(text, "executable", executable, sizeof(executable));
        free(text);
        require(fields, manifestPath, "Missing manifest identity, name or executable", report);
        if (!fields) continue;
        char canonical[SC_PORTFOLIO_TEXT]; memcpy(canonical, identity, sizeof(canonical));
        for (size_t j = 0U; j < aliasCount; ++j) if (strcmp(identity, aliases[j].alias) == 0) memcpy(canonical, aliases[j].canonical, sizeof(canonical));
        bool found = false;
        for (size_t j = 0U; j < experienceCount; ++j) if (strcmp(canonical, experiences[j].id) == 0) found = true;
        require(found, manifestPath, "Manifest identity is absent from the experience catalogue", report);
        if (strncmp(canonical, "org.umicom.", 11U) != 0 || !safeName(canonical + 11U)) {
            ScFinding(report, manifestPath, 1U, "portfolio.identity", "Invalid canonical application identity", false); continue;
        }
        char recipe[512], include[512];
        (void)snprintf(recipe, sizeof(recipe), "%s%s/standard.inc", recipeRoot, canonical + 11U);
        (void)snprintf(include, sizeof(include), "#include \"%s/standard.inc\"", canonical + 11U);
        require(ScContains(&includes, include), recipe, "Standard recipe is not included", report);
        ScText recipeText = {0};
        if (readSource(root, recipe, &recipeText, report) == 0) {
            size_t symbol = ScIndex(&recipeText, "UmiApplicationComponentRecipe", 0U);
            require(symbol != SIZE_MAX && ScIdentifier(&recipeText, symbol + 1U) && ScTokenEquals(&recipeText, symbol + 2U, "="), recipe, "Recipe object declaration is missing", report);
            if (symbol != SIZE_MAX && symbol + 1U < recipeText.count) {
                char *name = ScCompactRange(&recipeText, symbol + 1U, symbol + 2U);
                if (name != NULL) {
                    char reference[512]; (void)snprintf(reference, sizeof(reference), "&%s", name);
                    require(ScContains(&records, reference), recipe, "Recipe is not registered", report); free(name);
                } else ScFinding(report, recipe, 1U, "portfolio.memory", "Allocation failed", true);
            }
            char quoted[SC_PORTFOLIO_TEXT + 3U]; (void)snprintf(quoted, sizeof(quoted), "\"%s\"", canonical);
            require(ScContains(&recipeText, quoted), recipe, "Recipe identity differs from its application", report);
            require(ScContains(&recipeText, "UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD"), recipe, "Missing standard recipe audience", report);
            ScTextFree(&recipeText);
        }
        const char *endName = strrchr(manifestPath, '/');
#if defined(_WIN32)
        const char *back = strrchr(manifestPath, '\\');
        if (back != NULL && (endName == NULL || back > endName)) endName = back;
#endif
        if (endName == NULL) continue;
        const char *beginName = endName;
        while (beginName > manifestPath && beginName[-1] != '/' && beginName[-1] != '\\') --beginName;
        char product[SC_PORTFOLIO_TEXT]; size_t n = (size_t)(endName - beginName);
        if (n == 0U || n >= sizeof(product)) { ScFinding(report, manifestPath, 1U, "portfolio.path", "Invalid application directory", true); continue; }
        memcpy(product, beginName, n); product[n] = '\0';
        if (!dedicated(product)) {
            ++sharedCount;
            require(strcmp(identity, canonical) == 0, manifestPath, "Shared native identity cannot use a legacy alias", report);
            char modulePath[512], declaration[512];
            (void)snprintf(modulePath, sizeof(modulePath), "applications/%s/CMakeLists.txt", product);
            (void)snprintf(declaration, sizeof(declaration), "\"applications/%s\"", product);
            require(ScContains(&modules, declaration) || ScContains(&extended, declaration), modulePath, "Thin application is not enabled by the module catalogue", report);
            ScText module = {0};
            if (readSource(root, modulePath, &module, report) == 0) {
                bool library = false;
                for (size_t j = 0U; j + 2U < module.count; ++j) {
                    if (!ScTokenEquals(&module, j, "add_library") || !ScTokenEquals(&module, j + 1U, "(")) continue;
                    char *name = ScCompactRange(&module, j + 2U, j + 3U);
                    if (name != NULL) { size_t length = strlen(name); library = library || (length > 14U && strncmp(name, "umicom_", 7U) == 0 && strcmp(name + length - 7U, "_module") == 0); free(name); }
                }
                require(library, modulePath, "Missing thin module library target", report);
                ScTextFree(&module);
            }
        }
    }
    require(sharedCount == 18U, "applications", "Expected 18 shared native frontends and six dedicated frontends", report);
done:
    ScTextFree(&catalogue); ScTextFree(&includes); ScTextFree(&records); ScTextFree(&modules); ScTextFree(&extended); ScFilesFree(&manifests);
    return report->findings != before ? 1 : 0;
}
