# Project state — Delay, Multi-FX, and Synth Harness

Last updated: 2026-08-22
Status: active continuity index; Synth Harness PR #2 P2 StatusField copy-lifetime repair is applied, exact-head CI/review pending; physical authority remains disabled.

## Purpose

This file is the durable continuity record for work spanning:

- delay pedal;
- multi-FX pedal;
- shared pedal platform;
- Synth Harness for users, developers, repairers, and small manufacturers;
- firmware/DSP, hardware, mechanics, verification, manufacturing, service, and productization.

It is not a replacement for requirements, decision logs, test evidence, BOMs, schematics, source manifests, or reports. It links their current state so a new chat or agent can recover the thread without reconstructing it from conversation history.

## Mandatory persistence rule

After every run in which any of the following occurs, this file or the nearest lane-specific `state.md` **must be updated before the run is considered complete**:

- a design decision is accepted, rejected, reversed, deferred, or moved to HOLD;
- firmware, DSP, hardware, mechanical, UI, test, BOM, cost, roadmap, or product-scope artifacts are created or modified;
- a risk, dependency, assumption, blocker, acceptance criterion, or next action materially changes;
- repository state, source revision, build status, measurement status, or validation evidence changes.

A report or chat answer alone is not durable project state.

If the active working folder is writable, update its lane-local `state.md`. If not, update this GitHub file. Never claim persistence unless the file was actually written and the resulting path/commit is reported.

A run with no material state change may leave the file unchanged, but the run output must state: `State update: not required — no decision or artifact state changed.`

## Update order

1. Inspect this file and any more-specific lane `state.md` before work.
2. Inspect the latest authoritative artifacts and repository SHA/dirty state.
3. Perform the work and independent critique.
4. Update affected decision/artifact/test records.
5. Update `state.md` last, so it reflects what actually exists.
6. Report the exact state file path and commit/hash or local artifact path.

## Required state entry

Each substantive run must add or refresh an entry containing:

```yaml
run:
  date: YYYY-MM-DD
  lane: shared | delay | multi-fx | synth-harness | verification | productization
  objective:
  repository_or_workspace:
  base_sha_or_snapshot:
  state_before:
  changes:
    decisions:
    artifacts_created_or_modified:
    tests_or_measurements:
  evidence:
    - path/url/commit/log
  risks_or_blockers:
  acceptance_status: PASS | PASS_WITH_GAPS | BLOCKED
  next_single_action:
  owner:
```

## Current lane index

| Lane | Current authority | Current state file | Notes |
|---|---|---|---|
| Shared platform | Repository files plus latest united engineering report and active instructions | `state.md` until a more-specific file exists | Common interfaces, reusable DSP/HAL/UI/test assets, platform decisions |
| Delay pedal | Latest delay architecture report, delay handoff context, delay AGENTS instructions | create `work_products/delay/state.md` or equivalent when that lane is materialized | Delay-centric; compact validation path plus full-featured pedal |
| Multi-FX pedal | Latest multi-FX report and multi-FX AGENTS instructions | create `work_products/multi_fx/state.md` or equivalent when that lane is materialized | Separate companion product; fixed-slot graph, not arbitrary everything-pedal |
| Synth Harness | ADR-0005, current repository evidence, accepted descriptor contract, and PR #2 candidate | `work_products/synth_harness/state.md` | Fail-closed source candidate; physical authority remains disabled |
| Verification/productization | Test plans, evidence logs, BOM/cost/manufacturing artifacts | create lane-local state when artifacts exist | Hardware tests, firmware profiling, fixtures, service, DFM, commercialization |

## Current cross-lane invariants

- Delay and multi-FX remain separate products.
- Shared infrastructure is preferred only when it does not degrade product identity, audio performance, ergonomics, serviceability, or validation clarity.
- Existing Daisy hardware is an alpha/reference baseline, not automatically a production platform.
- Real-time, hardware, mechanical, cost, and commercial claims require evidence at the appropriate gate.
- Facts, inferences, estimates, proposals, and decisions must remain distinguishable.
- Hardware testing by the user is a dependency for hardware acceptance, but it must not stop independent firmware, DSP, architecture, tooling, documentation, and test-harness work that can proceed beforehand.

## Decision log summary

ADR-0001 through ADR-0005 are active and have no superseding records. ADR-0005 remains the accepted documentation contract. Its descriptor/control-inventory implementation has historical host and six-ARM evidence on dedicated branch `agent/harness-lite-control-inventory`; PR #2 now carries the fail-closed authorization/parser repairs, the P2 StatusField copy-lifetime correction, and the reproducible CI matrix. Exact-head PR #2 CI/re-review are pending; transport and hardware validation remain `NOT_RUN / HOLD`.

## Run history

### 2026-07-28 — shared continuity scaffold

- **Objective:** establish mandatory durable state persistence across lane-specific chats and repository work.
- **Change:** created root `state.md` and defined the completion gate for future runs.
- **Acceptance:** PASS.
- **Next single action:** each new lane chat reads this file first and creates or updates its nearest lane-specific `state.md` after its first substantive work package.

### 2026-07-28 — automation continuity governance

- **Run ID:** `pedal-harness-automation-governance-20260728`.
- **Primary lane:** shared-platform.
- **Objective:** make repository state, durable decisions, language boundaries, lane isolation, and context rollover mandatory for the `Pedal & Synth Harness Design` automation.
- **Repository/workspace:** `Denys/Daisy_Pedal_Projects`, branch `main`.
- **Base state snapshot:** root `state.md` blob `c691fadce7588b4181d810fa0f2cdf264adaf762`.
- **State before:** root `state.md` contained the continuity scaffold; root `AGENTS.md` and `decision_log.md` were absent; automation `6a60de0031a88191aba946213d984ba0` did not require canonical state/decision reads, append-preserving persistence, lane isolation, or re-fresh rollover.
- **Accepted decisions:** ADR-0001 through ADR-0004 are active: separate state from decision rationale; keep Delay, Multi-FX, and Synth Harness distinct; use English for internal artifacts and Italian for visible conversation; invoke re-fresh on observable context degradation.
- **Rejected:** chat history as the primary continuity store; same-context critique labelled independent; date-only handoff names; static old-report lists as current authority; lane-local state as a substitute for root state.
- **Deferred:** behavioral validation on a future scheduled engineering run; each ADR remains `NOT_RUN` for its named behavioral test.
- **Artifacts created or modified:** root `AGENTS.md` (commit `2178c1f8b0d940d900157b1cfeff1d3363b7d397`); root `decision_log.md` (commit `cf62d2fec08b43c9b5bcb881dbc8410e5d81fd78`); existing automation `6a60de0031a88191aba946213d984ba0` updated in place.
- **Evidence inspected:** current automation record and schedule; repository metadata and permissions; repository `README.md`; root `state.md`; absence of prior root `AGENTS.md` and `decision_log.md`; current Delay/Multi-FX operating sources supplied in the Project; UPE v5.6, Fable Instruction Critique, GitHub, and re-fresh contracts.
- **Tests and verification:** deterministic contract branches PASS for material run, no-material-change, missing continuity, partial persistence failure, supersession, two same-day rollovers, no-new-chat fallback, Italian failure language, and ADR validation status. Independent Reviewer 1 found six issues; F2–F6 were repaired. Fresh Reviewer 2 reviewed package `0a659be1e52fc82d0711b66999be37ea5163379d271c0fddbce80c88a3b62e7d`, closed F2–F6, reported no new blocker, scored 48/50, and recommended ACCEPT. Technical security isolation was not proven and was not claimed. GitHub create/re-fetch verification PASS for both new root files. Automation update/re-fetch verification PASS.
- **Automation schedule after update:** enabled; daily; `DTSTART;TZID=Europe/Zurich:20260723T100000`; timezone `Europe/Zurich`; timing mode unchanged (`flexible_schedule`).
- **Assumptions:** none used to claim implementation, build, measurement, hardware validation, or new-chat creation.
- **Risks/open questions:** first real scheduled-run behavioral evidence is still pending; future Project-source availability can vary and must be dynamically inventoried.
- **Blocking issues:** none for governance deployment.
- **Next single action:** on the next scheduled run, read root `AGENTS.md`, root `state.md`, root `decision_log.md`, and the active lane state before selecting exactly one highest-value engineering increment.
- **Impact:** HW — none; FW/DSP — none; Mechanical — none; Documentation/automation — root governance, ADR persistence, verified in-place automation update, and bounded re-fresh rollover added.
- **Acceptance status:** PASS.

### 2026-07-29 — Synth Harness DeviceCapabilityDescriptor contract

- **Run ID:** `pedal-synth-harness-20260729-01`.
- **Primary lane:** `synth-harness`.
- **Objective:** define the highest-value unblocked reusable boundary for firmware identity and capabilities across diagnostics, editors, preset tooling, fixtures, service, and future update workflows.
- **Repository/workspace:** `Denys/Daisy_Pedal_Projects`, branch `main`.
- **Base SHA/snapshot:** repository HEAD `87caaed73086a7767442aeb572b91fa97f96e5a8`; root-state source blob `df885bc847c18ce16deaa785ba5f8f8a11f95ffe`.
- **State before:** continuity governance was active, but Synth Harness had no lane-local state, no active technical ADR, and no source-backed device capability contract. The exact change since the previous run was limited to governance files; no technical work product had been persisted.
- **Active decisions:** ADR-0001 through ADR-0005; no `Supersedes` chain exists. ADR-0005 is newly accepted as a documentation contract.
- **Accepted decision:** use a versioned, immutable-after-publication, evidence-scoped `DeviceCapabilityDescriptor` with separate compiled-for/observed identity, explicit unknown states, preset-layout identity, a bounded 238-byte canonical binary profile, and fail-closed authorization.
- **Rejected alternatives:** boolean-only capability flags; silent external fallback to 125B; build target as physical identity; JSON as normative control transport; pin maps or mutable state in the descriptor; preset mutation without exact compatibility; physical-operation authority from compiled-only evidence.
- **Deferred:** firmware/parser implementation, independent binary golden vector, current layout-hash execution, transport, authentication, firmware update/recovery, calibration, observed carrier identity, and hardware qualification.
- **Artifacts created/modified:** `work_products/automation_runs/pedal_synth_harness_2026-07-29_device_capability_descriptor.md` (commit `431f50efa79f4d6878a3f52f6989a49ec90b114f`); root `decision_log.md` ADR-0005 (commit `5b8e9fb572c1e3fe8e4da836ff1771aed59eb9a5`); `work_products/synth_harness/state.md` (commit `9b8aee3c890371a47ff3e816aa48baca602fe192`).
- **Evidence inspected:** root continuity files; README and MIT license; GuitarPedal README, target selector, Makefile, storage format/layout hash, base effect and hardware abstractions, all five carrier implementations, audio callback, and bootloader UI path. Exact blob SHAs are pinned in the artifact.
- **Project-source inventory:** `SOURCE_UNAVAILABLE` — no current Project-source inventory/read route was exposed. Historical chat filenames were not promoted; current repository primary evidence was sufficient.
- **Tests/measurements:** continuity gate `PASS`; source matrix `PASS`; terminal independent documentation review `PASS` after two earlier `ACCEPT_WITH_REQUIRED_CHANGES` rounds; terminal result 0 critical, 0 major, 3 minor, confidence 0.94. Firmware build `NOT_RUN`; parser/serialization `NOT_RUN`; layout hash `NOT_RUN`; transport `NOT_RUN`; hardware measurements `NOT_RUN`.
- **Assumptions:** none used to claim physical carrier, connector, revision, codec, transport, recovery, calibration, performance, build, or test status.
- **Risks/open questions:** drift between descriptors and initialization; compile target cannot prove installed hardware; exact layout hash absent; v0.1 cannot authorize physical operations; transport/security/update/recovery remain unsolved.
- **Blockers:** none for the documentation decision. Implementation acceptance is blocked until all six build configurations, parser/policy negatives, layout-hash checks, and independent 238-byte golden vector pass.
- **Next single action:** implement shared per-target constants, the immutable descriptor, and host tests for five explicit targets plus the no-macro 125B default; keep transport exposure disabled.
- **Impact:** HW — none; FW/DSP — future read-only descriptor/tests, no audio-path change; Mechanical — none; UI/service — future capability gating and accurate labelling; BOM/cost — no change, implementation estimate 1–2 days and unverified; Documentation — accepted contract, ADR, lane state, and root continuity update.
- **Acceptance status:** `PASS_WITH_GAPS` — documentation contract accepted; implementation and physical validation remain `NOT_RUN`.

### 2026-08-17 — Synth Harness source integration and six-target ARM verification

- **Run ID:** `harness-lite-control-inventory-20260817`.
- **Primary lane/objective:** `synth-harness`; integrate the cumulative Harness Lite descriptor/control inventory on a dedicated branch and verify host plus all six ARM target configurations.
- **Repository/base:** `Denys/Daisy_Pedal_Projects`, branch `agent/harness-lite-control-inventory`, base/current-main `bdb53ce13e83736b6909618adbfda3e237623649`; no merge.
- **State before:** the cumulative patch existed only in `Denys/custom-pedals`; source application, ARM integration and branch application were `NOT_RUN`.
- **Active ADRs:** ADR-0001 through ADR-0005; no new or superseding decision.
- **Changes:** applied the 14-file cumulative implementation/test package; the descriptor is built once from initialized carrier facts and current storage-layout hash; a fixed-capacity inventory enumerates supported endpoints without a transport.
- **Drift/repair:** the originating SHA matched exactly, but the published patch contained three inapplicable cosmetic EOF blank-line deletion hunks. Those hunks alone were removed; semantic content was unchanged. Initial parallel ARM link failed because one generated pre-existing-source object was corrupt; serial regeneration repaired the build artifact without source changes.
- **Validation:** descriptor strict/UBSan `458 PASS`; inventory strict/UBSan `363 PASS`; target-selection probes and conflict rejection `PASS`; pinned dependency builds `PASS`; ARM compile/link `PASS` for default, `125B`, `1590B`, `1590B_SMD`, `TERRARIUM`, and `FUNBOX` using GCC 10.3.1.
- **Boundary:** no Synth-Harness transport or MIDI transaction path added; endpoint records grant no MIDI authority; physical authorization remains unconditional false. Existing application MIDI code is unchanged.
- **Hardware/physical status:** `NOT_RUN`; no hardware validation claimed.
- **Risks/gaps:** transport framing/authentication, real client, flash/boot, target timing, MIDI transactions, electrical/audio/fixture evidence remain `HOLD`; build should remain serial or add a clean reproducibility check if parallel corruption recurs.
- **Acceptance status:** `PASS_WITH_GAPS`.
- **Next single action:** review the dedicated branch diff and open a PR if desired; keep it unmerged until review acceptance.
- **Impact:** Delay and Multi-FX unchanged; Synth Harness gains source/host/ARM evidence only; no HW, mechanical or BOM claim.

### 2026-08-21 — Synth Harness PR #2 fail-closed review repair

- **Run ID:** `harness-fail-closed-pr2-20260821`.
- **Primary lane/objective:** `synth-harness`; close the known preset-mutation fail-closed defect and convert the previous local validation matrix into an exact-head PR gate.
- **Repository/base:** `Denys/Daisy_Pedal_Projects`, branch `agent/harness-lite-control-inventory`, base `main` `bdb53ce13e83736b6909618adbfda3e237623649`; PR `#2` draft/open.
- **Changes:** `AuthorizesPresetMutation()` validates the entire descriptor before authorization; rejected `Parse()` results now invalidate descriptor target identity on every non-`Ok` status, including late unknown-critical-tag failures; a dedicated rejected-wire authorization regression is part of the strict host Makefile; Build All includes strict host, UBSan, default/no-macro and all five explicit ARM variants.
- **Review:** Codex review of PR head `82095521969d7300ad62931a9c0828c1f4848df3` found two P1 issues: rejected parse could retain an authorizable descriptor, and root/lane state lagged the new correction/CI gate. Both defects are repaired on the branch; exact repaired-head re-review is still required.
- **Validation boundary:** 2026-08-17 results remain valid historical source-recorded evidence for the pre-fix candidate. Exact repaired-head host/UBSan/ARM workflow execution has not yet been observed through GitHub Actions and is not claimed PASS. Local execution is unavailable in the current connector runtime.
- **Physical-operation boundary:** `AuthorizesPhysicalOperation()` remains unconditional false; no transport, flash, calibration, fixture, MIDI transaction, physical routing, timing, electrical, or audio evidence is added.
- **Acceptance status:** `PASS_WITH_GAPS / SOURCE_REPAIR_APPLIED / EXACT_HEAD_CI_REVIEW_PENDING / PHYSICAL_HOLD`.
- **Next single action:** inspect exact repaired-head GitHub workflow and fresh review; repair valid findings; merge only if the reviewed head remains unchanged and the required source/build gate passes or is explicitly dispositioned by repository policy.

### 2026-08-22 — Synth Harness PR #2 StatusField copy-lifetime repair

- **Run ID:** `harness-pr2-statusfield-copy-20260822`.
- **Primary lane/objective:** `synth-harness`; repair only the fresh P2 concerning copied `StatusField` ownership at exact starting head `d949e0145b678052c17ba905b6a05fec1cd69a56`.
- **Finding:** `VERIFIED` — implicit `StatusField` copies retained an owner `Descriptor*`; reassignment of a saved copy could invalidate the wrong live `ParseResult`, while a copy extracted from a temporary could retain a pointer to destroyed storage.
- **Source/test repair:** commit `15870d5d7bc9fe1e4c262ac0f12f8b869d2453e4` adds explicit detached `StatusField` copy construction and value-only copy assignment, and extends `Software/GuitarPedal/Synth-Harness/tests/test_rejected_wire_authorization.cpp` with live-result and temporary-result copy-lifetime regressions. Parser wire format and authorization policy are otherwise unchanged.
- **Focused validation:** actual modified regression translation unit strict C++20 compile `PASS`; dedicated header-level status-copy runtime probe `PASS` with ASan+UBSan. Full host Makefile suite, target-selection matrix, ARM builds, and hardware checks were `NOT_RUN` locally. PR workflow query for source commit `15870d5d7bc9fe1e4c262ac0f12f8b869d2453e4` returned no pull-request workflow runs, so exact-head CI is `NOT_OBSERVED`.
- **Decisions:** no new or superseding ADR; ADR-0005 remains active. No Delay or Multi-FX change.
- **Boundary:** `AuthorizesPhysicalOperation()` remains unconditional false; no transport, flash, calibration, fixture, MIDI transaction, physical routing, timing, electrical, audio, mechanical, or BOM claim is added.
- **Acceptance status:** `PASS_WITH_GAPS / P2_REPAIR_APPLIED / EXACT_HEAD_CI_REVIEW_PENDING / PHYSICAL_HOLD`.
- **Next single action:** obtain a fresh review and exact-head CI on the final PR head; merge only from an unchanged reviewed passing head.
