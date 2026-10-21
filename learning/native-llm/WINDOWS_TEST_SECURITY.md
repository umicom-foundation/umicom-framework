# Windows Security Blocks a Native CTest Executable — Investigation Guide

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT

## The incident

During the native AI laboratory test run on **21 October 2026 at 23:38**,
security software displayed **"We protected your PC by stopping a threat"**
for `umicom-native-llm-experiment.exe` under the `windows-ucrt64-native-llm-debug`
build folder. CTest reported:

```text
framework.ai.training.accumulation_demo  ***Not Run (BAD_COMMAND)
Process not started: umicom-native-llm-experiment.exe
```

The crucial clue is **Process not started**. This does not establish that the
experiment's calculations failed, nor does it establish that the executable is
safe. A security product blocked execution before CTest could execute the test.
The observed wording closely resembles McAfee protection alerts, but the
specific installed security product and detection name must be verified.

## Safety-first response

Do not disable real-time protection, rename the executable to evade detection,
add blanket `C:\umicom` exclusions, automatically restore a quarantined binary,
or suppress the failed CTest as if it passed. Keep the incident visible until
security software has reviewed and cleared the *specific* executable.

The source code for the experiment command is a native C23 educational client:
it loads a reviewed local experiment plan and source manifest; calculates a
small model training experiment; it only saves a checkpoint if an explicit new
path is passed. It contains no deliberate network connectivity or arbitrary
process execution. This source review cannot verify the binary that was
actually compiled on the user's Windows machine.

## Read-only PowerShell triage

Open PowerShell; these commands do not turn off protection or launch the
quarantined executable.

```powershell
Set-Location "C:\umicom\Umicom-Applications"

$exe = Join-Path (Get-Location) `
  "build\windows-ucrt64-native-llm-debug\umicom-native-llm-experiment.exe"

Get-CimInstance -Namespace root/SecurityCenter2 -ClassName AntiVirusProduct |
  Select-Object displayName, productState, pathToSignedProductExe

Test-Path -LiteralPath $exe

if (Test-Path -LiteralPath $exe) {
  Get-FileHash -LiteralPath $exe -Algorithm SHA256
  Get-AuthenticodeSignature -LiteralPath $exe |
    Select-Object Status, StatusMessage, SignerCertificate
  Get-Item -LiteralPath $exe | Select-Object FullName, Length, LastWriteTime
}

try {
  Get-MpThreatDetection | Where-Object {
    $_.Resources -match 'umicom-native-llm-experiment'
  } | Format-List *
} catch {
  Write-Host "Microsoft Defender history unavailable; inspect your installed antivirus history."
}
```

An executable already moved into quarantine may no longer exist at `$exe`.
That does **not** prove it was deleted from the source tree or never built.
Windows Defender's cmdlet reports its own detections, not McAfee's quarantine
history. If McAfee is the active provider, review **McAfee → My Protection →
Quarantined Items / security history** to record the full detection name,
rule/engine version, action and original pathname.

**Evidence to capture:** vendor/product version, exact detection family/name,
file SHA-256 if still available, whether the file was quarantined or only
blocked, the UTC/local time, antivirus definition version, your OS release,
and the precise compiler/build preset used. Avoid uploading secrets or
unreviewed proprietary source data as part of a false-positive report.

## Vendor false-positive review

McAfee provides an official **Dispute a Detection & Allowlisting Data** form:
https://www.mcafee.com/en-us/consumer-support/dispute-detection-allowlisting.html

Submit the *exact detected build* with its hash and engineering context for
classification. Do not submit a renamed replacement. If Defender rather than
McAfee is confirmed, use Microsoft's official file-submission portal:
https://www.microsoft.com/en-us/wdsi/filesubmission

The antivirus provider, not a CMake variable, must determine whether a flagged
binary is a false positive. An updated verified verdict and definitions can
allow testing without weakening protection. Publisher Authenticode signing is
appropriate for reviewed distributed release executables; it can improve
publisher reputation but it never guarantees exemption from malware scanning.
Never commit signing keys or certificates containing private keys.

## Keep test evidence honest

The full test registration remains unchanged: `accumulation_demo` continues
to launch the `umicom-native-llm-experiment` binary. This delivery neither
renames it nor conceals its result. While the antivirus issue is unresolved,
other CTests may be evaluated separately:

```powershell
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure `
  -E '^framework\.ai\.training\.accumulation_demo$'
```

**Report the exclusion explicitly** — other tests passing does not mean the
full suite passed. After the vendor has cleared the exact program, restore and
rebuild from trusted source as necessary, verify the hash, then run:

```powershell
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure `
  -R '^framework\.ai\.training\.accumulation_demo$'

ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure --no-tests=error
```

If antivirus blocks another binary, follow the same process. Do not design
renaming or packer changes merely to evade antivirus analysis. A build-time
malware report is a security incident to investigate, not a reason to disable
endpoint protection.
