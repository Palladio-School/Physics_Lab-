const fs = require('node:fs/promises');
const http = require('node:http');
const path = require('node:path');
const { test, expect } = require('@playwright/test');

const repoRoot = path.resolve(__dirname, '..');
const docsRoot = path.join(repoRoot, 'docs');

let server;
let baseUrl;

function contentType(filePath) {
  if (filePath.endsWith('.html')) return 'text/html; charset=utf-8';
  if (filePath.endsWith('.css')) return 'text/css; charset=utf-8';
  if (filePath.endsWith('.js')) return 'application/javascript; charset=utf-8';
  if (filePath.endsWith('.png')) return 'image/png';
  return 'application/octet-stream';
}

function sampleForMode(mode) {
  return {
    id: Date.now(),
    mode,
    ax: 0.1,
    ay: 0.2,
    az: 9.7,
    gx: 0,
    gy: 0,
    gz: 0,
    force: 1.23,
    distance: 0.42,
    velocity: 0.08,
    acceleration: 0.01,
    temp1: 22.1,
    temp2: 24.3,
    tempDelta: 2.2,
    sonarEchoMs: 1.2
  };
}

async function startServer() {
  server = http.createServer(async (request, response) => {
    const requestUrl = new URL(request.url, 'http://127.0.0.1');
    if (requestUrl.pathname === '/samples') {
      const mode = Number(requestUrl.searchParams.get('mode') || 4);
      response.setHeader('Content-Type', 'application/json; charset=utf-8');
      response.end(JSON.stringify({
        ap: 'PhysicsLab-M5',
        ip: '127.0.0.1',
        sampleRate: 10,
        samples: [sampleForMode(mode)]
      }));
      return;
    }
    if (
      requestUrl.pathname === '/data' ||
      requestUrl.pathname === '/settings' ||
      requestUrl.pathname === '/calibrate' ||
      requestUrl.pathname === '/set-mode'
    ) {
      response.setHeader('Content-Type', 'application/json; charset=utf-8');
      response.end(JSON.stringify({
        ap: 'PhysicsLab-M5',
        ip: '127.0.0.1',
        sampleRate: 10,
        ...sampleForMode(4)
      }));
      return;
    }
    if (requestUrl.pathname === '/favicon.ico') {
      response.writeHead(204);
      response.end();
      return;
    }

    const decodedPath = decodeURIComponent(requestUrl.pathname === '/' ? '/index.html' : requestUrl.pathname);
    const filePath = path.normalize(path.join(docsRoot, decodedPath));
    if (!filePath.startsWith(docsRoot)) {
      response.writeHead(403);
      response.end('Forbidden');
      return;
    }

    try {
      const body = await fs.readFile(filePath);
      response.setHeader('Content-Type', contentType(filePath));
      response.end(body);
    } catch (error) {
      response.writeHead(404);
      response.end('Not found');
    }
  });

  await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
  baseUrl = `http://127.0.0.1:${server.address().port}`;
}

test.beforeAll(startServer);

test.afterAll(async () => {
  if (!server) return;
  await new Promise(resolve => server.close(resolve));
});

async function openDashboard(page) {
  const errors = [];
  page.on('console', message => {
    if (message.type() !== 'error') return;
    if (message.text().startsWith('Failed to load resource:')) return;
    errors.push(message.text());
  });
  page.on('response', response => {
    if (response.status() >= 400) errors.push(`HTTP ${response.status()} ${response.url()}`);
  });
  page.on('pageerror', error => errors.push(error.message));
  await page.goto(`${baseUrl}/?device=${encodeURIComponent(baseUrl)}`, { waitUntil: 'domcontentloaded' });
  await expect(page.locator('.workspace')).toBeVisible();
  await expect(page.locator('#sessionStatus')).toBeVisible();
  return errors;
}

async function clickExperiment(page, experimentId) {
  await page.evaluate(id => {
    const button = document.querySelector(`[data-experiment="${id}"]`);
    const group = button && button.closest('details');
    if (group) group.open = true;
    button.click();
  }, experimentId);
}

async function openRailIfCollapsed(page) {
  const opener = page.locator('#collisionRailOpen');
  if (await opener.isVisible()) await opener.click();
}

async function clickMode(page, mode) {
  await page.evaluate(nextMode => {
    const button = document.querySelector(`[data-mode="${nextMode}"]`);
    const group = button && button.closest('details');
    if (group) group.open = true;
    button.click();
  }, String(mode));
}

async function seedChartRows(page, mode, values) {
  await page.evaluate(({ mode, values }) => {
    setRunning(true);
    rows = [];
    lastSampleId = 0;
    sampleEpochMs = null;
    const samples = values.map((sample, index) => ({
      sampleId: index + 1,
      sampleMs: index * 500,
      sampleRate: 10,
      mode,
      ax: 0.1,
      ay: 0.2,
      az: 9.7,
      gx: 0,
      gy: 0,
      gz: 0,
      force: 1,
      distance: 0.3,
      velocity: 0.05,
      acceleration: 0.01,
      temp1: 22,
      temp2: 24,
      tempDelta: 2,
      sonarEchoMs: 1.2,
      ...sample
    }));
    addRows(samples);
    setRunning(false);
    draw();
  }, { mode, values });
}

async function seedHookeTrials(page) {
  await page.evaluate(() => {
    hookeTrials = [
      { extensionCm: 1.0, extensionM: 0.01, forceMean: 1.1, forceMax: 1.2, forceStdDev: 0.04, duration: 1.2, kEstimate: 110 },
      { extensionCm: 2.0, extensionM: 0.02, forceMean: 2.1, forceMax: 2.2, forceStdDev: 0.05, duration: 1.2, kEstimate: 105 },
      { extensionCm: 3.0, extensionM: 0.03, forceMean: 3.2, forceMax: 3.3, forceStdDev: 0.06, duration: 1.2, kEstimate: 107 }
    ];
    renderHookeExperiment();
    drawHookeExperimentChart(hookeTrials, hookeFit(hookeTrials));
    draw();
  });
}

async function setSplitChart(page, active) {
  await page.evaluate(active => {
    splitChart = active;
    localStorage.setItem('palladio-split-chart', active ? '1' : '0');
    updateSplitChartControls();
    updateModeUi();
  }, active);
}

async function setHookeChartView(page, view) {
  await page.evaluate(view => {
    hookeChartView = view;
    localStorage.setItem('palladio-hooke-chart-view', view);
    updateSplitChartControls();
    updateModeUi();
  }, view);
}

async function chartContractSnapshot(page) {
  return page.evaluate(() => {
    const canvasEl = document.getElementById('chart');
    const image = canvasEl.getContext('2d').getImageData(0, 0, canvasEl.width, canvasEl.height).data;
    let nonWhitePixels = 0;
    for (let index = 0; index < image.length; index += 16) {
      const alpha = image[index + 3];
      const red = image[index];
      const green = image[index + 1];
      const blue = image[index + 2];
      if (alpha > 0 && (red < 245 || green < 245 || blue < 245)) nonWhitePixels += 1;
    }
    const geometry = typeof chartGeometry === 'undefined' ? null : chartGeometry;
    return {
      title: document.getElementById('chartTitle').textContent,
      nonWhitePixels,
      geometry: geometry
        ? {
            split: Boolean(geometry.split),
            panelCount: Array.isArray(geometry.panels) ? geometry.panels.length : 0,
            panelKeys: Array.isArray(geometry.panels) ? geometry.panels.map(panel => panel.key || panel.title) : [],
            selectedSeries: Array.isArray(geometry.selectedSeries)
              ? geometry.selectedSeries.map(series => series.key)
              : [],
            plotW: Math.round(geometry.plotW),
            plotH: Math.round(geometry.plotH)
          }
        : null
    };
  });
}

test('loads dashboard scripts in dependency order and exposes expected globals', async ({ page }) => {
  const errors = await openDashboard(page);
  const scriptOrder = await page.$$eval('script[src]', scripts => scripts.map(script => script.getAttribute('src')));
  expect(scriptOrder).toEqual([
    'js/experiment-catalog.js',
    'js/app-config.js',
    'js/device-client.js',
    'js/storage-export.js',
    'js/chart-utils.js',
    'js/physics-calculations.js',
    'js/fit-utils.js',
    'js/pendulum-utils.js'
  ]);
  await expect.poll(() => errors).toEqual([]);
  await expect(page.locator('#network')).toContainText('PhysicsLab-M5');
  const globals = await page.evaluate(() => ({
    catalog: Boolean(window.PalladioExperimentCatalog),
    config: Boolean(window.PalladioConfig),
    device: Boolean(window.PalladioDeviceClient),
    storage: Boolean(window.PalladioStorageExport),
    chart: Boolean(window.PalladioChartUtils),
    physics: Boolean(window.PalladioPhysicsCalculations),
    fit: Boolean(window.PalladioFitUtils),
    pendulum: Boolean(window.PalladioPendulumUtils)
  }));
  expect(globals).toEqual({
    catalog: true,
    config: true,
    device: true,
    storage: true,
    chart: true,
    physics: true,
    fit: true,
    pendulum: true
  });
  await expect(page.locator('[data-experiment="b-rotation"]')).toHaveCount(0);
  await expect.poll(() => page.evaluate(() => Boolean(window.PalladioExperimentCatalog.experimentCatalog['b-rotation']))).toBe(false);
});

test('smokes Hooke, Sonar, Pendulum, and Collision views without console errors', async ({ page }) => {
  const errors = await openDashboard(page);

  await clickExperiment(page, 'a-motion');
  await expect(page.locator('#motionExperiment')).toBeVisible();
  await expect(page.locator('#motionWorksheetPanel')).toBeVisible();
  await expect(page.locator('#motionPanelTitle')).toContainText('Απόσταση και χρόνος');
  await expect(page.locator('#clearMotionMeasurements')).toBeVisible();
  await expect(page.locator('#motionVelocityArea')).toHaveCount(1);

  await clickExperiment(page, 'a-speed');
  await expect(page.locator('#motionExperiment')).toBeVisible();
  await expect(page.locator('#motionWorksheetTitle')).toContainText('Μέση ταχύτητα');

  await clickExperiment(page, 'b-uniform-motion');
  await expect(page.locator('#motionExperiment')).toBeVisible();
  await expect(page.locator('#motionWorksheetTitle')).toContainText('Ευθύγραμμη ομαλή κίνηση');
  await expect(page.locator('#splitChartToggleWrap')).toBeVisible();

  await clickExperiment(page, 'b-hooke');
  await expect(page.locator('#hookeExperiment')).toBeVisible();
  await expect(page.locator('#hookeExperiment h3').first()).toContainText('Νόμος του Hooke');
  await expect(page.locator('#hookeWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearHookeTrials')).toBeVisible();
  await expect(page.locator('#chartTitle')).toContainText(/Hooke|Δύναμη/);

  await clickExperiment(page, 'a-weight');
  await expect(page.locator('#weightExperiment')).toBeVisible();
  await expect(page.locator('#weightExperiment h3').first()).toContainText('Μάζα/Βάρος');
  await expect(page.locator('#weightWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearWeightTrials')).toBeVisible();
  await expect(page.locator('.weight-overview .experiment-intro-card')).toHaveCount(3);
  await expect(page.locator('#weightBody')).toBeVisible();

  await clickExperiment(page, 'b-buoyancy');
  await expect(page.locator('#buoyancyExperiment')).toBeVisible();
  await expect(page.locator('#buoyancyExperiment h3').first()).toContainText('Άνωση');
  await expect(page.locator('#buoyancyWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearBuoyancyTrials')).toBeVisible();
  await expect(page.locator('.buoyancy-overview .experiment-intro-card')).toHaveCount(3);
  await expect(page.locator('#buoyancyBody')).toBeVisible();

  await clickExperiment(page, 'a-sonar');
  await expect(page.locator('#sonarWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearSonarMeasurements')).toBeVisible();
  await expect(page.locator('#sonarMainView')).toBeVisible();
  await expect(page.locator('#sonarMainTime')).toContainText('ms');

  await clickExperiment(page, 'a-heat');
  await expect(page.locator('#heatExperiment')).toBeVisible();
  await expect(page.locator('#heatExperiment h3').first()).toContainText('Θερμική ισορροπία');
  await expect(page.locator('#heatWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearHeatMeasurements')).toBeVisible();
  await expect(page.locator('.heat-overview .experiment-intro-card')).toHaveCount(3);
  await expect(page.locator('#heatSummary')).toBeVisible();

  await clickExperiment(page, 'a-pendulum');
  await openRailIfCollapsed(page);
  await expect(page.locator('#pendulumExperiment')).toBeVisible();
  await expect(page.locator('#pendulumExperiment h3').first()).toContainText('Απλό εκκρεμές');
  await expect(page.locator('#pendulumWorksheetPanel')).toBeVisible();
  await expect(page.locator('#clearPendulumTrials')).toBeVisible();
  await expect(page.locator('.pendulum-overview .experiment-intro-card')).toHaveCount(4);
  await expect(page.locator('#pendulumCanvas')).toBeVisible();
  await expect(page.locator('#pendulumBody')).toBeVisible();
  await page.locator('#runPendulumTrial').click();
  await expect(page.locator('#sampleCount')).not.toHaveText('0');

  await clickMode(page, 0);
  await page.locator('.advanced-settings').evaluate(element => { element.open = true; });
  await page.locator('#accelView').selectOption('force');
  await openRailIfCollapsed(page);
  await expect(page.locator('#chartTitle')).toContainText('Δύναμη από επιτάχυνση');
  await expect(page.locator('#collisionExperiment')).toBeVisible();
  await expect(page.locator('#collisionMass')).toHaveValue('0.20');
  await expect(page.locator('#collisionSummary')).toBeVisible();
  await expect(page.locator('#collisionChart')).toBeVisible();
  await expect(page.locator('#collisionExperiment .experiment-detail-group')).toHaveCount(2);
  await expect(page.locator('#collisionExperiment .experiment-detail-group[open]')).toHaveCount(0);

  await expect.poll(() => errors).toEqual([]);
});

test('characterizes draw special render path order', async ({ page }) => {
  const errors = await openDashboard(page);

  await clickExperiment(page, 'b-hooke');
  await seedChartRows(page, 2, [
    { force: 1.1 },
    { force: 1.7 },
    { force: 2.4 },
    { force: 3.1 }
  ]);
  await seedHookeTrials(page);

  await setSplitChart(page, false);
  await setHookeChartView(page, 'extension');
  const hookeExtension = await chartContractSnapshot(page);
  expect(hookeExtension.title).toBe('Δύναμη - επιμήκυνση');
  expect(hookeExtension.geometry).toBeNull();
  expect(hookeExtension.nonWhitePixels).toBeGreaterThan(100);

  await setSplitChart(page, true);
  const hookeSplit = await chartContractSnapshot(page);
  expect(hookeSplit.title).toBe('Νόμος Hooke: F-t και F-Δx');
  expect(hookeSplit.geometry).toMatchObject({
    split: true,
    panelCount: 1,
    panelKeys: ['hooke-time'],
    selectedSeries: ['force']
  });
  expect(hookeSplit.geometry.plotW).toBeGreaterThan(100);
  expect(hookeSplit.geometry.plotH).toBeGreaterThan(100);
  expect(hookeSplit.nonWhitePixels).toBeGreaterThan(100);

  await clickExperiment(page, 'b-uniform-motion');
  await seedChartRows(page, 4, [
    { distance: 0.2, velocity: 0.08, acceleration: 0.01 },
    { distance: 0.4, velocity: 0.08, acceleration: 0.01 },
    { distance: 0.6, velocity: 0.08, acceleration: 0.01 },
    { distance: 0.8, velocity: 0.08, acceleration: 0.01 }
  ]);
  await setSplitChart(page, true);
  const genericSplit = await chartContractSnapshot(page);
  expect(genericSplit.title).toBe('Κίνηση με αισθητήρα υπερήχων');
  expect(genericSplit.geometry).toMatchObject({
    split: true,
    panelCount: 2,
    panelKeys: ['primary', 'secondary'],
    selectedSeries: ['distance', 'velocity']
  });
  expect(genericSplit.geometry.plotW).toBeGreaterThan(100);
  expect(genericSplit.geometry.plotH).toBeGreaterThan(100);
  expect(genericSplit.nonWhitePixels).toBeGreaterThan(100);

  await expect.poll(() => errors).toEqual([]);
});

test('prioritizes the workspace after mobile experiment selection', async ({ page }) => {
  await page.setViewportSize({ width: 390, height: 844 });
  const errors = await openDashboard(page);

  await page.evaluate(() => {
    const button = document.querySelector('[data-experiment="a-weight"]');
    button.closest('details').open = true;
  });
  await page.locator('[data-experiment="a-weight"]').click();

  await expect(page.locator('#weightExperiment')).toBeVisible();
  await expect.poll(() => page.evaluate(() => Math.round(document.querySelector('.workspace').getBoundingClientRect().top))).toBeLessThan(40);
  await expect.poll(() => page.$$eval('.lab-sidebar details[open]', groups => groups.length)).toBe(0);
  await expect.poll(() => errors).toEqual([]);
});
