# Store semantic theme tokens safely

![Umicom](../../resources/brand/umicom-logo.svg)

A semantic token names a presentation role such as `text.code.font`.
The role is stable even when a GTK, Qt or native Windows adapter changes.
Umicom Framework owns the token catalogue; an application supplies values and
asks an adapter to interpret them. Applications should reuse the approved
Umicom assets and typography rather than implement separate branding rules.

This lesson covers the toolkit-neutral `UmiUiThemeModel`. It does not
apply a theme, load a font, download an asset or create a window.

## 1. Separate storage from presentation

Each `UmiUiThemeSnapshot` contains three inline character arrays:

| Field | Storage rule | Meaning |
| --- | --- | --- |
| `token_id` | Nonempty, terminated within `UMI_UI_ID_CAPACITY`, shared UI identifier grammar | Stable lookup key |
| `value` | Terminated within its declared array; empty is permitted | Adapter-specific semantic value |
| `description` | Terminated within its declared array; empty is permitted | Explanation for an inspector or editor |

Termination means a zero byte occurs before the end of the array. Filling every
byte with visible characters leaves no room for that byte. A later unbounded
string operation could then read unrelated storage. Framework now checks all
three fields before it changes the catalogue.

The store intentionally does not parse colours, fonts, CSS, UTF-8 or file paths.
The [profile and editor validation guide](theme-field-validation.md) describes
the additional rules used by those typed owners. A stored font list does not
prove that the fonts are installed. Use the existing Segoe UI Variable Text /
Segoe UI and Cascadia Code / Consolas roles with the platform fallback policy.
Do not download or redistribute font files as part of token registration.

## 2. Create a complete value and copy it into Framework

This complete console example uses only the public Framework API:

~~~c
#include <umicom/ui/theme.h>
#include <stdio.h>

int main(void)
{
    UmiUiThemeModel *model = NULL;
    UmiUiThemeSnapshot token = {
        .token_id = "text.code.font",
        .value = "Cascadia Code, Consolas",
        .description = "Code text uses installed fonts and adapter fallbacks."
    };
    UmiUiThemeSnapshot found = {0};
    if (umi_ui_theme_model_create(&model) != UMI_STATUS_OK) return 1;

    UmiStatus status = umi_ui_theme_model_upsert(model, &token);
    if (status == UMI_STATUS_OK)
        status = umi_ui_theme_model_find(model, token.token_id, &found);
    if (status == UMI_STATUS_OK)
        (void)printf("%s = %s\n", found.token_id, found.value);

    umi_ui_theme_model_destroy(model);
    return status == UMI_STATUS_OK ? 0 : 1;
}
~~~

Save it as `theme_tokens.c` and use a matching installed Framework SDK:

~~~cmake
cmake_minimum_required(VERSION 3.25)
project(UmicomThemeTokens LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ui)
add_executable(theme_tokens theme_tokens.c)
set_target_properties(theme_tokens PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(theme_tokens PRIVATE Umicom::ui)
if(WIN32)
    enable_language(RC)
endif()
umicom_apply_application_branding(TARGET theme_tokens
    PRODUCT_NAME "Umicom Theme Tokens" INTERNAL_NAME "umicom-theme-tokens")
~~~

The exported branding helper supplies the approved Windows ICO resource and
stages the shared SVG branding assets for the example. Runtime deployment follows
the selected SDK helper contract; it still needs platform qualification.

The model copies the snapshot. Changing the caller's value afterwards does not
change the stored token. Call `upsert` again to replace it. This API does
not expose pointers to the private item array. Input snapshots and output
snapshots must be separate from the opaque model allocation.

The expected successful output is:

~~~text
text.code.font = Cascadia Code, Consolas
~~~

This output reports storage and lookup only. No font selection occurred.

## 3. Handle refusal without changing the owner

An invalid inline field returns `UMI_STATUS_INVALID_ARGUMENT`. A valid
new ID in a full catalogue returns `UMI_STATUS_CAPACITY_EXCEEDED`.
Both leave the item count, records and revision unchanged. Replacing an existing
ID still works when all `UMI_UI_THEME_MAX` slots are occupied.

Lookup and removal validate their query through `umi_ui_id_is_valid`.
The caller must provide readable storage through its zero byte or the
`UMI_UI_ID_CAPACITY` bound. Empty, invalid or unterminated queries return
`INVALID_ARGUMENT`. A valid key that is absent returns `NOT_FOUND`.
Lookup and indexed access leave the output snapshot unchanged on refusal.

It is safe to use `output.token_id` as the query when `output` is a
separate writable snapshot: comparison finishes before the result is copied.
It is never valid to cast the model allocation into an output snapshot.

Removal compacts the remaining records. An array index can therefore identify a
different token after removal; retain token IDs across edits. The revision is
the existing change marker and wraps from its maximum value to one. It is not
a permanent object identity or a globally ordered timestamp.

The model mutex serializes access to its own state. Callers still synchronize
their input/output storage and the lifetime of the model. Do not destroy a model
while another thread is using it.

## 4. Qualify the regression and example

Run each command separately and stop on an error. Adapt the SDK installation
prefix to the installed Framework being evaluated.

~~~powershell
Set-Location C:\umicom\Umicom-Applications\framework
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ui-theme-token-storage-test
ctest --preset windows-ucrt64-debug --output-on-failure --no-tests=error -R "^framework[.]ui[.]tokens[.]storage$"
~~~

The regression covers all three unterminated fields, longest valid values,
empty optional fields, bounded queries, output alias refusal, unchanged state,
full-capacity replacement, removal ordering and re-addition. A successful run
reports one selected test and zero failures. Source review alone cannot establish
that result.

For the saved example and CMakeLists.txt:

~~~powershell
Set-Location C:\umicom\projects\theme-tokens
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/umicom/installed/framework"
cmake --build build --target theme_tokens
& ".\build\theme_tokens.exe"
~~~

If CMake cannot find Umicom, check the installed SDK path and its configuration
package. If the test is missing, reconfigure from the source containing its
registration and build the named target. A failure requires inspection of the
first assertion and the exact source/SDK versions; do not clear the model to
hide an admission failure.

The full regression source is
[tests/ui_components/test_theme_token_storage.c](../../tests/ui_components/test_theme_token_storage.c).
Native rendering, installed font fallback, SVG/ICO appearance and adapter
concurrency require separate qualification.
