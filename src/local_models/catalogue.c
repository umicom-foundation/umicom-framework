/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/catalogue.c
 *
 * PURPOSE:
 *   Provide a small immutable example catalogue and strict native model source validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>

/* Publisher metadata pins the bytes, not this engine's support or a machine's
 * available memory. Keep provenance in docs/local-model-manager.md. Adding an
 * entry requires reviewing the model licence and its full file digest. */
static const UmiModelCatalogueEntry catalogue[] = {
    {"qwen2.5-0.5b-q4", "Qwen2.5 0.5B Instruct Q4_K_M", "Apache-2.0",
     {"Qwen/Qwen2.5-0.5B-Instruct-GGUF",
      "9217f5db79a29953eb74d5343926648285ec7e67",
      "qwen2.5-0.5b-instruct-q4_k_m.gguf",
      "74a4da8c9fdbcd15bd1f6d01d621410d31c6fc00986f5eb687824e7b93d7a9db",
      UINT64_C(491400032)}}
};
size_t UmiModelCatalogueCount(void) { return sizeof(catalogue) / sizeof(catalogue[0]); }
const UmiModelCatalogueEntry *UmiModelCatalogueAt(size_t index)
{ return index < UmiModelCatalogueCount() ? &catalogue[index] : NULL; }
const UmiModelCatalogueEntry *UmiModelCatalogueFind(const char *id)
{
    if (id == NULL) return NULL;
    for (size_t i = 0U; i < UmiModelCatalogueCount(); ++i)
        if (strcmp(id, catalogue[i].id) == 0) return &catalogue[i];
    return NULL;
}
bool UmiModelTextFits(const char *text, size_t capacity)
{ return text != NULL && memchr(text, '\0', capacity) != NULL; }
bool UmiModelDigestValid(const char *text, size_t digits)
{
    if (!UmiModelTextFits(text, digits + 1U) || strlen(text) != digits) return false;
    for (size_t i = 0U; i < digits; ++i)
        if (!((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'a' && text[i] <= 'f'))) return false;
    return true;
}
bool UmiModelNameValid(const char *text, size_t capacity, bool paths)
{
    if (!UmiModelTextFits(text, capacity) || text[0] == '\0') return false;
    size_t segment = 0U;
    for (size_t i = 0U;; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U || (paths && c == '/')) {
            size_t length = i - segment;
            if (length == 0U || (length == 1U && text[segment] == '.') ||
                (length == 2U && text[segment] == '.' && text[segment + 1U] == '.')) return false;
            if (c == 0U) return true;
            segment = i + 1U;
        } else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.')) return false;
    }
}
bool UmiModelNumber(const char *text, uint64_t minimum, uint64_t maximum, uint64_t *out)
{
    if (text == NULL || text[0] == '\0' || out == NULL) return false;
    uint64_t value = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
        unsigned digit = (unsigned)(text[i] - '0');
        if (value > maximum / 10U || (value == maximum / 10U && digit > maximum % 10U)) return false;
        value = value * 10U + digit;
    }
    if (value < minimum) return false;
    *out = value;
    return true;
}
UmiStatus UmiModelSourceValidate(const UmiModelSource *source)
{
    if (source == NULL || !UmiModelNameValid(source->repository, sizeof(source->repository), true) ||
        !UmiModelNameValid(source->filename, sizeof(source->filename), true) ||
        !UmiModelDigestValid(source->revision, 40U) || !UmiModelDigestValid(source->sha256, 64U) ||
        source->bytes < 24U || source->bytes > UMI_MODEL_MAX_BYTES) return UMI_STATUS_INVALID_ARGUMENT;
    const char *slash = strchr(source->repository, '/');
    if (slash == NULL || strchr(slash + 1, '/') != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(source->filename);
    return length > 5U && strcmp(source->filename + length - 5U, ".gguf") == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
