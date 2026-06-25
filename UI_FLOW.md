# Student And Teacher UI Flow

This document maps the current browser dashboard flow in `docs/index.html` for classroom use. It is documentation-only: it records how the dashboard is intended to be used before any UI refactor or product redesign.

## Audience And Surfaces

The dashboard serves two classroom roles:

- Students use the experiment menu, live chart, student guide, worksheets, and CSV/export actions while collecting data.
- Teachers use the same dashboard plus the right-side experiment panels, calibration controls, sampling controls, trial tables, and export/review tools to prepare and guide the activity.

The dashboard can run from:

- GitHub Pages or a local `docs/` server while pointing at the M5 API.
- The M5 access point at `http://192.168.4.1` for hardware-backed measurements.
- A local browser-only mode for digital activities such as the pendulum simulation.

## Student Flow

### 1. Choose the activity

Students start from the left experiment menu. The current dashboard groups activities by school level:

| Group | Activities |
| --- | --- |
| A Gymnasium | Distance and time, SONAR, average speed, mass/weight and spring extension, heat, simple pendulum |
| B Gymnasium | Uniform motion, Hooke's law, buoyancy, rotation |
| Technical sensor modes | Acceleration, gyroscope, force, ultrasonic, heat |

Selecting an activity updates the title, description, graph setup, active sensor mode, and the student guide prompts. The selected activity is stored in browser `localStorage` as `palladio-active-experiment`.

### 2. Connect to the measurement source

For hardware-backed activities, the student device should be connected to the `PhysicsLab-M5` Wi-Fi network and use the M5 API at:

```text
http://192.168.4.1
```

The dashboard stores the chosen device URL in `localStorage` as `palladio-device-url`. When the dashboard is opened with a query parameter such as:

```text
?device=http://192.168.4.1
```

that value becomes the active measurement source.

The simple pendulum activity is digital-only and does not require the M5.

### 3. Measure

The primary student controls are in the instrument bar:

- `Έναρξη` starts or pauses live collection.
- `Νέα μέτρηση` clears the visible run and starts a fresh trial.
- `Αποθήκευση CSV` exports the current data.
- The calibration button runs the relevant zero/tare action when the active sensor supports it.

During live collection, the student guide treats the first state as measurement. The chart updates from `/samples?since=<sampleId>` when available, with `/data` as a fallback.

### 4. Observe

After samples exist, students inspect the graph:

- Toggle plotted series from the legend.
- Use value inspection to read a point.
- Use selection mode to mark an A-B region.
- Use pan/zoom and the `Live` control to move between a fixed view and real-time tracking.
- Use split view or experiment-specific chart modes where available.

For ultrasonic activities, the dashboard keeps one main series active at a time so distance, velocity, and acceleration are easier to compare without clutter.

### 5. Conclude

When students select an A-B region, the guide moves toward conclusion. The current dashboard supports this through:

- A-B selection statistics and graph annotations.
- Experiment-specific panels and tables for trials.
- Worksheet text areas for observations and conclusions.
- CSV export for follow-up analysis.

The main student loop is:

```text
Choose activity -> Start measurement -> Observe chart -> Select evidence -> Write conclusion -> Export or reset
```

## Teacher Flow

### 1. Prepare the setup

Before students start, the teacher should choose the activity and confirm that the dashboard selected the expected hardware mode:

| Activity type | Expected source |
| --- | --- |
| Acceleration / motion with IMU | Mode `0`, built-in M5 IMU |
| Rotation | Mode `1`, built-in M5 gyroscope |
| Force / Hooke / buoyancy | Mode `2`, HX711 + load cell |
| Distance / SONAR / speed | Mode `4`, ultrasonic sensor |
| Heat | Mode `5`, DS18B20 sensors |
| Pendulum | Browser simulation, no M5 polling |

Some activities apply defaults automatically. For example, force activities prefer a 10 Hz sampling rate, heat uses a longer time window, and pendulum stops hardware polling because it is simulated in-browser.

### 2. Calibrate and check readings

Teacher-facing calibration depends on the selected mode:

- Force / load cell: tare the load cell before hanging masses or collecting buoyancy data.
- Ultrasonic: zero the distance offset if the setup needs a reference position.
- Acceleration: use the built-in IMU readings as the baseline for acceleration activities.

Before distributing the activity, confirm:

- The live readout changes when the sensor changes.
- The graph updates at the intended rate.
- Units match the activity.
- The first few samples look plausible for the physical setup.

### 3. Guide the live activity

The right-side rail is the teacher's main review and facilitation area. It contains experiment-specific panels for:

- Hooke's law and force-extension work.
- Mass/weight and spring extension.
- Buoyancy force trials.
- Motion, collision, and ultrasonic analysis.
- Heat / thermal equilibrium.
- Pendulum timing and analysis.

The rail can be collapsed or resized so a projector view can emphasize either the graph or the teacher panels.

### 4. Review student evidence

During or after collection, the teacher can ask students to compare:

- Raw graph shape.
- A-B selection statistics.
- Repeated trials in the experiment tables.
- Worksheet observations and conclusion text.
- Exported CSV data when a deeper analysis is needed.

The dashboard stores several trial sets in browser `localStorage`, including collision, Hooke, weight, pendulum, and buoyancy trial data. This makes quick classroom iteration possible, but it is not a long-term student record system.

### 5. Export or reset

The teacher can use the generic CSV export or experiment-specific exports where present:

- Hooke trial export.
- Mass/weight extension export.
- Buoyancy export.
- Collision export.
- Pendulum export.

Use `Νέα μέτρηση` when the next group needs a clean run. For a completely clean classroom device, clear browser storage or use a fresh browser profile.

## Current UI State Model

The dashboard uses a simple student-guide state:

| State | Trigger | Student meaning |
| --- | --- | --- |
| Measure | Running or no sample data yet | Collect data from the device or simulation. |
| Observe | Samples exist, no A-B selection | Read the graph and compare visible series. |
| Conclude | A-B selection exists | Use selected evidence to write or discuss the conclusion. |

This state model should remain visible in future UI work because it matches the classroom rhythm of measurement, observation, and conclusion.

## Classroom Checklist

Before class:

- Upload current firmware to the M5StickC Plus2.
- Confirm the M5 starts the `PhysicsLab-M5` Wi-Fi network.
- Connect one browser device and open the dashboard.
- Select the intended experiment.
- Confirm sensor mode, sample rate, and units.
- Run calibration when the activity needs it.
- Collect a short test run and export CSV once.

During class:

- Keep one projected dashboard as the reference view.
- Use the student guide to keep the flow on measure, observe, conclude.
- Use A-B selection when asking for evidence.
- Reset between groups or trials.

After class:

- Export useful CSV or trial tables.
- Note any sensor noise, connection drop, calibration drift, or confusing UI step.

## Known UI Flow Risks

- `docs/index.html` is a large single file, so even small UI behavior changes need careful review.
- Several teacher and student controls share the same screen; future design work should clarify role-specific views without hiding advanced controls from teachers.
- Browser `localStorage` is useful for classroom continuity but can surprise users if an old activity or trial set reappears.
- Hardware-backed activities depend on the browser reaching the M5 API; the UI should eventually make connection state and recovery clearer.
- The pendulum flow is digital-only while most other activities are hardware-backed, so documentation and UI labels should keep that difference explicit.

## Suggested Follow-Up PRs

1. Add screenshots or short classroom walkthrough images after the visual flow is accepted.
2. Review dashboard copy for student-facing Greek consistency.
3. Measure polling and rendering performance on actual classroom tablets/laptops.
4. Split `docs/index.html` into smaller modules only after the documented flow is stable.
