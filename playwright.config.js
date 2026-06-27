const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  testDir: './tests',
  testMatch: /browser-smoke\.spec\.js/,
  timeout: 30000,
  use: {
    browserName: 'chromium',
    viewport: { width: 1440, height: 1000 }
  }
});
