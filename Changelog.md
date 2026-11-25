VirtualSMC Changelog
====================
#### v1.3.9
- Fixed loadPlugin() freeing/losing key entries it had already erased from a plugin's own storage on failure (e.g. a full plugin table), which could corrupt any sensor plugin's key data on load
- Fixed the private-key unlock gate (KeyKPPW/KeyKPST) being permanently non-functional since the initial 2018 commit due to the unlock flag being cleared before its own consumer got to check it
- Fixed a null-pointer dereference in SMCBatteryManager's ACPI battery status/info handling when evaluateObject() reports success with a null object
- Fixed an out-of-bounds heap read when copying a non-NUL-terminated hardware model name from a ModelInfo/OverrideModelInfo plist entry
- Fixed a kernel panic in SMCBatteryManager when `_BSS` supplement status could not be evaluated (e.g. EC busy after wake)
- Fixed SMCBatteryManager reporting the battery as bad (BBAD) when ACPI reports an unknown warning capacity
- Fixed unit conversion of unknown (0xFFFFFFFF) battery values on mWh batteries overflowing into an arbitrary rate/capacity
- Fixed BatteryManager probe returning with its lock held on early failures, deadlocking later callers
- Fixed SMBus battery words (e.g. time to empty on AC) being truncated instead of clamped to 16 bits
- Fixed poll scheduling delta wrapping around after 256 s since the last SMBus access
- SMCLightSensor now posts the ALS change interrupt only when the value changed instead of every second
- SMCLightSensor keeps a reference to the ALS ACPI device it polls
- Fixed SMCProcessor reporting an absurd power value whenever a 32-bit RAPL energy counter wrapped around
- SMCProcessor no longer keeps polling threads for hyper-threaded logical CPUs, which only woke those CPUs every 500 ms
- Fixed SMCDellSensors possibly blocking system sleep forever while waiting for queued fan updates
- SMCDellSensors no longer wakes CPU 0 every 100 ms: the SMM thread sleeps until the next sensor refresh (every 3 s) and wakes at once for fan control writes
- SMCBatteryManager no longer runs the SMBus completion timer 10 times a second with nothing queued; it is armed only while requests are queued
- SMCBatteryManager shortens the quick-poll window after an ACPI notification from 60 s to 10 s when `_BST` reports a present rate (fewer `_STA`/`_PSR`/`_BST` evaluations, which are SMIs on Dell)
- SMCProcessor parks its per-core workers when the CPU sensor keys were not read for 10 s and resumes them on the next read
- SMCLightSensor polls `_ALI` every 3 s after 10 unchanged readings and returns to 1 s on the next change

#### v1.3.8
- Reduced unneeded power source notifications to remove repeated "PMRD: clamshell closed 0, disabled 0/0, desktopMode 0, ac 0" messages in kernel log
- Fixed freezing of battery percentage when the battery is 100% charged initially but the system is connected to an insufficiently powerful AC adapter
- Fixed a kernel heap out-of-bounds read in the SMC PMIO protocol that could leak kernel memory on a key-size mismatch
- Fixed unbounded recursion (kernel stack overflow) in ITE SuperIO fan curve lookup when a fan's PWM curve had no valid points
- Fixed a hang/use-after-free in SMCProcessor's per-CPU worker threads on a partially failed start()
- Fixed out-of-bounds reads on non-NUL-terminated device-tree string properties in SMCSuperIO/ITEDevice
- Fixed an off-by-one heap buffer overflow in SMCBatteryManager's ACPI battery/adapter discovery
- Fixed fan/temperature sensor data being written to the wrong slot in SMCDellSensors
- Wired up IT8638 (Device_0x8638) into the ITE device dispatch chain, fixing a chip that was implemented but never reachable
- Fixed buffer and integer overflows in smc-fuzzer, smcread, and fanpwmgen userspace tools

#### v1.3.7
- Added constants for macOS 26 support

#### v1.3.6
- Fixed error code when accessing missing key by index, solves PerfPowerServices freeze on 15.4+

#### v1.3.5
- Added ITE 8638 support, thx @cmd2001
- Deprecated userspace utility support for 10.12 and lower

#### v1.3.4
- Fixed loading on macOS 10.10 and older due to a MacKernelSDK regression

#### v1.3.3
- Added constants for macOS 15
- Fixed NCT6799D support

#### v1.3.2
- Added NCT6799D support by @Lorys89
- Added preliminary macOS 14 support
- Added FAN controls for ITE chips by @xCuri0

#### v1.3.1
- Fixed `smcread -l` output

#### v1.3.0
- Added preliminary macOS 13 support

#### v1.2.9
- Fix infinite panic loop when kernel panic happens on macOS 11 and newer
- Fix rare panics caused by SMCDellSensors (new way of reading SMM was ported from FanIO driver V2.3)
- Added IT8689E device

#### v1.2.8
- Do not override CPU proximity SMC key in SMCDellSensors + minor fixes in logic

#### v1.2.7
- Fixed build settings for 32-bit
- Improved EC fan monitoring instructions, thx all the contributors
- Added `fan0-dividend` to support more EC fan monitors

#### v1.2.6
- Added macOS 10.4 support for VirtualSMC and SMCBatteryManager

#### v1.2.5
- Added preliminary macOS 12 support
- Added macOS 10.6 support for SMCBatteryManager

#### v1.2.4
- Added support for NCT6683D series

#### v1.2.3
- Fixed Intel NUC EC sensors not showing proper values on some platforms
- Added `generic` EC sensor type
- Added EC fan monitoring support on `HP Pavilion 14 CE2072NL`, thx @1alessandro1

#### v1.2.2
- Improve manual fan control in SMCDellSensors (SMM access is enabled even if audio is played)
- Fixed sensor DEBUG logging with `-liludbgall` argument
- Improved startup performance when probing SuperIO chips by splitting vendors
- Added SuperIO device activation when it is disabled on probe
- Added support for Nuvoton NCT6796D-E (0xD42A)
- Added support for ITE IT8987 (requires DEBUG firmware, not available for public)
- Added Intel NUC monitoring (requires manual configuration via `ec-device`, see `EmbeddedControllers.md`)

#### v1.2.1
- Fix version publishing for VirtualSMC and plugins

#### v1.2.0
- Improve manual fan control in SMCDellSensors (switch off manual control before going to sleep), rename control boot-args (start with -dell)

#### v1.1.9
- Improve manual fan control in SMCDellSensors (use control registers 0x35a3 and 0x34a3 to cover more Dell models)
- Fix processKext in SMCDellSensors (could be called multiple times for the same kext since flag Reloadable was set)
- Reduce audio lags in SMCDellSensors when USB audio device is used
- Allow not injecting TB0T SMC key when it is unavailable in SMCBatteryManager

#### v1.1.8
- Reduce audio lags in SMCDellSensors

#### v1.1.7
- Added MacKernelSDK with Xcode 12 compatibility
- Fixed SMCDellSensors loading on macOS 10.8
- Added VirtualSMC support for 10.6 (most plugins require newer versions)
- Fixed rare kernel panic in SMCSuperIO

#### v1.1.6
- Added battery supplement info, thx @zhen-zen
- Fix audio lags in Safari caused by reading SMM in SMCDellSensors plugin
- Fix module version for SMCDellSensors, SMCBatteryManager and SMCLightSensor
- Optimised floating point sensor key reading with fewer arithmetic operations
- Improved SMCProcessor CPU power consumption by relaxing core synchronisation
- Fix key sensor key enumeration on Macmini8,x and MacBookPro models

#### v1.1.5
- Improved CHLC key value reporting
- Fixed B0PS and B0St key size to resolve broken fully charged state
- Fixed sometimes stuck battery update thx to @zhen-zen
- Added workaround for kBRemainingCapacityCmd exceeding kBFullChargeCapacityCmd
- Added preliminary 11.0 support
- Fixed SMCProcessor model detection warning
- Fixed legacy smc tool value calculation
- Fixed running smcread on 11.0 without IOKit framework
- Added a new plugin SMCDellSensors for Temp/FAN monitor/control
- Added basic SMCBatteryManager compatibility with 11.0
- Fixed crashes when trying to read CLKT key

#### v1.1.4
- Fixed incorrect revision reporting on T2 models (e.g. Macmini8,1)

#### v1.1.3
- Fixed compatibility with 10.15 debug kernel with traptrace enabled

#### v1.1.2
- Improved performance with Lilu 1.4.3 APIs

#### v1.1.1
- Fixed SMCSuperIO crashes with unsupported chips
- SMCSuperIO detected chip name to ioreg

#### v1.1.0
- Minor performance improvements
- Added OpenCore builtin protocol support (VirtualSmc.efi can still be used in other bootloaders)
- Added fan and voltage reporting in SMCSuperIO through I/O Registry (requires client updates) by @joedmru

#### v1.0.9
- Fixed multiple issues with charge level reports in SMCBatteryManager

#### v1.0.8
- Added Xcode 11 compatibility to plugin kexts on 10.14

#### v1.0.7
- Unified release archive names
- Added new ssio sensors

#### v1.0.6
- Fixed `vsmcgen=1` support on select models
- Improve SMCBatteryManager compatibility with 10.15

#### v1.0.5
- Allow loading on 10.15 without `-lilubetaall`
- Fixed SMCBatteryManager compatibility with 10.15
- Changed RGEN 3 -> 2 for AppleIntelPCHPMC.kext compatibility

#### v1.0.4
- Removed exposed REV, RBr, RPlt keys from I/O Registry
- Minor EFI driver compatibility improvements
- Synced RVUF, RVFB with REV from booter keys
- Fixed prebuilt revisions for GEN 3 chip emulation
- Dropped custom prebuilt revisions in favour of loader inject

#### v1.0.3
- Added multiple new ssio sensors
- Improved 3rd generation SMC support (they have no REV and RBr keys)

#### v1.0.2
- Fixed TC0C/TC1C selection in SMCProcessor to match mac models
- Added per-plugin debug switches (`-scpudbg`, `-sbatdbg`, `-ssiodbg`, `-alsddbg`)

#### v1.0.1
- Added Penryn CPU support to SMCProcessor
- Improved keystore management
- Initial implementation of SuperIO devices: support fans reading

#### v1.0.0
- Initial release
