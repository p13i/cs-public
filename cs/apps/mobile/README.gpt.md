# Cursor App

Minimal Expo-managed React Native app with a friendly "Hello
World" screen built with TypeScript. Use Expo CLI to run
locally or build native binaries for TestFlight. The Bazel
wrapper targets let you drive the same npm scripts through
the repo's standard build and test flows.

## Getting started

Install the dependencies once (outside of Bazel) so the
wrapper targets can reuse the local `node_modules/` tree:

```bash
cd cs/apps/mobile
npm install
```

After that you can either run Expo directly:

```bash
npm run start
```

or rely on Bazel to invoke the same scripts:

```bash
bazel run //cs/apps/mobile:expo_start
```

The Bazel targets simply delegate to `npm run …`, so they
work with `make build`/`make test` as soon as dependencies
are installed.

Then scan the QR code with the Expo Go app or press `i` to
launch the iOS simulator (if available).

## Running tests

A lightweight Jest + Testing Library setup is included for
UI instrumentation. Run the suite with either workflow:

```bash
npm test
# or
bazel test //cs/apps/mobile:tests
```

The `App` component exposes test IDs so the tests can verify
copy and simulate button presses. Because `bazel test`
shells out to the npm script, make sure `npm install` has
completed at least once beforehand.

## Building for TestFlight

1. Make sure you have Expo CLI installed
   (`npm install -g expo-cli`).
2. Run `npx expo prebuild` to generate the native iOS
   project.
3. From `ios/`, open Xcode and archive the build for
   TestFlight (or use `eas build` if you prefer Expo
   Application Services).

You can also run other Expo helpers through Bazel, e.g.
`bazel run //cs/apps/mobile:expo_ios` (simulator) or
`bazel run //cs/apps/mobile:expo_android`.

This project is intentionally lightweight so you can
customize it quickly.
