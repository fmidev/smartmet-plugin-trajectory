# smartmet-plugin-trajectory

Part of [SmartMet Server](https://github.com/fmidev/smartmet-server). See the [SmartMet Server documentation](https://github.com/fmidev/smartmet-server) for a full overview of the ecosystem.

## Overview

The trajectory plugin provides an HTTP interface for computing massless particle trajectories through atmospheric wind fields. Given a starting location, time, and direction (forward/backward), it returns the computed trajectory path.

## Dependencies

- [smartmet-library-trajectory](https://github.com/fmidev/smartmet-library-trajectory) — trajectory calculation
- [smartmet-engine-querydata](https://github.com/fmidev/smartmet-engine-querydata) — wind field data access

## License

MIT — see [LICENSE](LICENSE)

## Contributing

Bug reports and pull requests are welcome on [GitHub](../../issues).
