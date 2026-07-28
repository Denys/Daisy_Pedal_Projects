# Project state — Delay, Multi-FX, and Synth Harness

Last updated: 2026-07-28
Status: continuity scaffold; detailed lane state must be populated and maintained by the next substantive runs.

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
| Synth Harness | Active project instructions and latest harness suitability work | create `work_products/synth_harness/state.md` or equivalent when that lane is materialized | Users, developers, repairers, small manufacturers; broader than production test |
| Verification/productization | Test plans, evidence logs, BOM/cost/manufacturing artifacts | create lane-local state when artifacts exist | Hardware tests, firmware profiling, fixtures, service, DFM, commercialization |

## Current cross-lane invariants

- Delay and multi-FX remain separate products.
- Shared infrastructure is preferred only when it does not degrade product identity, audio performance, ergonomics, serviceability, or validation clarity.
- Existing Daisy hardware is an alpha/reference baseline, not automatically a production platform.
- Real-time, hardware, mechanical, cost, and commercial claims require evidence at the appropriate gate.
- Facts, inferences, estimates, proposals, and decisions must remain distinguishable.
- Hardware testing by the user is a dependency for hardware acceptance, but it must not stop independent firmware, DSP, architecture, tooling, documentation, and test-harness work that can proceed beforehand.

## Decision log summary

No design decision is newly frozen by creation of this scaffold. The next substantive lane run must populate the current decisions from the latest authoritative artifacts rather than relying on this initial summary.

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
