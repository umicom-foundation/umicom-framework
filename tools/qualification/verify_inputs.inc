/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/qualification/verify_inputs.inc
 *
 * PURPOSE:
 *   Bind each declared closed-input test to its actual discovered executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The catalogue is a Framework service. This adapter only understands the
 * configured driver's two-column registry and never executes its contents. */
typedef struct ReceiptOwner { char *name; char *program; } ReceiptOwner;
static int CompareReceiptOwner(const void *left, const void *right)
{
    const ReceiptOwner *a = left, *b = right;
    return strcmp(a->name, b->name);
}
static int AbsoluteReceiptProgram(const char *program)
{
#ifdef _WIN32
    size_t length = strlen(program);
    if (length < 3U) return 0;
    int slash0 = program[0] == '/' || program[0] == '\\';
    int slash1 = program[1] == '/' || program[1] == '\\';
    int drive = (program[0] >= 'A' && program[0] <= 'Z') ||
        (program[0] >= 'a' && program[0] <= 'z');
    return (slash0 && slash1) || (drive && program[1] == ':' &&
        (program[2] == '/' || program[2] == '\\'));
#else
    return program[0] == '/';
#endif
}
static int SameReceiptProgram(const char *left, const char *right)
{
    for (;;) {
        char a = *left++, b = *right++;
#ifdef _WIN32
        /* Slash spelling is equivalent on Windows. Do not fold letter case:
         * a directory may explicitly enable case-sensitive file names. */
        if (a == '\\') a = '/';
        if (b == '\\') b = '/';
#endif
        if (a != b) return 0;
        if (a == '\0') return 1;
    }
}
static UmiStatus VerifyReceiptInputs(const char *registryFile, const char *discoveryFile)
{
    unsigned char *registry = NULL, *json = NULL;
    size_t length = 0U, jsonLength = 0U, count = 0U;
    UmiCtestCommandCatalogue *catalogue = NULL;
    ReceiptOwner *owners = NULL;
    UmiStatus status = UmiInputFileRead(registryFile, UMI_TEST_REUSE_MAX_BYTES, &registry, &length);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (memchr(registry, '\0', length) != NULL) { status = UMI_STATUS_PARSE_ERROR; goto cleanup; }
    status = UmiInputFileRead(discoveryFile, UMI_TEST_PLATFORM_CTEST_MAX_BYTES, &json, &jsonLength);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiCtestCommandCatalogueParse((const char *)json, jsonLength, &catalogue);
    if (status != UMI_STATUS_OK) goto cleanup;
    /* InputFileRead provides a trailing NUL outside length. Split only this
     * private buffer; discovery and the on-disk registry remain unchanged. */
    for (size_t i = 0U; i < length; ++i)
        if (registry[i] == '\n') ++count;
    if (length != 0U && registry[length - 1U] != '\n') ++count;
    if (count > 65536U) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto cleanup; }
    if (count != 0U) {
        owners = calloc(count, sizeof(*owners));
        if (owners == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    }
    size_t used = 0U, offset = 0U;
    while (offset < length) {
        char *line = (char *)registry + offset;
        char *newline = memchr(line, '\n', length - offset);
        size_t lineLength = newline == NULL ? length - offset : (size_t)(newline - line);
        offset += lineLength + (newline != NULL ? 1U : 0U);
        if (lineLength != 0U && line[lineLength - 1U] == '\r' && newline != NULL) --lineLength;
        line[lineLength] = '\0';
        if (lineLength == 0U || strchr(line, '\r') != NULL) { status = UMI_STATUS_PARSE_ERROR; goto cleanup; }
        char *tab = strchr(line, '\t');
        if (tab == NULL || tab == line || tab[1] == '\0' || strchr(tab + 1, '\t') != NULL) {
            status = UMI_STATUS_PARSE_ERROR; goto cleanup;
        }
        *tab = '\0';
        owners[used].name = line;
        owners[used].program = tab + 1;
        ++used;
    }
    if (used != count) { status = UMI_STATUS_PARSE_ERROR; goto cleanup; }
    if (count > 1U) qsort(owners, count, sizeof(*owners), CompareReceiptOwner);
    for (size_t i = 0U; i < count; ++i) {
        if (i != 0U && strcmp(owners[i - 1U].name, owners[i].name) == 0) {
            status = UMI_STATUS_ALREADY_EXISTS; goto cleanup;
        }
        const char *actual = NULL;
        status = UmiCtestCommandCatalogueFind(catalogue, owners[i].name, &actual);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "No discovered executable for registered test: %s\n", owners[i].name);
            goto cleanup;
        }
        if (!AbsoluteReceiptProgram(actual) ||
            !AbsoluteReceiptProgram(owners[i].program) ||
            !SameReceiptProgram(actual, owners[i].program)) {
            fprintf(stderr, "Registered owner differs from the CTest executable: %s\n", owners[i].name);
            status = UMI_STATUS_INVALID_STATE; goto cleanup;
        }
    }
    printf("Verified %zu closed-input executable bindings.\n", count);
cleanup:
    free(owners);
    UmiCtestCommandCatalogueDestroy(catalogue);
    UmiInputFileFree(registry);
    UmiInputFileFree(json);
    return status;
}
