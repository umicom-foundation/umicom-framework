/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_protocol_json.c
 *
 * PURPOSE:
 *   Verify bounded JSON escaping and primitive field extraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/protocol/json.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char escaped[128];
    char name[64];
    int64_t number = 0;
    int boolean = 0;
    const char *json = "{\"name\":\"Umicom\",\"count\":7,\"ready\":true}";
    assert(umi_json_escape("a\nb\"c", escaped, sizeof(escaped)) == UMI_STATUS_OK);
    assert(strstr(escaped, "\\n") != NULL);
    assert(umi_json_get_string(json, "name", name, sizeof(name)) == UMI_STATUS_OK);
    assert(strcmp(name, "Umicom") == 0);
    assert(umi_json_get_integer(json, "count", &number) == UMI_STATUS_OK && number == 7);
    assert(umi_json_get_boolean(json, "ready", &boolean) == UMI_STATUS_OK && boolean);
    return 0;
}
