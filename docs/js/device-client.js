(function (global) {
  const {
    DIGITAL_POLL_MS,
    SAMPLE_ENDPOINT_RETRY_MS,
    OFFLINE_POLL_MS,
    IDLE_HARDWARE_POLL_MS,
    MIN_HARDWARE_POLL_MS,
    MAX_HARDWARE_POLL_MS
  } = global.PalladioConfig;

  let deviceBaseUrl = '';
  let callbacks = {};
  let samplesEndpointAvailable = true;
  let lastSamplesEndpointAttempt = 0;
  let consecutivePollFailures = 0;
  let pollTimerId = null;

  function apiUrl(path) {
    return deviceBaseUrl + (path.startsWith('/') ? path : '/' + path);
  }

  function fetchJson(path) {
    return fetch(apiUrl(path), { cache: 'no-store' }).then(response => {
      if (!response.ok) throw new Error('HTTP ' + response.status);
      return response.json();
    });
  }

  function getState() {
    return callbacks.getState ? callbacks.getState() : {};
  }

  function nextHardwarePollDelay(receivedSamples = false) {
    const state = getState();
    if (!state.running) return IDLE_HARDWARE_POLL_MS;
    const sampleInterval = 1000 / Math.max(1, state.sampleRateHz || 1);
    const multiplier = receivedSamples ? 0.55 : 0.9;
    return Math.round(Math.max(MIN_HARDWARE_POLL_MS, Math.min(MAX_HARDWARE_POLL_MS, sampleInterval * multiplier)));
  }

  function stop() {
    clearTimeout(pollTimerId);
    pollTimerId = null;
  }

  function schedulePoll(delayMs) {
    stop();
    pollTimerId = setTimeout(() => runPoll({ reschedule: true }), delayMs);
  }

  async function runPoll(options = {}) {
    const shouldReschedule = options.reschedule !== false;
    const state = getState();
    if (state.digital) {
      if (callbacks.onDigitalTick) callbacks.onDigitalTick();
      consecutivePollFailures = 0;
      if (shouldReschedule) schedulePoll(DIGITAL_POLL_MS);
      return;
    }

    let nextDelay = nextHardwarePollDelay();
    try {
      const now = performance.now();
      const shouldTrySamples = samplesEndpointAvailable || now - lastSamplesEndpointAttempt > SAMPLE_ENDPOINT_RETRY_MS;
      if (shouldTrySamples) {
        try {
          lastSamplesEndpointAttempt = now;
          const batch = await fetchJson('/samples?since=' + state.lastSampleId);
          samplesEndpointAvailable = true;
          consecutivePollFailures = 0;
          if (callbacks.onStatusChange) callbacks.onStatusChange({ online: true, text: batch.ap + ' · ' + batch.ip });
          if (callbacks.onSampleRate) callbacks.onSampleRate(batch.sampleRate);
          if (callbacks.onSamples) callbacks.onSamples(batch.samples);
          nextDelay = nextHardwarePollDelay(Array.isArray(batch.samples) && batch.samples.length > 0);
        } catch (samplesError) {
          samplesEndpointAvailable = false;
          const data = await fetchJson('/data');
          consecutivePollFailures = 0;
          if (callbacks.onStatusChange) callbacks.onStatusChange({ online: true, text: data.ap + ' · ' + data.ip });
          if (callbacks.onData) callbacks.onData(data);
          nextDelay = nextHardwarePollDelay();
        }
      } else {
        const data = await fetchJson('/data');
        consecutivePollFailures = 0;
        if (callbacks.onStatusChange) callbacks.onStatusChange({ online: true, text: data.ap + ' · ' + data.ip });
        if (callbacks.onData) callbacks.onData(data);
        nextDelay = nextHardwarePollDelay();
      }
    } catch (error) {
      consecutivePollFailures += 1;
      if (callbacks.onStatusChange) callbacks.onStatusChange({ online: false, text: 'Αναμονή για το M5Stick' });
      nextDelay = Math.min(OFFLINE_POLL_MS * consecutivePollFailures, OFFLINE_POLL_MS * 3);
    } finally {
      if (shouldReschedule) schedulePoll(nextDelay);
    }
  }

  function configure(options = {}) {
    deviceBaseUrl = (options.deviceBaseUrl || '').replace(/\/+$/, '');
    callbacks = options;
    return global.PalladioDeviceClient;
  }

  function start() {
    stop();
    return runPoll({ reschedule: true });
  }

  function pollOnce() {
    return runPoll({ reschedule: false });
  }

  global.PalladioDeviceClient = {
    configure,
    start,
    stop,
    pollOnce,
    fetchJson,
    apiUrl
  };
})(window);
