# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

SmartMet trajectory plugin (`smartmet-plugin-trajectory`) — an HTTP handler for the SmartMet Server that computes massless particle trajectories through atmospheric wind fields. Given a starting location, time, pressure level, and direction (forward/backward), it returns the trajectory path in KML, KMZ, GPX, XML, or debug format.

## Build commands

```bash
make                # Build trajectory.so
make clean          # Clean build artifacts
make format         # Run clang-format on source files
make install        # Install to $(plugindir)
make rpm            # Build RPM package
```

There is no test suite — the `test/` directory does not exist and CI tests are disabled.

## Architecture

The plugin has four source files in `trajectory/`:

- **Plugin.cpp/h** — `SmartMet::Plugin::Trajectory::Plugin` implements `SmartMetPlugin`. It registers the HTTP handler at the configured URL (default `/trajectory`), parses request parameters (location, model, pressure, timestep, length, direction, plume settings), obtains wind field data from the Querydata engine, runs `NFmiTrajectorySystem::CalculateTrajectory()`, and formats output using CTPP2 templates.
- **Config.cpp/h** — Reads libconfig configuration (see `cnf/trajectory.conf.sample`) and provides defaults for all simulation parameters.

The plugin is compiled into `trajectory.so`, which the SmartMet Server loads at runtime. It depends on two engines resolved at load time:
- **Querydata engine** (`itsQEngine`) — provides wind field data (`q->info()` returns `NFmiFastQueryInfo`)
- **Geonames engine** (`itsGeoEngine`) — parses location parameters from HTTP requests

The actual trajectory math lives in `smartmet-library-trajectory` (`NFmiTrajectory`, `NFmiTrajectorySystem`, `NFmiSingleTrajector`).

## Key HTTP request parameters

`place`/`latlon`, `starttime`, `model` (default: `ecmwf_eurooppa_mallipinta`), `pressure` (hPa), `height`, `heightrange`, `timestep` (minutes), `length` (hours), `format` (kml/kmz/gpx/xml/debug), `backwards`, `isentropic`, `plumes`, `disturbance`, `radius`, `range`, `interval`, `tessellate`, `extrude`.

## Output formatting

Results are formatted through CTPP2 templates (`.c2t` files) found in the template directory (default `/usr/share/smartmet/trajectory/`). The `debug` format bypasses templates and returns `NFmiTrajectory::ToXMLStr()` directly. KMZ/KMZX formats are gzip-compressed.

## Dependencies

Links against: `libsmartmet-spine`, `libsmartmet-trajectory`, `libsmartmet-smarttools`, `libsmartmet-newbase`, `libsmartmet-macgyver`, Boost (thread, iostreams), configpp, GDAL.
