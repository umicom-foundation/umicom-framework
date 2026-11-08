# Source recipe format fixtures

Each JSON file is an independent input to the real Framework recipe parser. The CTest registration names its expected status. The harness retains an accepted owner while attempting the new input; every refusal must preserve that owner, and every accepted recipe must project matching task metadata that survives recipe destruction.

These are test data, not build instructions. None of the stage text is executed. Zero-filled digest values are syntactically valid unverified claims.

| Fixture | Expected status | Contract |
| --- | --- | --- |
| minimal.json | OK | One source claim projects to one semantic build task. |
| six-phases.json | OK | Every supported phase retains its own task kind. |
| reverse-order.json | OK | Forward references resolve independently of declaration order. |
| diamond.json | OK | Shared prerequisites form a valid DAG. |
| maximum-inputs.json | OK | The root admits exactly 64 distinct file claims. |
| maximum-stages.json | OK | A 32-stage chain reaches the stage boundary. |
| maximum-stage-inputs.json | OK | Sixteen distinct input references fit a stage. |
| maximum-stage-outputs.json | OK | Sixteen distinct output files fit a stage. |
| maximum-dependencies.json | OK | A fan-in stage accepts sixteen distinct prerequisites. |
| unicode-label.json | OK | Display labels retain valid UTF-8 while IDs stay portable. |
| spaces-in-path.json | OK | Internal spaces are allowed without shell interpretation. |
| escaped-key.json | OK | Escaped member spelling decodes to the known key. |
| maximum-size-claim.json | OK | INT64_MAX remains an exact claim, not a rounded floating number. |
| unknown-root.json | PARSE_ERROR | Closed root schema refuses command injection fields. |
| unknown-toolchain.json | PARSE_ERROR | Toolchain metadata does not admit a shell adapter. |
| unknown-input.json | PARSE_ERROR | Input declarations cannot silently carry network URLs. |
| unknown-stage.json | PARSE_ERROR | Stages cannot supply executable command text. |
| duplicate-root.json | ALREADY_EXISTS | Repeated root keys are ambiguous. |
| duplicate-escaped-root.json | ALREADY_EXISTS | Decoded keys, not raw spelling, determine duplicates. |
| duplicate-toolchain.json | ALREADY_EXISTS | A duplicate compiler member cannot replace an inspected path. |
| duplicate-input-member.json | ALREADY_EXISTS | Duplicate size claims are rejected before numeric interpretation. |
| duplicate-stage-member.json | ALREADY_EXISTS | A repeated phase cannot alter the admitted semantic task. |
| missing-root.json | PARSE_ERROR | Target identity is mandatory. |
| missing-toolchain.json | PARSE_ERROR | Toolchain provenance identity cannot be omitted. |
| missing-input.json | PARSE_ERROR | Every source claim requires a byte count. |
| missing-stage.json | PARSE_ERROR | Explicit dependency declarations are mandatory even when empty. |
| unsupported-schema.json | PARSE_ERROR | Future schemas are refused rather than partially interpreted. |
| schema-string.json | PARSE_ERROR | A textual version is not an integer schema. |
| negative-size.json | PARSE_ERROR | Source byte counts cannot be negative. |
| fractional-size.json | PARSE_ERROR | Byte counts are exact integers. |
| overflow-size.json | CAPACITY_EXCEEDED | A claim exceeding signed 64-bit admission is refused. |
| empty-inputs.json | PARSE_ERROR | A source recipe requires at least one source claim. |
| empty-stages.json | PARSE_ERROR | An empty stage plan is not a build recipe. |
| empty-outputs.json | PARSE_ERROR | Each stage declares at least one output file. |
| non-string-reference.json | PARSE_ERROR | References cannot use null as an implicit source. |
| excess-inputs.json | CAPACITY_EXCEEDED | Root input bounds are checked before writing fixed storage. |
| excess-stages.json | CAPACITY_EXCEEDED | Stage bounds are checked before descriptor writes. |
| excess-stage-dependencies.json | CAPACITY_EXCEEDED | Stage dependencies capacity is checked before text copies. |
| excess-stage-inputs.json | CAPACITY_EXCEEDED | Stage inputs capacity is checked before text copies. |
| excess-stage-outputs.json | CAPACITY_EXCEEDED | Stage outputs capacity is checked before text copies. |
| long-id.json | CAPACITY_EXCEEDED | Fixed ID fields reserve their terminating byte. |
| long-path.json | CAPACITY_EXCEEDED | Recipe paths reject truncation beyond 511 bytes. |
| invalid-id.json | INVALID_ARGUMENT | IDs use a lowercase portable alphabet. |
| invalid-triple.json | INVALID_ARGUMENT | Build/host/target text requires nonempty triple components. |
| invalid-digest.json | INVALID_ARGUMENT | Digest claims use exactly 64 lower-case hexadecimal digits. |
| duplicate-input-id.json | ALREADY_EXISTS | Source IDs cannot alias distinct files. |
| duplicate-stage-id.json | ALREADY_EXISTS | Distinct stages cannot share one registry key. |
| empty-label.json | INVALID_ARGUMENT | A stage requires a display label. |
| control-label.json | INVALID_ARGUMENT | Labels cannot inject terminal control sequences. |
| unsupported-phase.json | PARSE_ERROR | Run is not an admitted source-build recipe phase. |
| absolute-path.json | INVALID_ARGUMENT | File claims must be relative to a separately chosen source root. |
| drive-path.json | INVALID_ARGUMENT | Drive paths cannot escape a future root mapping. |
| parent-traversal.json | INVALID_ARGUMENT | Parent traversal is refused before filesystem access. |
| current-segment.json | INVALID_ARGUMENT | Dot segments cannot create alternate file identities. |
| backslash-path.json | INVALID_ARGUMENT | One portable separator spelling prevents host-dependent aliases. |
| empty-segment.json | INVALID_ARGUMENT | Repeated separators cannot alias a declared source. |
| trailing-dot.json | INVALID_ARGUMENT | Windows trailing-dot aliases are not portable file claims. |
| trailing-space.json | INVALID_ARGUMENT | Windows trailing-space aliases are refused. |
| reserved-device.json | INVALID_ARGUMENT | DOS device stems remain reserved with an extension. |
| reserved-port.json | INVALID_ARGUMENT | Reserved serial-port stems cannot appear as directories. |
| wildcard-path.json | INVALID_ARGUMENT | Source claims identify exact files, not expansion patterns. |
| case-collision.json | ALREADY_EXISTS | Case-folded input/output overlap is refused on every host. |
| ancestor-collision.json | ALREADY_EXISTS | A file cannot also be an ancestor directory of another claim. |
| toolchain-collision.json | ALREADY_EXISTS | A compiler file cannot be overwritten by a stage output. |
| output-collision.json | ALREADY_EXISTS | Distinct stages cannot claim the same output file. |
| missing-source.json | NOT_FOUND | Stage input IDs resolve to declared root sources. |
| missing-dependency.json | NOT_FOUND | Dependencies resolve through the existing task graph. |
| duplicate-reference.json | ALREADY_EXISTS | Repeated source references cannot silently collapse. |
| duplicate-dependency.json | ALREADY_EXISTS | Repeated dependencies cannot silently collapse. |
| cycle.json | INVALID_STATE | A multi-stage cycle is refused by the shared graph. |
| escaped-nul.json | PARSE_ERROR | Decoded NUL cannot truncate a source identity. |
| trailing-document.json | PARSE_ERROR | Only one complete JSON document is accepted. |

The separate boundary harness covers null arguments, the total byte limit, malformed UTF-8, literal NUL and controlled reader failure. Native command checks verify that invalid arguments cause no file read and that the actual route reports accepted metadata or refusal. Qualification remains an explicit build and CTest step; fixture authorship alone does not establish a pass.
