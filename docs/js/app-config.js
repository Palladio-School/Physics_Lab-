(function (global) {
const DEFAULT_DEVICE_URL = 'http://192.168.4.1';

const SONAR_SOUND_SPEED_WATER = 1480;
const SONAR_SOUND_SPEED_AIR = 343;
const SONAR_POSITIONS = [
  { label: 'Θέση 1', xRatio: 0.25, depthM: 0.30, level: 0.58 },
  { label: 'Θέση 2', xRatio: 0.50, depthM: 0.60, level: 0.75 },
  { label: 'Θέση 3', xRatio: 0.75, depthM: 0.90, level: 0.89 }
];
const SONAR_BOAT_SENSOR_X_RATIO = 0.502;
const SONAR_BOAT_SENSOR_BOTTOM_RATIO = 0.93;
const DIGITAL_POLL_MS = 40;
const SAMPLE_ENDPOINT_RETRY_MS = 5000;
const OFFLINE_POLL_MS = 1000;
const IDLE_HARDWARE_POLL_MS = 300;
const MIN_HARDWARE_POLL_MS = 40;
const MAX_HARDWARE_POLL_MS = 500;
const experimentCatalogGlobal = global.PalladioExperimentCatalog || {};
const modes = experimentCatalogGlobal.modes;
const experimentCatalog = experimentCatalogGlobal.experimentCatalog;

function compactNumber(value) {
  if (!isFinite(value)) return '—';
  const magnitude = Math.abs(value);
  if (magnitude !== 0 && (magnitude >= 10000 || magnitude < 0.001)) return value.toExponential(3);
  const digits = magnitude >= 100 ? 1 : (magnitude >= 10 ? 2 : 3);
  return value.toFixed(digits).replace(/\.?0+$/, '');
}

function niceNumber(value, round) {
  if (!isFinite(value) || value <= 0) return 1;
  const exponent = Math.floor(Math.log10(value));
  const fraction = value / Math.pow(10, exponent);
  let niceFraction;
  if (round) {
    if (fraction < 1.5) niceFraction = 1;
    else if (fraction < 3) niceFraction = 2;
    else if (fraction < 7) niceFraction = 5;
    else niceFraction = 10;
  } else {
    if (fraction <= 1) niceFraction = 1;
    else if (fraction <= 2) niceFraction = 2;
    else if (fraction <= 5) niceFraction = 5;
    else niceFraction = 10;
  }
  return niceFraction * Math.pow(10, exponent);
}

global.PalladioConfig = {
  DEFAULT_DEVICE_URL,
  SONAR_SOUND_SPEED_WATER,
  SONAR_SOUND_SPEED_AIR,
  SONAR_POSITIONS,
  SONAR_BOAT_SENSOR_X_RATIO,
  SONAR_BOAT_SENSOR_BOTTOM_RATIO,
  DIGITAL_POLL_MS,
  SAMPLE_ENDPOINT_RETRY_MS,
  OFFLINE_POLL_MS,
  IDLE_HARDWARE_POLL_MS,
  MIN_HARDWARE_POLL_MS,
  MAX_HARDWARE_POLL_MS,
  modes,
  experimentCatalog,
  compactNumber,
  niceNumber
};
})(window);
