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
