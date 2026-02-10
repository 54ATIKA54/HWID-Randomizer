#pragma once

/*
 * HWID Randomizer Kernel Driver - Header File
 * User-mode communication definitions
 */

// IOCTL codes (must match driver definitions)
#define IOCTL_HWID_RANDOMIZE_SMBIOS 0x800
#define IOCTL_HWID_RANDOMIZE_CPUID  0x801
#define IOCTL_HWID_GET_VERSION      0x802

// Driver device name for CreateFile()
#define HWID_DRIVER_DEVICE_NAME "\\\\.\\HWIDDriver"

// Driver version
#define HWID_DRIVER_VERSION 0x00010000  // 1.0.0
