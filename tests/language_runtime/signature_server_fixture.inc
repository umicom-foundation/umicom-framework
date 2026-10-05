/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/signature_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native parameter-help requests and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
static int SignatureFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192];
    int stage = 0, result = 0;
    for (;;)
    {
        size_t length = 0U;
        int have_length = 0;
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        for (;;)
        {
            if (strncmp(line, "Content-Length: ", 16U) == 0)
            {
                char *end = NULL;
                unsigned long value = strtoul(line + 16U, &end, 10);
                if (end == line + 16U || (*end != '\r' && *end != '\n') || value >= 65536UL)
                {
                    result = 4;
                    goto done;
                }
                length = (size_t)value;
                have_length = 1;
            }
            if (strcmp(line, "\r\n") == 0)
                break;
            if (fgets(line, sizeof(line), stdin) == NULL)
            {
                result = 4;
                goto done;
            }
        }
        if (!have_length || fread(body, 1U, length, stdin) != length)
        {
            result = 4;
            break;
        }
        body[length] = '\0';
        const char *id_text = strstr(body, "\"id\":");
        unsigned long long id = id_text == NULL ? 0ULL : strtoull(id_text + 5U, NULL, 10);
        int written = 0;
        if (strstr(body, "\"method\":\"initialize\"") != NULL)
        {
            if (stage != 0 || strstr(body, "\"labelOffsetSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "signature-encoding") == 0
                                   ? "\"signatureHelpProvider\":{},\"textDocumentSync\":2,"
                                     "\"positionEncoding\":\"utf-8\""
                                   : "\"signatureHelpProvider\":{},\"textDocumentSync\":{"
                                     "\"openClose\":true,\"change\":2}";
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{%s}}}", id, caps);
        }
        else if (strstr(body, "\"method\":\"initialized\"") != NULL)
        {
            if (stage != 1)
            {
                result = 5;
                break;
            }
            stage = 2;
        }
        else if (strstr(body, "\"method\":\"textDocument/didOpen\"") != NULL)
        {
            if (stage != 2 || strstr(body, "\"text\":") == NULL || strstr(body, "\"version\":1") == NULL)
            {
                result = 5;
                break;
            }
            if (strcmp(mode, "signature-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
            {
                result = 6;
                break;
            }
            stage = 3;
        }
        else if (strstr(body, "\"method\":\"textDocument/signatureHelp\"") != NULL)
        {
            if (stage != 3)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "signature-timeout") == 0)
                continue;
            if (strcmp(mode, "signature-disconnect") == 0)
                break;
            /* Explicit parameter help sends a caret but no unadvertised context. */
            if (strstr(body, "\"position\":") == NULL || strstr(body, "\"options\":") != NULL ||
                strstr(body, "\"context\":") != NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "signature-start") == 0 && strstr(body, "\"character\":0") == NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "signature-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else if (strcmp(mode, "signature-invalid") == 0)
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"isIncomplete\":false}}", id);
            else if (strcmp(mode, "signature-empty") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else
            {
                const char *payload =
                    "{\"signatures\":[{\"label\":\"sum(left, right)\",\"documentation\":\"Add two "
                    "values.\",\"parameters\":[{\"label\":\"left\",\"documentation\":\"First "
                    "value.\"},{\"label\":[10,15],\"documentation\":\"Second "
                    "value.\"}]}],\"activeParameter\":1}";
                if (strcmp(mode, "signature-overloads") == 0)
                    payload = "{\"signatures\":[{\"label\":\"f()\"},{\"label\":\"f(value)\",\"parameters\":[{"
                              "\"label\":\"value\"}]}],\"activeSignature\":1}";
                if (strcmp(mode, "signature-markup") == 0)
                    payload =
                        "{\"signatures\":[{\"label\":\"f(value)\",\"documentation\":{\"kind\":\"markdown\","
                        "\"value\":\"<script>literal</script>\"},\"parameters\":[{\"label\":\"value\"}]}]}";
                if (strcmp(mode, "signature-surrogate") == 0)
                    payload = "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":["
                              "2,3]}]}]}";
                if (strcmp(mode, "signature-unicode") == 0 && strstr(body, "\"character\":2") == NULL)
                {
                    result = 6;
                    break;
                }
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%s}", id, payload);
            }
        }

        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if (stage != 4)
            {
                result = 5;
                break;
            }
            stage = 5;
        }
        else if (strstr(body, "\"method\":\"shutdown\"") != NULL)
        {
            if (stage != 5)
            {
                result = 5;
                break;
            }
            stage = 6;
            if (strcmp(mode, "signature-shutdown") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":"
                                   "\"shutdown failed\"}}",
                                   id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
        }
        else if (strstr(body, "\"method\":\"exit\"") != NULL)
        {
            if (stage != 6)
                result = 5;
            break;
        }
        else
        {
            result = 6;
            break;
        }
        if (written > 0 &&
            ((size_t)written >= sizeof(response) || printf("Content-Length: %d\r\n\r\n", written) < 0 ||
             fwrite(response, 1U, (size_t)written, stdout) != (size_t)written || fflush(stdout) != 0))
        {
            result = 7;
            break;
        }
    }
done:
    free(body);
    return result;
}
