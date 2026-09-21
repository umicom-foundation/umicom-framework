/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest_json_capture.inc
 * PURPOSE:
 *   Capture every CTest JSON byte through the existing native runner while
 *   keeping memory, process duration and publication explicitly bounded.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct UmiCtestCapture {
    char *data;
    size_t used;
    size_t capacity;
    size_t maximum;
    UmiStatus status;
} UmiCtestCapture;

static void UmiCtestCaptureChunk(const char *bytes, size_t length, void *context)
{
    UmiCtestCapture *capture = (UmiCtestCapture *)context;
    size_t required, capacity;
    char *data;
    if (capture->status != UMI_STATUS_OK || length == 0U) return;
    if (length > capture->maximum - capture->used) {
        capture->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    required = capture->used + length + 1U;
    if (required > capture->capacity) {
        capacity = capture->capacity != 0U ? capture->capacity : 4096U;
        while (capacity < required) {
            if (capacity > (capture->maximum + 1U) / 2U) { capacity = capture->maximum + 1U; break; }
            capacity *= 2U;
        }
        if (capacity > capture->maximum + 1U) capacity = capture->maximum + 1U;
        data = (char *)realloc(capture->data, capacity);
        if (data == NULL) { capture->status = UMI_STATUS_OUT_OF_MEMORY; return; }
        capture->data = data; capture->capacity = capacity;
    }
    (void)memcpy(capture->data + capture->used, bytes, length);
    capture->used += length; capture->data[capture->used] = '\0';
}

static void UmiCtestDiagnostic(char *destination, size_t capacity, const char *message,
    const char *tail)
{
    size_t used, length;
    if (destination == NULL || capacity == 0U) return;
    used = strlen(message); if (used >= capacity) used = capacity - 1U;
    (void)memcpy(destination, message, used); destination[used] = '\0';
    if (tail != NULL && tail[0] != '\0' && capacity - used > 2U) {
        destination[used++] = '\n'; length = strlen(tail);
        if (length >= capacity - used) tail += length - (capacity - used - 1U);
        length = strlen(tail); (void)memcpy(destination + used, tail, length + 1U);
    }
}

UmiStatus UmiTestPlatformCtestDiscoverConfigured(
    const UmiTestPlatformCtestImportOptions *options,
    const UmiTestPlatformCtestDiscoveryOptions *execution,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries, UmiTestPlatformCtestImportSummary *outSummary,
    char *outDiagnostics, size_t diagnosticsCapacity)
{
    UmiTestPlatformCtestImportOptions selected;
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiCtestCapture capture = {NULL, 0U, 0U, UMI_TEST_PLATFORM_CTEST_DEFAULT_BYTES, UMI_STATUS_OK};
    const char *arguments[5];
    size_t maximumTests = UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY - 1U;
    UmiStatus status;
    char marker[1060]; FILE *file;
    if (outSummary != NULL) (void)memset(outSummary, 0, sizeof(*outSummary));
    UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, "", NULL);
    status = UmiCtestOptionsValidate(options);
    if (status != UMI_STATUS_OK || items == NULL || suites == NULL || discoveries == NULL) {
        UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, "Invalid CTest discovery options; catalogue unchanged.", NULL);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&request, 0, sizeof(request));
    (void)memset(&result, 0, sizeof(result));
    result.exit_code = -1;
    request.timeout_ms = UMI_TEST_PLATFORM_CTEST_DEFAULT_TIMEOUT_MS;
    if (execution != NULL) {
        if (execution->maximum_output_bytes != 0U) capture.maximum = execution->maximum_output_bytes;
        if (execution->maximum_tests != 0U) maximumTests = execution->maximum_tests;
        if (execution->timeout_ms != 0U) request.timeout_ms = execution->timeout_ms;
        request.cancellation = execution->cancellation;
    }
    if (capture.maximum > UMI_TEST_PLATFORM_CTEST_MAX_BYTES ||
        maximumTests >= UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY) {
        UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, "CTest discovery limit exceeds the supported bound; catalogue unchanged.", NULL);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_cancellation_token_is_requested(request.cancellation)) {
        UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, "CTest discovery cancelled before launch; catalogue unchanged.", NULL);
        return UMI_STATUS_CANCELLED;
    }
    selected = *options;
    if (selected.configuration[0] == '\0') (void)memcpy(selected.configuration, "Debug", sizeof("Debug"));
    /* Empty valid catalogues are meaningful replacements. Reject an unconfigured
     * directory separately instead of making --no-tests=error block deletion. */
    (void)snprintf(marker, sizeof(marker), "%s/CTestTestfile.cmake", selected.build_directory);
    file = fopen(marker, "rb");
    if (file == NULL) {
        UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, "The build root has no readable CTestTestfile.cmake; configure it before discovery. Catalogue unchanged.", NULL);
        return UMI_STATUS_NOT_FOUND;
    }
    if (fclose(file) != 0) return UMI_STATUS_IO_ERROR;
    arguments[0] = "--test-dir"; arguments[1] = selected.build_directory;
    arguments[2] = "--show-only=json-v1"; arguments[3] = "-C"; arguments[4] = selected.configuration;
    request.program = "ctest"; request.arguments = arguments; request.argument_count = 5U;
    request.capture_stdout = 1; request.capture_stderr = 1; request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = UmiProcessExecuteStreamed(&request, UmiCtestCaptureChunk, &capture, &result);
    if (result.cancelled || umi_cancellation_token_is_requested(request.cancellation)) status = UMI_STATUS_CANCELLED;
    else if (result.timed_out) status = UMI_STATUS_TIMEOUT;
    else if (status == UMI_STATUS_OK && !result.launched) status = UMI_STATUS_UNAVAILABLE;
    else if (status == UMI_STATUS_OK && result.exit_code != 0) status = UMI_STATUS_INTERNAL_ERROR;
    else if (status == UMI_STATUS_OK && capture.status != UMI_STATUS_OK) status = capture.status;
    if (status == UMI_STATUS_OK) {
        if (capture.data == NULL) status = UMI_STATUS_PARSE_ERROR;
        else status = UmiCtestImportJson(capture.data, capture.used, &selected, maximumTests,
            1, items, suites, discoveries, outSummary);
    }
    if (status != UMI_STATUS_OK) {
        char message[160];
        (void)snprintf(message, sizeof(message), "CTest JSON discovery failed (status %d); catalogue unchanged. Raw bytes retained: %zu.", (int)status, capture.used);
        UmiCtestDiagnostic(outDiagnostics, diagnosticsCapacity, message, result.output);
    }
    /* output_truncated is intentionally NOT an error: it describes only the
     * retained diagnostic tail. capture.status/used describe the complete stream. */
    free(capture.data);
    return status;
}

/* Invoke ctest with argument arrays, capture diagnostics, then use the same
 * parser exercised by deterministic unit tests. */
UmiStatus umi_test_platform_ctest_discover(
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries, UmiTestPlatformCtestImportSummary *outSummary,
    char *outDiagnostics, size_t diagnosticsCapacity)
{
    return UmiTestPlatformCtestDiscoverConfigured(options, NULL, items, suites,
        discoveries, outSummary, outDiagnostics, diagnosticsCapacity);
}
