# Decide which registered tests a release needs

A test list tells you what CTest can discover. It does not tell you which tests
your product promises to pass. Test policy adds that engineering decision to
each exact test name, with an owner and a reason.

The policy service belongs to `Umicom::distribution`. It does not execute tests,
authenticate reviewers or approve a release. Its assessment answers a smaller
question: does this captured configuration have a complete, consistent policy
and usable registration for every required test?

## 1. Capture the configuration and registration

Follow [the inventory lesson](inspect-build-registration.md) to configure, build
and capture the CTest inventory. Work in the Applications directory for these
examples, using the Windows UCRT64 Debug preset. Keep the same build directory
and configuration throughout.

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$InventoryBuild = (Resolve-Path ".\build\windows-ucrt64-all-debug").Path
$InventoryTool = Join-Path $InventoryBuild "bin\umicom-release-inventory.exe"
$ConfiguredInventory = Join-Path $InventoryBuild "release-inventory\inventory-Debug.tsv"
$CTestInventory = Join-Path $InventoryBuild "release-inventory\ctest-Debug.tsv"
```

The input files describe one configuration generation. Reconfiguring produces
a new generation, so recapture CTest and review a new policy draft afterwards.
The tool refuses a policy from another generation, source directory, build
directory or configuration. Matching identifiers are consistency checks; they
do not authenticate the files or freeze the source tree.

## 2. Create an unassigned draft

Choose a new output filename. The command refuses to overwrite an existing file.

```powershell
$PolicyDraft = Join-Path $InventoryBuild "release-inventory\policy-draft-01.tsv"
& $InventoryTool draft-policy $ConfiguredInventory $PolicyDraft
if ($LASTEXITCODE -ne 0) { throw "Policy draft could not be created." }

& $InventoryTool review-policy $PolicyDraft
if ($LASTEXITCODE -ne 0) { throw "Policy could not be read." }

& $InventoryTool assess-policy $ConfiguredInventory $CTestInventory $PolicyDraft
if ($LASTEXITCODE -ne 1) { throw "A new unassigned draft must remain blocked." }
```

Every configured test starts as `unassigned`. A draft is not an approval and
does not silently classify all tests as optional. An empty configured test
population is an error.

## 3. Record one reviewed decision

Choose an exact name from `review-policy`. Here is a decision about the policy
service's own round-trip regression. Replace the example owner and reason with
the actual engineering decision for your release.

```powershell
$PolicyRevision = Join-Path $InventoryBuild "release-inventory\policy-review-01.tsv"
& $InventoryTool set-policy $PolicyDraft `
    "framework.release_policy.roundtrip" required `
    "Framework release maintainer" `
    "Policy edits must preserve earlier decisions and round-trip without losing text." `
    $PolicyRevision
if ($LASTEXITCODE -ne 0) { throw "Decision could not be recorded." }

& $InventoryTool review-policy $PolicyRevision
if ($LASTEXITCODE -ne 0) { throw "Updated policy could not be read." }
```

The original draft remains intact. Each edit emits the complete new document,
retaining every other decision and the configuration identity. Use the new
revision as the input for your next decision and another new output filename.
Names are literal: `framework.*` does not select a group. An unknown name is
refused and creates no output.

| Decision | What you must supply | Effect on assessment |
| --- | --- | --- |
| `unassigned` | A known test name | Blocks readiness until reviewed. |
| `required` | A nonblank owner and reason | A disabled test or missing executable command blocks readiness. |
| `optional` | A nonblank owner and reason | A disabled test or missing command remains visible but does not alone block readiness. |

Optional means the product decision permits that registered test to be
unavailable in this configuration. It does not permit the test name to disappear
from the population. Added or missing registrations always require review,
including a missing optional test. At least one test must remain required;
marking everything optional cannot produce readiness.

Do not assign optional status to work around an unexplained failure. Keep its
failure visible and decide whether the product's documented scope supports an
exception. This service records the decision; it does not make that decision
for you. It has no automatic connection to the separate Core approval ledger.

## 4. Assess the completed policy, then execute tests

```powershell
& $InventoryTool assess-policy $ConfiguredInventory $CTestInventory $PolicyRevision
$PolicyExit = $LASTEXITCODE
if ($PolicyExit -eq 1) { throw "Policy remains incomplete or registration needs review." }
if ($PolicyExit -ne 0) { throw "Policy input or configuration context is invalid." }

& "C:\msys64\ucrt64\bin\ctest.exe" `
    --preset windows-ucrt64-all-debug --parallel 2 --no-tests=error --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Tests failed." }
```

The single decision in step 3 will normally leave many unassigned tests, so
assessment should remain blocked until their owners complete the policy. An
exit code of 0 means registration is ready for execution. It is not a pass
receipt. Inspect the actual run, including skips and failures, and apply the
product's qualification requirements separately. This version does not import
CTest result logs or decide whether a skipped execution is acceptable.

## Use the C API

Include `umicom/distribution/runtime/test_policy.h` and link
`Umicom::distribution`. `UmiReleaseTestPolicyDraft` returns a complete draft as
owned text. Parse it with `UmiReleaseTestPolicyParse`, edit one rule with
`UmiReleaseTestPolicyEdit`, and inspect the immutable rules with
`UmiReleaseTestPolicyAt`. Assess against both inventories with
`UmiReleaseTestPolicyAssess`.

Free returned text with `UmiReleaseTestPolicyTextDestroy` and parsed policies
with `UmiReleaseTestPolicyDestroy`. Rule strings are borrowed from their policy.
Caller-provided strings must remain valid and unchanged during each call. On
failure, output pointers, lengths and assessments are left unchanged. Finding
callbacks are invoked only after context validation; they must not destroy the
objects being inspected. No C API function opens files or starts processes.

The separate `policy-consumer` example in `examples/release_inventory_consumer`
uses the focused installed package to draft, edit and assess a synthetic lesson.
It retains the earlier inventory consumer. Follow the installed-consumer steps
in the inventory lesson to build and run both checks.

## Format and troubleshooting

Version 1 starts with `UMICOM-RELEASE-TEST-POLICY<TAB>1`. Its context row contains
`context`, `policy`, a 32-digit generation identifier, and hex-encoded source
root, build root and configuration. Each row then contains `test`, hex name,
hex owner, plain decision and hex reason. The tool handles encoding so a beginner
does not have to edit hexadecimal text. Fields may contain tabs, Unicode bytes
and newlines without changing the row boundaries. Console controls are escaped
when reviewing a policy.

The bounds match the inventory: 64 MiB input, 200,000 rules and 256 KiB per
decoded field. Duplicate names, malformed hex, embedded NUL, unknown decisions
and missing required/optional explanations are refused. An empty policy can be
parsed but cannot make a nonempty test population ready.

For a stale context, recapture and make a fresh reviewed draft; do not paste the
new generation identifier over an old policy to hide changes. For a missing
rule, assign that exact test. For an orphan rule, inspect why its test left the
configuration. For an existing-output error, use a new filename. A filesystem
failure may leave a partial newly created output, which must not be treated as
a valid decision. Existing output files are never truncated by these commands.
Keep policies and captured inventories with local qualification evidence.
