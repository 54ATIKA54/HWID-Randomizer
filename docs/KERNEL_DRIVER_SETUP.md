# Kernel Driver Setup Guide

## ⚠️ WARNING

**Kernel drivers can cause system instability, crashes, and data loss.**

**Test signing weakens Windows security protections.**

**Only use in isolated, controlled test environments (VMs recommended).**

---

## Why Kernel Driver?

The kernel driver stub is provided for operations that **cannot** be performed from user-mode:

- **Direct SMBIOS table modification** (physical memory access)
- **CPUID instruction interception** (dynamic spoofing)
- **EFI/UEFI variable writes** (firmware interface)
- **Deep hardware register access** (chipset-level modifications)

**Note:** The provided driver is a **STUB only** - it loads and communicates but does not implement deep hardware modifications. Full implementation requires advanced kernel programming knowledge.

---

## Prerequisites

### Required Software

1. **Windows Driver Kit (WDK)**
   - Download from: https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk
   - Version: Matching your Windows SDK version
   - Install with Visual Studio integration

2. **Visual Studio 2019/2022**
   - With "Desktop development with C++" workload
   - Windows SDK 10.0.19041.0 or later

3. **Administrator Privileges**

### Test System Requirements

- **Dedicated test system or Virtual Machine** (strongly recommended)
- **Windows 10** x64
- **Secure Boot disabled** in BIOS/UEFI (required for test signing)
- **Full system backup** before proceeding

---

## Step 1: Enable Test Signing

Test signing allows loading of unsigned or test-signed drivers.

### Enable Test Mode

1. Open **Command Prompt as Administrator**

2. Execute command:
   ```cmd
   bcdedit /set testsigning on
   ```

3. Verify success:
   ```
   The operation completed successfully.
   ```

4. **Reboot the system**

5. After reboot, desktop shows watermark:
   ```
   Test Mode
   Windows 10
   Build XXXXX
   ```

### Verify Test Mode

```cmd
bcdedit /enum {current}
```

Look for:
```
testsigning             Yes
```

---

## Step 2: Compile the Driver

### Using WDK Build Environment

1. Open **x64 Native Tools Command Prompt for VS 2019/2022**

2. Navigate to driver directory:
   ```cmd
   cd C:\Path\To\HWID-Randomizer\driver
   ```

3. Build driver (example using MSBuild):
   ```cmd
   msbuild hwid_driver.vcxproj /p:Configuration=Release /p:Platform=x64
   ```

   *(Note: Full project file not included in stub - manual driver project setup required)*

4. Output:
   ```
   driver/x64/Release/hwid_driver.sys
   ```

### Alternative: Pre-built Binary

If provided with a pre-built `hwid_driver.sys`, skip compilation.

**Always verify driver source code before loading.**

---

## Step 3: Sign the Driver (Test Certificate)

### Create Test Certificate

1. **Create certificate:**
   ```cmd
   makecert -r -pe -ss PrivateCertStore -n "CN=HWIDDriverTestCert" TestCert.cer
   ```

2. **Install certificate to Trusted Root:**
   ```cmd
   certmgr.exe /add TestCert.cer /s /r localMachine root
   ```

3. **Install certificate to Trusted Publishers:**
   ```cmd
   certmgr.exe /add TestCert.cer /s /r localMachine trustedpublisher
   ```

### Sign the Driver

```cmd
signtool sign /v /s PrivateCertStore /n "HWIDDriverTestCert" /t http://timestamp.digicert.com hwid_driver.sys
```

Verify:
```cmd
signtool verify /pa /v hwid_driver.sys
```

---

## Step 4: Install the Driver

### Method 1: Using SC (Service Control)

1. **Copy driver to system directory:**
   ```cmd
   copy hwid_driver.sys C:\Windows\System32\drivers\
   ```

2. **Create service:**
   ```cmd
   sc create HWIDDriver binPath= "C:\Windows\System32\drivers\hwid_driver.sys" type= kernel start= demand
   ```

   **Important:** Space after `binPath=` and `type=` is required.

3. **Verify service created:**
   ```cmd
   sc query HWIDDriver
   ```

   Output:
   ```
   SERVICE_NAME: HWIDDriver
   TYPE               : 1  KERNEL_DRIVER
   STATE              : 1  STOPPED
   ```

### Method 2: Using Device Manager

1. Open Device Manager
2. Action → Add legacy hardware
3. Follow wizard to install driver manually

---

## Step 5: Start the Driver

### Start Service

```cmd
sc start HWIDDriver
```

Expected output:
```
SERVICE_NAME: HWIDDriver
        TYPE               : 1  KERNEL_DRIVER
        STATE              : 4  RUNNING
```

### Verify Driver Loaded

1. **Check service status:**
   ```cmd
   sc query HWIDDriver
   ```

2. **Check loaded drivers:**
   ```cmd
   driverquery | findstr HWID
   ```

3. **View debug output (DebugView):**
   - Download Sysinternals DebugView
   - Run as administrator
   - Look for `[HWID-Driver]` messages:
     ```
     [HWID-Driver] DriverEntry called
     [HWID-Driver] This is a STUB driver for demonstration only
     [HWID-Driver] Driver loaded successfully (Version 1.0.0)
     ```

---

## Step 6: User-Mode Communication

### Test Driver Communication (C++ Example)

```cpp
#include <Windows.h>
#include "../driver/hwid_driver.h"

int main() {
    // Open driver handle
    HANDLE hDriver = CreateFileA(
        HWID_DRIVER_DEVICE_NAME,  // "\\\\.\\HWIDDriver"
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hDriver == INVALID_HANDLE_VALUE) {
        printf("Failed to open driver: %d\n", GetLastError());
        return 1;
    }

    // Get driver version
    DWORD version = 0;
    DWORD bytesReturned = 0;
    BOOL result = DeviceIoControl(
        hDriver,
        IOCTL_HWID_GET_VERSION,
        NULL, 0,
        &version, sizeof(version),
        &bytesReturned,
        NULL
    );

    if (result) {
        printf("Driver version: 0x%08X\n", version);
    } else {
        printf("IOCTL failed: %d\n", GetLastError());
    }

    CloseHandle(hDriver);
    return 0;
}
```

---

## Step 7: Stop and Uninstall Driver

### Stop Driver

```cmd
sc stop HWIDDriver
```

### Delete Service

```cmd
sc delete HWIDDriver
```

### Remove Driver File

```cmd
del C:\Windows\System32\drivers\hwid_driver.sys
```

### Disable Test Signing (Restore Security)

```cmd
bcdedit /set testsigning off
```

**Reboot** to restore normal signed-driver enforcement.

---

## Troubleshooting

### Issue: "The hash for the file is not present in the specified catalog file"

**Cause:** Driver not properly signed or test mode not enabled

**Solution:**
1. Verify test signing: `bcdedit /enum {current}`
2. Re-sign driver with test certificate
3. Install certificate to Trusted Root and Trusted Publishers

### Issue: "This driver is not meant for this platform"

**Cause:** Architecture mismatch (x86 vs x64)

**Solution:**
- Rebuild driver for correct platform (x64)
- Verify with: `dumpbin /headers hwid_driver.sys`

### Issue: Driver loads but IOCTL returns ERROR_INVALID_FUNCTION

**Cause:** IOCTL code mismatch between user-mode and kernel-mode

**Solution:**
- Verify IOCTL codes match in both `hwid_driver.h` and `hwid_driver.c`
- Use `CTL_CODE` macro consistently

### Issue: System crash (BSOD) on driver load

**Cause:** Driver bug (memory corruption, improper IRP handling, etc.)

**Solution:**
1. Boot into Safe Mode
2. Delete driver service: `sc delete HWIDDriver`
3. Analyze crash dump with WinDbg
4. Fix driver code issue

### Issue: Test Mode watermark annoying

**Solution:**
- Use third-party watermark remover (not recommended)
- Or accept watermark as security reminder
- Or disable test signing when not testing

---

## Security Implications

### Risks of Test Signing

- **Malware can load unsigned drivers** on test-signed systems
- **Rootkits can bypass kernel protections**
- **System stability reduced**

### Recommendations

1. **Use only in VMs** or isolated test systems
2. **Disable test signing** when not actively testing
3. **Do not browse internet** with test signing enabled
4. **Snapshot VM state** before driver experiments
5. **Monitor driver behavior** with DebugView

---

## Advanced: Full Driver Implementation

The provided driver is a **stub**. Full implementation requires:

### SMBIOS Table Modification

```c
// Pseudo-code (requires physical memory access)
PHYSICAL_ADDRESS smbiosPhysAddr = GetSMBIOSPhysicalAddress();
PVOID smbiosVirtAddr = MmMapIoSpace(smbiosPhysAddr, smbiosSize, MmNonCached);

// Parse SMBIOS structures
// Locate Type 1 (System Information) structure
// Modify UUID field
// Recalculate checksum

MmUnmapIoSpace(smbiosVirtAddr, smbiosSize);
```

### CPUID Interception

Requires MSR (Model-Specific Register) programming and hypervisor-level interception - **extremely advanced**.

---

## Conclusion

Kernel driver development is **complex and dangerous**. The stub provided demonstrates driver loading and communication only. Full hardware spoofing requires:

- Deep Windows kernel knowledge
- Understanding of hardware specifications (SMBIOS, ACPI, etc.)
- Proper error handling to prevent crashes
- Extensive testing

**Most operations can be performed in user-mode** via registry and WMI. Use the kernel driver only when absolutely necessary and you fully understand the implications.

---

**For issues, refer to Windows Driver Kit documentation:**
https://docs.microsoft.com/en-us/windows-hardware/drivers/
