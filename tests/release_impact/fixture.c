/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/fixture.c
 * Purpose: Encode synthetic inventory fixtures using the public bounded format.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdlib.h>
#include <string.h>
static bool Append(Fixture *f, const char *text)
{
    size_t n = strlen(text);
    if (n > UMI_RELEASE_INVENTORY_TEXT_LIMIT - f->used) return false;
    if (f->used + n + 1U > f->capacity) {
        size_t cap = (f->used + n + 1U) * 2U;
        char *next = realloc(f->text, cap);
        if (next == NULL) return false;
        f->text = next; f->capacity = cap;
    }
    memcpy(f->text + f->used, text, n + 1U); f->used += n;
    return true;
}
static bool Hex(Fixture *f, const char *text)
{
    static const char digits[] = "0123456789abcdef";
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        char bytes[] = {digits[*p >> 4U], digits[*p & 15U], '\0'};
        if (!Append(f, bytes)) return false;
    }
    return true;
}
bool FixtureStart(Fixture *f)
{
    memset(f, 0, sizeof(*f));
    return Append(f, "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake\t0123456789abcdef0123456789abcdef\t2f7265706f\t2f6275696c64\t4465627567\n");
}
bool FixtureRow(Fixture *f, const char *kind, const char *name, const char *owner, const char *state, const char *detail)
{
    return Append(f, kind) && Append(f, "\t") && Hex(f, name) && Append(f, "\t") &&
        Hex(f, owner) && Append(f, "\t") && Append(f, state) && Append(f, "\t") && Hex(f, detail) && Append(f, "\n");
}
bool FixtureTarget(Fixture *f, const char *name, const char *dependency)
{
    Fixture detail = {0};
    bool ok = Append(&detail, "impact-version=1\ncomplete=1\n");
    if (ok && dependency != NULL) ok = Append(&detail, "dependency=") && Hex(&detail, dependency) && Append(&detail, "\n");
    if (ok) ok = Append(&detail, "impact-end\nTYPE=STATIC_LIBRARY\n") && FixtureRow(f, "target", name, "/repo", "configured", detail.text);
    FixtureDestroy(&detail);
    return ok;
}
bool FixtureTest(Fixture *f, const char *name, const char *target, const char *helper, bool always)
{
    Fixture detail = {0};
    bool ok = Append(&detail, "impact-version=1\ncomplete=1\n");
    if (ok && target != NULL) ok = Append(&detail, "target=") && Hex(&detail, target) && Append(&detail, "\n");
    if (ok && helper != NULL) ok = Append(&detail, "target=") && Hex(&detail, helper) && Append(&detail, "\n");
    if (ok && always) ok = Append(&detail, "always=1\n");
    if (ok) ok = Append(&detail, "impact-end\nregistration only") && FixtureRow(f, "test", name, "/repo", "registered", detail.text);
    FixtureDestroy(&detail);
    return ok;
}
bool FixtureSource(Fixture *f, const char *name, const char *target)
{
    return FixtureRow(f, "source", name, target, "present", "sha256=synthetic");
}
UmiStatus FixtureParse(Fixture *f, UmiReleaseInventory **inventory)
{
    return UmiReleaseInventoryParse(f->text, f->used, inventory);
}
void FixtureDestroy(Fixture *f)
{
    free(f->text); memset(f, 0, sizeof(*f));
}
bool PlanContains(const UmiReleaseImpactPlan *plan, const char *name, uint32_t reason)
{
    for (size_t i = 0U; i < UmiReleaseImpactCount(plan); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
        if (strcmp(item->name, name) == 0) return (item->reasons & reason) == reason;
    }
    return false;
}

/* The original version-one fixture remains available for compatibility cases. */
bool FixtureRelatedTest(Fixture *f, const char *name, const char *target,
    const char *const *related, size_t relatedCount, bool always)
{
    Fixture detail = {0};
    bool ok = Append(&detail, "impact-version=2\ncomplete=1\n");
    if (ok && target != NULL) ok = Append(&detail, "target=") && Hex(&detail, target) && Append(&detail, "\n");
    for (size_t i = 0U; ok && i < relatedCount; ++i)
        ok = Append(&detail, "test=") && Hex(&detail, related[i]) && Append(&detail, "\n");
    if (ok && always) ok = Append(&detail, "always=1\n");
    if (ok) ok = Append(&detail, "impact-end\nregistration only") &&
        FixtureRow(f, "test", name, "/repo", "registered", detail.text);
    FixtureDestroy(&detail);
    return ok;
}
