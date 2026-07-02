const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const repoRoot = path.resolve(__dirname, '..');

function createLocalStorage() {
  const store = new Map();
  return {
    getItem: key => (store.has(key) ? store.get(key) : null),
    setItem: (key, value) => store.set(key, String(value)),
    removeItem: key => store.delete(key)
  };
}

function loadDashboardHelpers() {
  const context = {
    console,
    localStorage: createLocalStorage()
  };
  context.window = context;
  vm.createContext(context);

  [
    'docs/js/experiment-catalog.js',
    'docs/js/app-config.js',
    'docs/js/storage-export.js',
    'docs/js/csv-row-utils.js',
    'docs/js/readout-utils.js',
    'docs/js/experiment-view-utils.js',
    'docs/js/sonar-utils.js',
    'docs/js/chart-utils.js',
    'docs/js/physics-calculations.js',
    'docs/js/fit-utils.js',
    'docs/js/pendulum-utils.js'
  ].forEach(file => {
    const source = fs.readFileSync(path.join(repoRoot, file), 'utf8');
    vm.runInContext(source, context, { filename: file });
  });

  return context;
}

function near(actual, expected, tolerance = 1e-9) {
  assert.ok(
    Math.abs(actual - expected) <= tolerance,
    `expected ${actual} to be within ${tolerance} of ${expected}`
  );
}

function plain(value) {
  return JSON.parse(JSON.stringify(value));
}

const dashboard = loadDashboardHelpers();

assert.equal(dashboard.PalladioConfig.DEFAULT_DEVICE_URL, 'http://192.168.4.1');
assert.equal(dashboard.PalladioConfig.compactNumber(12.3), '12.3');
assert.equal(dashboard.PalladioConfig.niceNumber(43, true), 50);
assert.equal(dashboard.PalladioConfig.experimentCatalog['b-hooke'].mode, 2);

const timeScale = dashboard.PalladioChartUtils.selectedTimeScale(0.0005, 'auto');
assert.equal(timeScale.unit, 'μs');
near(timeScale.multiplier, 1e-6);
const valueScale = dashboard.PalladioChartUtils.selectedValueScale([{ unit: 'N' }], { min: 0, max: 0.002 }, 'auto');
assert.equal(valueScale.prefix, 'm');
near(valueScale.multiplier, 1e-3);
assert.equal(dashboard.PalladioChartUtils.displayUnit('N', { prefix: 'm' }), 'mN');
assert.equal(dashboard.PalladioChartUtils.formatValue(0.1234, 1), '0.12');

const weight = dashboard.PalladioPhysicsCalculations.weightResult(-0.981, 100);
near(weight.massKg, 0.1);
near(weight.theoreticalWeight, 0.981);
near(weight.experimentalG, 9.81);
assert.equal(dashboard.PalladioPhysicsCalculations.weightResult(NaN, 100), null);

const buoyancy = dashboard.PalladioPhysicsCalculations.buoyancyResult(2.0, 1.019, 100);
near(buoyancy.buoyantForce, 0.981);
near(buoyancy.theoreticalBuoyancy, 0.981);
near(buoyancy.density, 1000);
assert.equal(dashboard.PalladioPhysicsCalculations.buoyancyResult(2, 1, 0), null);

dashboard.localStorage.setItem('valid-array', '[{"x":1}]');
dashboard.localStorage.setItem('invalid-array', '{"x":1}');
assert.equal(JSON.stringify(dashboard.PalladioStorageExport.loadJsonArray('valid-array')), '[{"x":1}]');
assert.equal(JSON.stringify(dashboard.PalladioStorageExport.loadJsonArray('invalid-array')), '[]');
assert.equal(dashboard.PalladioStorageExport.escapeHtml('<tag attr="x">'), '&lt;tag attr=&quot;x&quot;&gt;');
assert.match(
  dashboard.PalladioStorageExport.buildSonarWorksheetReport(
    [{ label: 'Θέση 1', dtMs: '1.2', dtS: '0.0012', distance: '0.89' }],
    '<ok>',
    '2026-06-27'
  ),
  /&lt;ok&gt;/
);

assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildLiveSeriesCsvRows(
    [{ time: 1.23456, sample: { force: -0.981 } }],
    [{ key: 'force', unit: 'N/kg' }],
    (row, series) => row.sample[series.key]
  )),
  [
    ['time_s', 'force_N_per_kg'],
    ['1.235', '-0.98100']
  ]
);
assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildHookeTrialCsvRows([{
    extensionCm: 2.5,
    extensionM: 0.025,
    forceMean: 1.2,
    forceMax: 1.25,
    stdDev: 0.02,
    duration: 3,
    kEstimate: 48
  }])),
  [
    ['trial', 'extension_cm', 'extension_m', 'force_mean_N', 'force_max_N', 'force_stddev_N', 'duration_s', 'k_estimate_N_per_m'],
    ['1', '2.500', '0.02500', '1.200000', '1.250000', '0.020000', '3.000000', '48.000000']
  ]
);
assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildWeightTrialCsvRows([{
    massG: 100,
    massKg: 0.1,
    extensionCm: NaN,
    extensionM: NaN,
    measuredWeight: 0.98,
    theoreticalWeight: 0.981,
    experimentalG: 9.8,
    difference: -0.001,
    errorPercent: 0.1
  }]))[1],
  ['1', '100.000', '0.100000', '', '', '0.980000', '0.981000', '9.800000', '-0.001000', '0.100']
);
assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildBuoyancyTrialCsvRows([{
    volumeMl: 100,
    volumeM3: 0.0001,
    weightAir: 2,
    weightLiquid: 1.019,
    buoyantForce: 0.981,
    theoreticalBuoyancy: NaN,
    difference: NaN,
    errorPercent: NaN,
    lossPercent: 49.05,
    density: 1000
  }], dashboard.PalladioPhysicsCalculations.buoyancyResult))[1],
  ['1', '100.000', '1.000000e-4', '2.000000', '1.019000', '0.981000', '0.981000', '0.000000', '0.000', '49.050', '1000.000']
);
assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildCollisionTrialCsvRows([{
    velocity: 0.5,
    impulse: -0.12,
    peakForce: 2.5,
    meanForce: 1.1,
    duration: 0.25
  }], 0.2))[1],
  ['1', '0.50000', '-0.120000', '0.120000', '2.500000', '1.100000', '0.250000', '0.20000']
);
assert.deepEqual(
  plain(dashboard.PalladioCsvRowUtils.buildPendulumTrialCsvRows([{
    lengthM: 1,
    massG: 120,
    angleDeg: 10,
    durationS: 20,
    deltaT: 9.2,
    oscillations: 5,
    measuredPeriod: 1.84,
    period: 2
  }], 100))[1],
  ['1', '1.0000', '120.000', '10.00', '20.00', '9.2000', '5', '1.840000', '2.000000', '1.000000', '0.500000', '20.000000']
);
assert.equal(
  dashboard.PalladioReadoutUtils.summaryCard('<T1>', 0, '°C & stable'),
  '<div class="summary-card"><span>&lt;T1&gt;</span><strong>0</strong><small>°C &amp; stable</small></div>'
);
assert.equal(dashboard.PalladioReadoutUtils.joinCards(['a', '', 'b']), 'ab');
assert.equal(
  dashboard.PalladioReadoutUtils.emptyMessage('<empty>'),
  '<div class="experiment-empty">&lt;empty&gt;</div>'
);
assert.equal(
  dashboard.PalladioReadoutUtils.emptyTableRow(3, 'no trials'),
  '<tr><td colspan="3" class="experiment-empty">no trials</td></tr>'
);

const viewState = (activeMode, activeExperimentId, activeForceExperiment = '', accelDisplayMode = 'accel') => ({
  activeMode,
  activeExperimentId,
  activeForceExperiment,
  accelDisplayMode
});
assert.equal(dashboard.PalladioExperimentViewUtils.isHookeExperimentMode(viewState(2, 'b-hooke', 'hooke')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isWeightExperimentMode(viewState(2, 'a-weight', 'weight')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isBuoyancyExperimentMode(viewState(2, 'b-buoyancy', 'buoyancy')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isCollisionExperimentMode(viewState(0, '', '', 'force')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isMotionExperimentMode(viewState(4, 'b-uniform-motion')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isSonarExperimentMode(viewState(4, 'a-sonar')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isHeatExperimentMode(viewState(5, 'a-heat')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isPendulumExperimentMode(viewState(6, 'a-pendulum')), true);
assert.equal(dashboard.PalladioExperimentViewUtils.isRightRailMode(viewState(1, '')), false);
assert.equal(dashboard.PalladioExperimentViewUtils.rightRailExperimentTitle(viewState(2, 'b-hooke', 'hooke')), 'Νόμος του Hooke');
assert.equal(dashboard.PalladioExperimentViewUtils.rightRailExperimentTitle(viewState(4, 'a-sonar')), 'SONAR: Μέτρηση απόστασης με ήχο');
assert.equal(dashboard.PalladioExperimentViewUtils.rightRailExperimentTitle(viewState(99, '')), 'Πείραμα');

near(dashboard.PalladioSonarUtils.sonarRoundTripTimeMs(0.74, 1480), 1);
assert.equal(dashboard.PalladioSonarUtils.firstFiniteNumber({ a: 'x', b: '2.5' }, ['a', 'b']), 2.5);
assert.ok(Number.isNaN(dashboard.PalladioSonarUtils.firstFiniteNumber({ a: 'x' }, ['a'])));
assert.equal(dashboard.PalladioSonarUtils.sonarEchoTimeMsFromSample({ sonarEchoMs: -4 }, 343), 0);
assert.equal(dashboard.PalladioSonarUtils.sonarEchoTimeMsFromSample({ sonarEchoUs: 2500 }, 343), 2.5);
near(dashboard.PalladioSonarUtils.sonarEchoTimeMsFromSample({ distance: 0.343 }, 343), 2);
near(dashboard.PalladioSonarUtils.sonarEchoTimeMsFromSample({ distanceCm: 34.3 }, 343), 2);
assert.ok(Number.isNaN(dashboard.PalladioSonarUtils.sonarEchoTimeMsFromSample({}, 343)));

const fit = dashboard.PalladioFitUtils.linearFit([
  { x: 0, y: 1 },
  { x: 1, y: 3 },
  { x: 2, y: 5 }
]);
near(fit.slope, 2);
near(fit.intercept, 1);
near(fit.r2, 1);
assert.equal(dashboard.PalladioFitUtils.linearFit([{ x: 1, y: 2 }]), null);

const hooke = dashboard.PalladioFitUtils.hookeFit([
  { extensionM: 0.01, forceMean: 0.5 },
  { extensionM: 0.02, forceMean: 1.0 },
  { extensionM: 0.03, forceMean: 1.5 }
]);
near(hooke.slope, 50);

const collision = dashboard.PalladioFitUtils.collisionFit([
  { velocity: 0.5, impulse: -0.1 },
  { velocity: 1.0, impulse: -0.2 },
  { velocity: 1.5, impulse: -0.3 }
]);
near(collision.slope, 0.2);

assert.equal(
  JSON.stringify(dashboard.PalladioFitUtils.solveLinearSystem([[2, 1], [1, -1]], [5, 1]).map(value => Number(value.toFixed(6)))),
  '[2,1]'
);

const quadratic = dashboard.PalladioFitUtils.polynomialFit([
  { x: 0, y: 1 },
  { x: 1, y: 4 },
  { x: 2, y: 9 },
  { x: 3, y: 16 }
], 2);
near(quadratic.predict(4), 25, 1e-7);
assert.equal(dashboard.PalladioFitUtils.chooseFit([{ x: 0, y: 1 }], 'linear'), null);

const period = dashboard.PalladioPendulumUtils.pendulumPeriod(1);
near(period, 2 * Math.PI * Math.sqrt(1 / 9.81));
const pendulumStart = dashboard.PalladioPendulumUtils.pendulumSample(0, 1, 10);
near(pendulumStart.theta, 10);
near(pendulumStart.omegaPendulum, 0);
near(pendulumStart.pendulumAmplitudeCm, 1 * (10 * Math.PI / 180) * 100);
near(pendulumStart.pendulumDisplacementCm, pendulumStart.pendulumAmplitudeCm);
const pendulumQuarter = dashboard.PalladioPendulumUtils.pendulumSample(period / 4, 1, 10);
near(pendulumQuarter.theta, 0, 1e-12);
assert.ok(pendulumQuarter.omegaPendulum < 0);
assert.equal(dashboard.PalladioPendulumUtils.pendulumIntervalInfo(period, 1).oscillations, 1);
assert.equal(dashboard.PalladioPendulumUtils.pendulumIntervalInfo(period / 2, 1).label, 'περίπου T/2');
assert.match(dashboard.PalladioPendulumUtils.pendulumIntervalInfo(period * 1.31, 1, value => value.toFixed(2)).label, /^1\.31 T$/);
assert.equal(dashboard.PalladioPendulumUtils.pendulumIntervalInfo(0, 1), null);

console.log('pure helper tests passed');
