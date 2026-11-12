/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_binding_admission.c
 *
 * PURPOSE:
 *   Verify bounded binding descriptors, transform output admission and target preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/binding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)

typedef struct TransformState { const char *mode; unsigned calls; } TransformState;
static UmiStatus Transform(const UmiUiValue *source, UmiUiValue *target, void *context)
{
    TransformState *state = context;
    ++state->calls;
    if (source->kind != UMI_UI_VALUE_STRING || strcmp(source->string_value, "source") != 0)
        return UMI_STATUS_INTERNAL_ERROR;
    if (strcmp(state->mode, "no-write") == 0) return UMI_STATUS_OK;
    if (strcmp(state->mode, "error") == 0) return UMI_STATUS_CANCELLED;
    if (strcmp(state->mode, "error-after-write") == 0) {
        (void)umi_ui_value_set_string(target, "must not publish");
        return UMI_STATUS_IO_ERROR;
    }
    if (strcmp(state->mode, "kind") == 0) { target->kind = (UmiUiValueKind)99; return UMI_STATUS_OK; }
    if (strcmp(state->mode, "unterminated") == 0) {
        target->kind = UMI_UI_VALUE_STRING;
        memset(target->string_value, 'x', sizeof(target->string_value));
        return UMI_STATUS_OK;
    }
    if (strcmp(state->mode, "none") == 0) { umi_ui_value_clear(target); return UMI_STATUS_OK; }
    return umi_ui_value_set_string(target, "converted");
}
static int MakeViews(UmiUiViewModel **source, UmiUiViewModel **target)
{
    UmiUiValue value;
    CHECK(umi_ui_view_model_create("source", "label", UMI_UI_ROLE_GENERIC, source) == UMI_STATUS_OK);
    CHECK(umi_ui_view_model_create("target", "label", UMI_UI_ROLE_GENERIC, target) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&value, "source") == UMI_STATUS_OK);
    CHECK(umi_ui_view_model_set_property(*source, "text", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&value, "retained") == UMI_STATUS_OK);
    CHECK(umi_ui_view_model_set_property(*target, "text", &value) == UMI_STATUS_OK);
    return 0;
}
static void MakeDescriptor(UmiUiBindingDescriptor *descriptor, UmiUiViewModel *source, UmiUiViewModel *target)
{
    memset(descriptor, 0, sizeof(*descriptor));
    strcpy(descriptor->binding_id, "binding");
    strcpy(descriptor->source_property, "text");
    strcpy(descriptor->target_property, "text");
    descriptor->source_view = source; descriptor->target_view = target; descriptor->enabled = 1;
}
static int CheckIdentifier(const char *name)
{
    char bytes[UMI_UI_ID_CAPACITY + 1U];
    const char *input = bytes;
    int expected = 0;
    memset(bytes, 'a', sizeof(bytes));
    if (strcmp(name, "valid") == 0) { input = "label.value-1/path_id"; expected = 1; }
    else if (strcmp(name, "limit") == 0) { bytes[UMI_UI_ID_CAPACITY - 1U] = '\0'; expected = 1; }
    else if (strcmp(name, "over") == 0) bytes[UMI_UI_ID_CAPACITY] = '\0';
    else if (strcmp(name, "unterminated") == 0) { /* complete bounded fixture */ }
    else if (strcmp(name, "null") == 0) input = NULL;
    else if (strcmp(name, "empty") == 0) input = "";
    else if (strcmp(name, "space") == 0) input = "two words";
    else if (strcmp(name, "colon") == 0) input = "name:value";
    else return 1;
    CHECK(umi_ui_id_is_valid(input) == expected);
    return 0;
}
static int CheckRegistration(const char *name)
{
    UmiUiViewModel *source = NULL, *target = NULL;
    UmiUiBindingRegistry *registry = NULL;
    UmiUiBindingDescriptor descriptor, retained;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(MakeViews(&source, &target) == 0);
    CHECK(umi_ui_binding_registry_create(&registry) == UMI_STATUS_OK);
    MakeDescriptor(&descriptor, source, target);
    UmiUiBindingRegistry *registry_argument = registry;
    const UmiUiBindingDescriptor *descriptor_argument = &descriptor;
    size_t before = 0U;
    if (strcmp(name, "valid") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(name, "id") == 0) memset(descriptor.binding_id, 'x', sizeof(descriptor.binding_id));
    else if (strcmp(name, "source-key") == 0) memset(descriptor.source_property, 'x', sizeof(descriptor.source_property));
    else if (strcmp(name, "target-key") == 0) memset(descriptor.target_property, 'x', sizeof(descriptor.target_property));
    else if (strcmp(name, "empty-source") == 0) descriptor.source_property[0] = '\0';
    else if (strcmp(name, "empty-target") == 0) descriptor.target_property[0] = '\0';
    else if (strcmp(name, "source-view") == 0) descriptor.source_view = NULL;
    else if (strcmp(name, "target-view") == 0) descriptor.target_view = NULL;
    else if (strcmp(name, "null-descriptor") == 0) descriptor_argument = NULL;
    else if (strcmp(name, "null-registry") == 0) registry_argument = NULL;
    else if (strcmp(name, "limit") == 0) {
        memset(descriptor.binding_id, 'a', sizeof(descriptor.binding_id) - 1U);
        memset(descriptor.source_property, 's', sizeof(descriptor.source_property) - 1U);
        memset(descriptor.target_property, 't', sizeof(descriptor.target_property) - 1U);
        expected = UMI_STATUS_OK;
    } else if (strcmp(name, "duplicate") == 0) {
        CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
        before = 1U; expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "full") == 0) {
        for (size_t i = 0U; i < UMI_UI_BINDING_MAX; ++i) {
            (void)snprintf(descriptor.binding_id, sizeof(descriptor.binding_id), "binding.%zu", i);
            CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
        }
        strcpy(descriptor.binding_id, "one-more");
        before = UMI_UI_BINDING_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else return 1;
    memcpy(&retained, &descriptor, sizeof(descriptor));
    CHECK(umi_ui_binding_register(registry_argument, descriptor_argument) == expected);
    CHECK(umi_ui_binding_count(registry) == before + (expected == UMI_STATUS_OK ? 1U : 0U));
    CHECK(memcmp(&retained, &descriptor, sizeof(descriptor)) == 0);
    umi_ui_binding_registry_destroy(registry);
    umi_ui_view_model_destroy(target); umi_ui_view_model_destroy(source);
    return 0;
}
static int CheckPropagation(const char *method, const char *mode)
{
    UmiUiViewModel *source = NULL, *target = NULL;
    UmiUiBindingRegistry *registry = NULL;
    UmiUiBindingDescriptor descriptor;
    UmiUiViewSnapshot before, after;
    UmiUiValue observed;
    TransformState state = {mode, 0U};
    UmiStatus expected = UMI_STATUS_OK;
    int changes = 1;
    unsigned calls = 1U;
    CHECK(MakeViews(&source, &target) == 0);
    CHECK(umi_ui_binding_registry_create(&registry) == UMI_STATUS_OK);
    MakeDescriptor(&descriptor, source, target);
    descriptor.transform = Transform; descriptor.user_data = &state;
    if (strcmp(mode, "identity") == 0) { descriptor.transform = NULL; calls = 0U; }
    else if (strcmp(mode, "disabled") == 0) { descriptor.enabled = 0; changes = 0; calls = 0U; }
    else if (strcmp(mode, "missing") == 0) {
        strcpy(descriptor.source_property, "missing"); expected = UMI_STATUS_NOT_FOUND; changes = 0; calls = 0U;
    } else if (strcmp(mode, "error") == 0) { expected = UMI_STATUS_CANCELLED; changes = 0; }
    else if (strcmp(mode, "error-after-write") == 0) { expected = UMI_STATUS_IO_ERROR; changes = 0; }
    else if (strcmp(mode, "no-write") == 0 || strcmp(mode, "kind") == 0 || strcmp(mode, "unterminated") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT; changes = 0;
    } else if (strcmp(mode, "valid") != 0 && strcmp(mode, "none") != 0) return 1;
    CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
    CHECK(umi_ui_view_model_snapshot(target, &before) == UMI_STATUS_OK);
    const uint64_t property_revision = umi_ui_property_bag_revision(umi_ui_view_model_properties(target));
    UmiStatus actual;
    if (strcmp(method, "all") == 0) {
        size_t count = 99U;
        actual = umi_ui_binding_propagate_all(registry, &count);
        CHECK(count == (expected == UMI_STATUS_OK ? 1U : 0U));
    } else actual = umi_ui_binding_propagate(registry, "binding");
    CHECK(actual == expected && state.calls == calls);
    CHECK(umi_ui_view_model_snapshot(target, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision + (changes ? 1U : 0U));
    CHECK(umi_ui_property_bag_revision(umi_ui_view_model_properties(target)) == property_revision + (changes ? 1U : 0U));
    CHECK(umi_ui_view_model_get_property(target, "text", &observed) == UMI_STATUS_OK);
    if (strcmp(mode, "none") == 0) CHECK(observed.kind == UMI_UI_VALUE_NONE);
    else {
        CHECK(observed.kind == UMI_UI_VALUE_STRING);
        CHECK(strcmp(observed.string_value, !changes ? "retained" : strcmp(mode, "identity") == 0 ? "source" : "converted") == 0);
    }
    CHECK(umi_ui_view_model_get_property(source, "text", &observed) == UMI_STATUS_OK);
    CHECK(strcmp(observed.string_value, "source") == 0);
    umi_ui_binding_registry_destroy(registry);
    umi_ui_view_model_destroy(target); umi_ui_view_model_destroy(source);
    return 0;
}
static int CheckLookup(const char *method)
{
    UmiUiViewModel *source = NULL, *target = NULL;
    UmiUiBindingRegistry *registry = NULL;
    UmiUiBindingDescriptor descriptor;
    char invalid[UMI_UI_ID_CAPACITY];
    CHECK(MakeViews(&source, &target) == 0);
    CHECK(umi_ui_binding_registry_create(&registry) == UMI_STATUS_OK);
    MakeDescriptor(&descriptor, source, target);
    CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
    memset(invalid, 'x', sizeof(invalid));
    UmiStatus status = strcmp(method, "unregister") == 0 ?
        umi_ui_binding_unregister(registry, invalid) : umi_ui_binding_propagate(registry, invalid);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && umi_ui_binding_count(registry) == 1U);
    CHECK(umi_ui_binding_propagate(registry, "binding") == UMI_STATUS_OK);
    umi_ui_binding_registry_destroy(registry);
    umi_ui_view_model_destroy(target); umi_ui_view_model_destroy(source);
    return 0;
}
static int CheckMixed(void)
{
    UmiUiViewModel *source = NULL, *target = NULL;
    UmiUiBindingRegistry *registry = NULL;
    UmiUiBindingDescriptor descriptor;
    UmiUiValue observed;
    TransformState bad = {"no-write", 0U}, good = {"valid", 0U}, disabled = {"valid", 0U};
    CHECK(MakeViews(&source, &target) == 0);
    CHECK(umi_ui_binding_registry_create(&registry) == UMI_STATUS_OK);
    MakeDescriptor(&descriptor, source, target);
    descriptor.transform = Transform; descriptor.user_data = &bad;
    CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
    strcpy(descriptor.binding_id, "good"); strcpy(descriptor.target_property, "second"); descriptor.user_data = &good;
    CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
    strcpy(descriptor.binding_id, "disabled"); strcpy(descriptor.target_property, "third");
    descriptor.user_data = &disabled; descriptor.enabled = 0;
    CHECK(umi_ui_binding_register(registry, &descriptor) == UMI_STATUS_OK);
    size_t count = 99U;
    CHECK(umi_ui_binding_propagate_all(registry, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(count == 2U && bad.calls == 1U && good.calls == 1U && disabled.calls == 0U);
    CHECK(umi_ui_view_model_get_property(target, "text", &observed) == UMI_STATUS_OK && strcmp(observed.string_value, "retained") == 0);
    CHECK(umi_ui_view_model_get_property(target, "second", &observed) == UMI_STATUS_OK && strcmp(observed.string_value, "converted") == 0);
    CHECK(umi_ui_view_model_get_property(target, "third", &observed) == UMI_STATUS_NOT_FOUND);
    umi_ui_binding_registry_destroy(registry);
    umi_ui_view_model_destroy(target); umi_ui_view_model_destroy(source);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "mixed") == 0) return CheckMixed();
    if (argc != 3) return 2;
    if (strcmp(argv[1], "identifier") == 0) return CheckIdentifier(argv[2]);
    if (strcmp(argv[1], "register") == 0) return CheckRegistration(argv[2]);
    if (strcmp(argv[1], "lookup") == 0) return CheckLookup(argv[2]);
    if (strcmp(argv[1], "single") == 0 || strcmp(argv[1], "all") == 0)
        return CheckPropagation(argv[1], argv[2]);
    return 2;
}
