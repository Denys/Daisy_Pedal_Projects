# Synth Harness Device Capability Descriptor Contract v0.1

- **Run ID:** `pedal-synth-harness-20260729-01`
- **Primary lane:** `synth-harness`
- **Status:** accepted documentation contract; implementation `NOT_RUN`
- **Repository:** `Denys/Daisy_Pedal_Projects`
- **Base commit:** `87caaed73086a7767442aeb572b91fa97f96e5a8`

## 1. Engineering question

How shall firmware, diagnostics, preset tooling, browser editors, fixtures, and service tools identify a supported pedal carrier and discover its implemented capabilities without duplicating compile-time variant logic?

## 2. Acceptance criteria

The contract is acceptable when it:

1. identifies the firmware target and carrier independently;
2. distinguishes implemented firmware capability from physical-hardware claims;
3. represents unknown evidence explicitly rather than converting it to `false`;
4. covers every carrier currently selected in `guitar_pedal.cpp`;
5. exposes bounded counts and semantic switch roles required by diagnostics and generated UI;
6. includes the existing preset-layout identity required for safe preset operations;
7. does not expose MCU pin assignments, mutable runtime state, user presets, or secrets;
8. can be assembled without heap allocation or hardware access and is immutable after publication;
9. defines one bounded, parseable, fail-closed wire profile;
10. records an evidence-backed current matrix and its source blobs.

## 3. Evidence status

### VERIFIED

- `guitar_pedal.cpp` selects one of `125B`, `1590B`, `1590B_SMD`, `TERRARIUM`, or `FUNBOX` at compile time and defaults to `125B`.
- `BaseHardwareModule` exposes knob, switch, encoder, and LED counts plus boolean support queries for stereo, MIDI, display, encoder, and true bypass.
- Current carrier implementations initialize their controls and capability flags in their `Init()` methods.
- Persistent storage uses `SETTINGS_FILE_FORMAT_VERSION = 10`, a 32-bit effect-layout hash, and a fixed 1024-word region containing metadata and parameter payloads.

### DERIVED

- Existing capability information is distributed across variant selection, protected booleans, initialized vectors, and switch metadata; there is no single immutable record suitable for a host tool or diagnostic report.
- A descriptor must distinguish `unknown` from `unsupported`: the current firmware source can prove implemented behavior, but cannot by itself prove every physical connector or electrical characteristic.

### UNVERIFIED

- Physical carrier revision identity at runtime.
- Daisy module revision, codec identity, bootloader version, serial number, and board serial.
- Physical audio channel count for carriers whose firmware does not enable stereo.
- USB MIDI, USB audio, CV/gate, expression, calibration, and firmware-update transport capabilities.
- Whether the descriptor can be exported over an existing control transport; no such transport was established in the inspected evidence.

## 4. Primary decision

**ACCEPTED as a documentation contract:** adopt a versioned, immutable-after-publication `DeviceCapabilityDescriptor` as the canonical capability-reporting boundary for Synth Harness consumers. Acceptance does not claim implementation or physical-hardware validation.

The descriptor reports what the compiled firmware implements. It must not infer physical capabilities that the firmware does not expose, and it must not become a second pin-map or board-support package.

## 5. Contract

### 5.1 Identity fields

| Field | Type | Rule |
|---|---|---|
| `schema_version` | `u16` | Starts at `1`; incompatible semantic changes increment it. |
| `descriptor_size_bytes` | `u16` | Total canonical binary size, never `sizeof(C++ struct)` or JSON text length. |
| `target_id` | stable enum | Compile target: `pedal_125b`, `pedal_1590b`, `pedal_1590b_smd`, `terrarium`, `funbox`. |
| `compiled_for_carrier_family` | stable enum | Carrier family selected by build configuration; not observed physical identity. |
| `observed_carrier_family` | state + stable enum | Fixed `unknown` until an authenticated/verified hardware identity mechanism exists. |
| `observed_carrier_revision` | state + bounded value | Fixed `unknown` until tied to verified hardware identity. |
| `firmware_application` | stable enum | Application identity, initially `guitar_pedal`. |
| `firmware_semver` | state + version tuple | `unknown` until generated from an authoritative build source. |
| `build_identity` | state + algorithm + fixed-width digest | Source/build identity; algorithm is explicit. |
| `settings_format_version` | `u16` | Current persistent-storage schema, presently `10`. |
| `settings_word_bits` | `u8` | Presently `32`. |
| `preset_layout_identity` | state + algorithm + `u32` | Existing effect-layout identity; required for preset operations. |

`target_id` and `compiled_for_carrier_family` identify build intent. Neither is a serial number, SKU, PCB revision, or proof of the installed physical carrier.

Identity enums reserve `0 = unknown`. Carrier-family values are:

```text
0 unknown
1 guitar_pedal_125b
2 guitar_pedal_1590b
3 guitar_pedal_1590b_smd
4 terrarium
5 funbox
```

`preset_layout_identity.algorithm = 1` means `fnv1a32_effect_layout_v1`, matching the current FNV-1a traversal of effect count, ordered effect names, separator, parameter count, and ordered parameter types. Its state is `present` only when computed from the same active effect registry used by the application; otherwise it is `unknown`.

### 5.2 Evidence-aware capability value

Every non-count capability uses a three-part value:

```text
scope: compiled_application | observed_hardware
support: supported | unsupported | unknown
evidence: implemented_source | measured | declared_hardware | unavailable
```

Rules:

- v0.1 capability fields have `scope = compiled_application`; physical claims are separate and remain unknown.
- `supported + implemented_source` means the inspected firmware initializes and uses the capability.
- `unsupported + implemented_source` means the compiled application does not initialize/use the capability, regardless of what the physical carrier might contain.
- `unknown + unavailable` is required when the compiled application evidence is insufficient.
- `measured` requires a referenced test record; the descriptor alone is not the test record.
- Consumers must fail closed on unknown capabilities that can affect safety, signal routing, destructive writes, or recovery.

Numeric values:

```text
scope:    0 unknown, 1 compiled_application, 2 observed_hardware
support:  0 unknown, 1 unsupported, 2 supported
evidence: 0 unavailable, 1 implemented_source, 2 measured, 3 declared_hardware
```

Legal v0.1 combinations are `(compiled_application, supported, implemented_source)`,
`(compiled_application, unsupported, implemented_source)`, and
`(compiled_application, unknown, unavailable)`. All other combinations are rejected by the v0.1 parser. Measured and declared-hardware claims are reserved for a later schema that also carries immutable provenance references.

### 5.3 Bounded resource counts

| Field | Type | Meaning |
|---|---|---|
| `audio_callback_input_channels` | `u8` | Channels read by the current callback. |
| `audio_callback_output_channels` | `u8` | Channels written by the current callback. |
| `analog_control_count` | `u8` | Initialized `AnalogControl` instances; not assumed to be panel knobs. |
| `switch_count` | `u8` | Initialized debounced switch inputs. |
| `encoder_count` | `u8` | Initialized encoders. |
| `led_count` | `u8` | Initialized controllable LEDs. |
| `preset_storage_region_words` | `u16` | Fixed 32-bit storage region including metadata and parameter payloads. |

Counts describe current firmware initialization, not connector inventory. A future control taxonomy may split knobs, expression inputs, CVs, buttons, toggles, and footswitches without changing the meaning of these v0.1 fields.

### 5.4 Capability set

The minimum v0.1 set is:

```text
stereo_effect_processing
midi_uart
display
encoder
relay_true_bypass
hardware_output_mute
persistent_presets
bootloader_reboot_primitive
local_ui_bootloader_reboot
remote_bootloader_reboot
cpu_load_meter
```

The following remain reserved and `unknown` until primary evidence and transport behavior exist:

```text
usb_midi
usb_audio
web_serial
firmware_update
atomic_firmware_recovery
calibration_records
diagnostic_log_export
cv_input
cv_output
gate_input
gate_output
expression_input
```

### 5.5 Semantic switch roles

Each semantic role is represented separately from the physical switch count:

```text
role: bypass | alternate
physical_index: u8 | none
availability: supported | unsupported | unknown
evidence: implemented_source | unavailable
```

v0.1 has exactly two ordered role slots: `bypass`, then `alternate`. `supported` requires
`physical_index < switch_count` and `implemented_source`; `unsupported` or `unknown`
requires `physical_index = none`. Context-dependent gestures such as save, tuner hold,
and effect cycling are outside v0.1 until a stable application-level mapping exists.

### 5.6 Normative binary wire profile

The C++ representation is logical only and must never be transmitted via `sizeof` or raw
memory. The normative v0.1 binary encoding is little-endian and bounded to 1024 bytes.

Fixed 12-byte header:

```text
offset  size  field
0       4     ASCII magic "SHD1"
4       2     schema_version = 1
6       2     header_bytes = 12
8       2     descriptor_size_bytes, range 12..1024
10      2     header_flags = 0
```

The header is followed by TLVs in strictly increasing tag order:

```text
u16 tag
u16 flags       bit 0 = critical; bits 1..15 = 0
u16 length
u8  payload[length]
```

Core tags are critical and appear exactly once:

```text
0x0001 target_id
0x0002 compiled_for_carrier_family
0x0003 firmware_application
0x0004 observed_carrier_identity
0x0005 firmware_identity
0x0006 settings_identity
0x0007 resource_counts
0x0008 capability_set
0x0009 switch_roles
```

v0.1 payload layouts are:

```text
tag     bytes  payload
0x0001  2      u16 target_id
0x0002  2      u16 compiled_for_carrier_family
0x0003  2      u16 firmware_application (1 = guitar_pedal)
0x0004  6      u8 state, u8 reserved, u16 observed_family, u16 observed_revision
0x0005  42     u8 semver_state, u8 build_state, u8 digest_algorithm,
               u8 reserved, u16 major, u16 minor, u16 patch, u8 digest[32]
0x0006  10     u16 settings_format_version, u8 settings_word_bits,
               u8 layout_state, u16 layout_algorithm, u32 layout_value
0x0007  8      u8 callback_inputs, u8 callback_outputs, u8 analog_controls,
               u8 switches, u8 encoders, u8 leds, u16 storage_region_words
0x0008  92     23 ordered records of:
               u8 capability_id, u8 scope, u8 support, u8 evidence
0x0009  8      2 ordered records of:
               u8 role_id, u8 physical_index, u8 support, u8 evidence
```

Identity `state` values are `0 unknown`, `1 present`. If `semver_state = unknown`,
major/minor/patch are zero; if present, the tuple is authoritative even when `0.0.0`.
If `build_state = unknown`, `digest_algorithm = 0` and all digest bytes are zero. If
`build_state = present`, `digest_algorithm = 1` and the SHA-256 digest is nonzero.
The reserved byte is zero. Target IDs are `1 pedal_125b`, `2 pedal_1590b`,
`3 pedal_1590b_smd`, `4 terrarium`, `5 funbox`. `physical_index = 0xff` means none.
Role IDs are `1 bypass`, `2 alternate`.

Capability IDs are:

```text
1 stereo_effect_processing       13 usb_audio
2 midi_uart                      14 web_serial
3 display                        15 firmware_update
4 encoder                        16 atomic_firmware_recovery
5 relay_true_bypass              17 calibration_records
6 hardware_output_mute           18 diagnostic_log_export
7 persistent_presets             19 cv_input
8 bootloader_reboot_primitive    20 cv_output
9 local_ui_bootloader_reboot     21 gate_input
10 remote_bootloader_reboot      22 gate_output
11 cpu_load_meter                23 expression_input
12 usb_midi
```

The v0.1 canonical descriptor contains exactly these 23 capability records and two role
records, giving `descriptor_size_bytes = 238`. Reserved bytes are zero.
The parser rejects bad magic/version/header size/total size, oversize input, truncation,
invalid enums, illegal support/evidence combinations, duplicate or out-of-order tags,
wrong fixed payload lengths, nonzero reserved bits, and unknown critical tags. It skips
unknown noncritical tags only when their bounded length fits entirely within
`descriptor_size_bytes`.

JSON is a non-normative diagnostic projection. Its `descriptor_size_bytes` value refers to
the canonical binary form, not JSON text length. It may not be used for firmware update,
preset write, calibration, or fixture actuation.

The descriptor contains no pointers, heap-owned strings, pin numbers, credentials, presets,
or mutable live control values. It is assembled into fixed storage after the active effect
registry exists, publishes the same layout hash used by persistent storage, and is immutable
thereafter. Assembly performs no allocation and no hardware access.

## 6. Current evidence matrix

The matrix reports `compiled_application` behavior from inspected source, not full physical
hardware qualification. `S/I` means `supported + implemented_source`, `U/I` means
`unsupported + implemented_source`, and `?/-` means `unknown + unavailable`.
Observed carrier family and revision are `?/-` for every target.

| Target / compiled-for carrier | Callback I/O | Stereo effect | Analog | Switch | Enc | LED | MIDI | Display | Relay | Mute | Local boot UI |
|---|---:|---|---:|---:|---:|---:|---|---|---|---|---|
| `pedal_125b` / `guitar_pedal_125b` | 2 in / 2 out | S/I | 6 | 2 | 1 | 2 | S/I | S/I | S/I | S/I | S/I |
| `pedal_1590b` / `guitar_pedal_1590b` | 2 in / 2 out | S/I | 4 | 2 | 0 | 2 | S/I | U/I | U/I | U/I | U/I |
| `pedal_1590b_smd` / `guitar_pedal_1590b_smd` | 2 in / 2 out | S/I | 4 | 2 | 0 | 2 | S/I | U/I | S/I | S/I | U/I |
| `terrarium` / `terrarium` | 2 in / 2 out | U/I | 6 | 6 | 0 | 2 | U/I | U/I | U/I | U/I | U/I |
| `funbox` / `funbox` | 2 in / 2 out | S/I | 7 | 12 | 0 | 2 | S/I | U/I | U/I | U/I | U/I |

For all five targets:

| Field | Value |
|---|---|
| `observed_carrier_family` | `unknown` |
| `observed_carrier_revision` | `unknown` |
| `settings_format_version` | `10` |
| `settings_word_bits` | `32` |
| `preset_storage_region_words` | `1024` |
| `persistent_presets` | `S/I` |
| `bootloader_reboot_primitive` | `S/I` |
| `remote_bootloader_reboot` | `U/I` |
| `cpu_load_meter` | `S/I` |
| `bypass` role | `physical_index = 0`, `S/I` |
| `alternate` role | `physical_index = 1`, `S/I` |

The source comment identifies Funbox analog control index 6 as an expression input. v0.1
therefore reports seven analog controls and does not call all seven knobs.

Reserved capabilities `usb_midi`, `usb_audio`, `web_serial`, `firmware_update`,
`atomic_firmware_recovery`, `calibration_records`, `diagnostic_log_export`, CV/gate, and
expression input semantics are `?/-`. Physical stereo capability for Terrarium is
`UNVERIFIED`; only its current mono effect-processing selection is `U/I`.

### 6.1 Complete source-backed logical fixtures

These literal fixtures are the independent test oracle for the first implementation. They
must not be generated from production descriptor constants.

| Target | ID | Compiled carrier | Observed identity | Firmware identity | Settings identity | Resources `(in,out,analog,sw,enc,led,words)` |
|---|---:|---:|---|---|---|---|
| `pedal_125b` | 1 | 1 | unknown/0/0 | semver unknown; build unknown | `10,32,layout unknown/0/0` | `2,2,6,2,1,2,1024` |
| `pedal_1590b` | 2 | 2 | unknown/0/0 | semver unknown; build unknown | `10,32,layout unknown/0/0` | `2,2,4,2,0,2,1024` |
| `pedal_1590b_smd` | 3 | 3 | unknown/0/0 | semver unknown; build unknown | `10,32,layout unknown/0/0` | `2,2,4,2,0,2,1024` |
| `terrarium` | 4 | 4 | unknown/0/0 | semver unknown; build unknown | `10,32,layout unknown/0/0` | `2,2,6,6,0,2,1024` |
| `funbox` | 5 | 5 | unknown/0/0 | semver unknown; build unknown | `10,32,layout unknown/0/0` | `2,2,7,12,0,2,1024` |

`firmware_application = 1`, `schema_version = 1`, `descriptor_size_bytes = 238`,
header flags/reserved bytes/digests are zero, and roles are
`(1 bypass, index 0, S/I)` then `(2 alternate, index 1, S/I)` for every fixture.

Complete ordered capability fixtures:

| ID / capability | 125B | 1590B | 1590B SMD | Terrarium | Funbox |
|---|---|---|---|---|---|
| 1 `stereo_effect_processing` | S/I | S/I | S/I | U/I | S/I |
| 2 `midi_uart` | S/I | S/I | S/I | U/I | S/I |
| 3 `display` | S/I | U/I | U/I | U/I | U/I |
| 4 `encoder` | S/I | U/I | U/I | U/I | U/I |
| 5 `relay_true_bypass` | S/I | U/I | S/I | U/I | U/I |
| 6 `hardware_output_mute` | S/I | U/I | S/I | U/I | U/I |
| 7 `persistent_presets` | S/I | S/I | S/I | S/I | S/I |
| 8 `bootloader_reboot_primitive` | S/I | S/I | S/I | S/I | S/I |
| 9 `local_ui_bootloader_reboot` | S/I | U/I | U/I | U/I | U/I |
| 10 `remote_bootloader_reboot` | U/I | U/I | U/I | U/I | U/I |
| 11 `cpu_load_meter` | S/I | S/I | S/I | S/I | S/I |
| 12 `usb_midi` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 13 `usb_audio` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 14 `web_serial` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 15 `firmware_update` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 16 `atomic_firmware_recovery` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 17 `calibration_records` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 18 `diagnostic_log_export` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 19 `cv_input` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 20 `cv_output` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 21 `gate_input` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 22 `gate_output` | ?/- | ?/- | ?/- | ?/- | ?/- |
| 23 `expression_input` | ?/- | ?/- | ?/- | ?/- | ?/- |

The initial source-backed fixture deliberately reports the layout identity as unknown
because no exact current hash was executed or independently materialized in this run.
This blocks external preset mutation. A later implementation may publish `present/1/value`
only when the emitted value is compared with the storage subsystem's current hash and the
FNV algorithm passes independent fixed test vectors.

## 7. Compatibility rules

1. A consumer must match `schema_version` before interpreting critical tags.
2. Preset import, export, read, or write is eligible only when `persistent_presets = S/I`,
   `firmware_application`,
   `settings_format_version`, `settings_word_bits`, and a `present`
   `preset_layout_identity` with recognized algorithm `1` and nonzero value all match.
   Missing, unknown, zero, unrecognized, or mismatched identity blocks the operation.
3. `compiled_application` scope authorizes application behavior only. Carrier-dependent
   fixture actuation, flashing, calibration, pin-level access, or physical signal routing
   requires verified observed identity plus suitable `observed_hardware` evidence.
   v0.1 cannot provide that evidence, so every such operation must refuse.
4. A descriptor mismatch produces a diagnostic refusal, not silent fallback to
   `pedal_125b`.
5. Firmware may intentionally default its no-macro build to `pedal_125b`; external tools
   may not assume that default when identity is absent.
6. `observed_carrier_revision = unknown` blocks revision-sensitive flashing, calibration,
   and pin-level fixture actions.
7. `bootloader_reboot_primitive` never implies `remote_bootloader_reboot`,
   `firmware_update`, or recovery support.

## 8. Decisive test

Centralize each carrier's per-target constants so hardware initialization and descriptor
assembly consume the same definitions. Then build all five explicit macro targets plus the
intentional no-macro `pedal_125b` default and run table-driven tests that verify:

- stable target and compiled-for-carrier identity;
- exact callback counts, analog/digital resource counts, support/evidence pairs, and roles;
- `terrarium.stereo_effect_processing = U/I` while observed physical stereo remains unknown;
- exactly one variant macro is accepted; multiple variant macros fail compilation;
- descriptor and hardware initialization share count constants, with array extents/static
  assertions where possible;
- capability IDs are exactly `1..23` in order without duplicates; role IDs are exactly
  `1,2` in order and every supported role index is in range;
- every core tag has the critical bit set; required-zero bytes are zero; actual input
  length equals the declared size exactly;
- valid target IDs paired with the wrong compiled-for-carrier IDs are rejected;
- illegal identity state/algorithm/zero-value combinations are rejected;
- reserved capabilities `12..23` reject any v0.1 value other than `?/-`;
- preset operations allow exact application/settings/layout matches and reject missing,
  unknown, or mismatched layout identities separately;
- a present descriptor layout hash must equal the storage subsystem hash; the FNV
  algorithm also passes independent fixed byte-sequence test vectors;
- reboot primitive support does not authorize remote reboot or firmware update;
- golden parser behavior for truncation, oversize, bad header/version/enums, wrong lengths,
  duplicate/out-of-order tags, illegal support/evidence pairs, unknown critical tags, and
  bounded unknown optional tags;
- no descriptor field changes when runtime controls or presets change;
- the canonical binary contains no pin assignments or mutable state.
- capability booleans are asserted against the actual carrier initialization paths, not
  only against descriptor constants; the literal section 6.1 fixtures remain separate
  from production constants.

**Pass condition:** all six build configurations match the source-backed matrix, all policy
and parser negative cases fail closed, and descriptor generation cannot drift independently
from the carrier initialization constants.

## 9. Engineering impact

- **Firmware/DSP:** adds a read-only identity/capability boundary; no audio-path change. Estimated implementation: 1.0–2.0 days including the bounded parser and six configurations.
- **Hardware:** no change. Physical revision detection remains unresolved.
- **Mechanics:** no change.
- **UI/editor:** enables capability-gated controls and accurate carrier labelling after a transport exists.
- **Verification/service:** enables deterministic fixture selection and prevents unsafe default-target assumptions.
- **BOM/cost:** no BOM change; negligible fixed flash cost expected but not measured.
- **Time-to-market:** small enabler for diagnostics, editor generation, firmware update, and service workflows.
- **Commercial leverage:** reusable integration boundary across Denys's pedals and third-party Daisy carriers.
- **Licensing/provenance:** repository `LICENSE` is MIT, copyright Keith Shepherd (2023); future derived code must retain the notice and license. This document copies no third-party implementation.

## 10. Risks and blockers

- The descriptor can drift if manually duplicated from carrier classes. Shared constants and compile-time assertions are required.
- `target_id` cannot prove which PCB is physically connected.
- The current `bool Supports*()` API collapses unsupported and unknown; descriptor generation must not overclaim physical hardware.
- Pin maps are intentionally excluded; fixtures requiring pin-level actions need a separate controlled board-support record.
- Transport exposure, authentication, update safety, and recovery are outside this increment.
- Future capabilities or identity encodings require an optional TLV or a new schema version; v0.1 core payload lengths are frozen.
- No build or hardware test was run in this documentation-only increment.

## 11. Next single action

Implement shared per-target constants, an immutable-after-publication descriptor, and the
host parser tests for all six build configurations. Do not expose it over USB/MIDI until
the local contract passes.

## 12. Review disposition

- Frozen candidate SHA-256: `d688b30ceaf0b4abc3f7b17af48dceeb5ab393bcbd2616a3dd3f976c740e1bf7`.
- Independent terminal verdict: `ACCEPT` for the documentation contract.
- Terminal findings: 0 critical, 0 major, 3 minor.
- Coordinator disposition: accept the contract; carry the minor findings into required implementation validation.
- Required implementation additions: omit-each-core-TLV tests; mixed semver/build-state tests; role evidence/index negative tests; an explicit five-target physical-operation refusal table; one independent 238-byte golden vector; fixed FNV layout-hash vectors; qualify any emitted layout hash as present-only.
- No firmware build, parser execution, serialization, layout-hash execution, transport test, or hardware test ran.

## 13. Inspected primary evidence

| Path | Blob SHA |
|---|---|
| `AGENTS.md` | `d828187b50a48a6a36cc11ff015e54cd05f4459a` |
| `state.md` | `df885bc847c18ce16deaa785ba5f8f8a11f95ffe` |
| `decision_log.md` | `438d12014806c35ea1103e11f73b700233e5e5db` |
| `LICENSE` | `485e6dcbf82de35a493626bdabcc4cdaf9d326c8` |
| `Software/GuitarPedal/README.md` | `14a0b81e22074bb58ff8317b4fb25cb4f313603e` |
| `Software/GuitarPedal/guitar_pedal.cpp` | `3cad343a53b7149670f98d6ae5d472e2ce358a6f` |
| `Software/GuitarPedal/guitar_pedal_storage.h` | `c21c0d5e3eb0affb264857a977349010feba4d48` |
| `Software/GuitarPedal/guitar_pedal_storage.cpp` | `c9e58935ff02e10b0a9d9bc9af28039af949cfc9` |
| `Software/GuitarPedal/UI/guitar_pedal_ui.cpp` | `438ef2b2a5430f3ecd8b6377be252e6a1e5c7e43` |
| `Software/GuitarPedal/Hardware-Modules/base_hardware_module.h` | `9fcbe3998969303835b74f65278170cf1f8b522c` |
| `Software/GuitarPedal/Hardware-Modules/base_hardware_module.cpp` | `a244da1b7d4a90f07c48d15604c71885a5e257aa` |
| `Software/GuitarPedal/Hardware-Modules/guitar_pedal_125b.cpp` | `a0f8c6cec9105ebd50fb57bf6e3ae8839c34fca7` |
| `Software/GuitarPedal/Hardware-Modules/guitar_pedal_1590b.cpp` | `05af05069afa1b58fc98a91417ed9de24de4eb5d` |
| `Software/GuitarPedal/Hardware-Modules/guitar_pedal_1590b-SMD.cpp` | `bc1c3aac221bae5046892eb8d3ef3cb4ff8e9adf` |
| `Software/GuitarPedal/Hardware-Modules/guitar_pedal_terrarium.cpp` | `0b2dc95c865ed273554aa6e65a64169861a1be03` |
| `Software/GuitarPedal/Hardware-Modules/guitar_pedal_funbox.cpp` | `28ae1e1dfb8d3c8b96c09f6b8742c8541283c04b` |
