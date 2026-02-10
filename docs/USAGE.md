# HWID Randomizer - Detailed Usage Guide

## Table of Contents
1. [First-Time Setup](#first-time-setup)
2. [Operation Modes](#operation-modes)
3. [Motherboard Profiles](#motherboard-profiles)
4. [Safety Options](#safety-options)
5. [Step-by-Step Operation](#step-by-step-operation)
6. [Revert Procedure](#revert-procedure)
7. [Verification Methods](#verification-methods)
8. [Log Interpretation](#log-interpretation)

---

## First-Time Setup

### Prerequisites

1. **Windows 10** system (any build from 10240 to < 22000)
2. **Administrator account** or ability to elevate privileges
3. **Backup important data** before first use
4. **Virtual Machine** or dedicated test system (recommended)

### Initial Launch

1. **Locate the executable:**
   - After compilation: `build/x64/Release/HWIDRandomizer.exe`
   - Or your custom build location

2. **Run as administrator:**
   - **Right-click** → "Run as administrator"
   - Or double-click (UAC prompt will appear)

3. **Application window appears:**
   - Dark-themed GUI with left configuration panel
   - Right panel shows 12 randomization targets
   - Bottom area shows action buttons and console log

4. **First run checks:**
   - Application verifies administrator privileges
   - Confirms Windows 10 OS
   - Creates backup directory: `C:\HWIDBackup`

---

## Operation Modes

### Temporary Mode (Until Reboot)

**When to use:**
- Testing changes without commitment
- Evaluating hardware identifier impact
- Temporary privacy enhancement
- Situations where permanent changes are undesirable

**Behavior:**
- All modifications applied immediately
- Most changes revert automatically on reboot
- For persistent changes (disk serials), a revert task is scheduled
- No scheduled task created for re-application

**Important notes:**
- Registry changes may persist even after reboot
- Always use "Full Revert" button for complete restoration
- Network adapters will reset to original MACs on reboot

### Persistent Mode (Survives Reboot)

**When to use:**
- Long-term research environments
- Consistent hardware identity needed
- VM configurations
- Privacy-focused system configurations

**Behavior:**
- All modifications applied and persist across reboots
- Registry changes permanent until reverted
- Scheduled task created (optional) to reapply changes on boot
- Configuration saved to `C:\HWIDBackup\last_config.json`

**Important notes:**
- Windows may require re-activation after MachineGuid change
- Software tied to hardware IDs may need re-activation
- Always keep backups available for restoration

---

## Motherboard Profiles

### MSI MAG X670E TOMAHAWK WIFI (MS-7E12) - **Default**

**Best for:**
- MSI X670E-based systems
- Systems requiring MSI-specific BIOS strings
- Research on MSI hardware identification

**Applied identifiers:**
- Baseboard Product: "MAG X670E TOMAHAWK WIFI (MS-7E12)"
- Manufacturer: "Micro-Star International Co., Ltd."
- Baseboard Serial Format: `MS7E12` + 8 random alphanumeric
- BIOS Vendor: "American Megatrends International, LLC."
- BIOS Version Format: `A.xx` (xx = random 10-99)
- BIOS Date: Random date between 2022-2024

### MSI X670E Generic

**Best for:**
- Generic MSI X670E systems without specific model

**Differences from TOMAHAWK:**
- Generic baseboard product name
- Same manufacturer and BIOS vendor

### ASUS Generic

**Best for:**
- ASUS motherboards
- General ASUS hardware research

**Applied identifiers:**
- Manufacturer: "ASUSTeK COMPUTER INC."
- Generic ASUS BIOS strings

### Generic

**Best for:**
- Non-MSI, non-ASUS systems
- Universal hardware profiles
- Maximum compatibility

**Applied identifiers:**
- Generic manufacturer strings
- Standard BIOS formats

---

## Safety Options

### System Restore Point (Recommended: ✓)

**What it does:**
- Creates Windows System Restore point before operations
- Restore point name: "HWID Randomizer - [timestamp]"
- Allows full system rollback via Windows System Restore

**When to disable:**
- System Restore disabled on system
- Insufficient disk space
- Multiple consecutive runs (to save space)

**Recovery method:**
```
Control Panel → System → System Protection → System Restore
Select "HWID Randomizer - [timestamp]" restore point
```

### Backup Original Values (Recommended: ✓)

**What it does:**
- Exports all registry keys to `.reg` file
- Creates text summary of original values
- Stores backups in `C:\HWIDBackup\backup_[timestamp].reg`

**Files created:**
- `backup_YYYYMMDD_HHMMSS.reg` - Registry backup
- `backup_YYYYMMDD_HHMMSS_summary.txt` - Human-readable summary

**When to disable:**
- Never (critical safety feature)
- Only disable if you have external backups

### Null Identifiers (Advanced Option)

**What it does:**
- Sets identifiers to null/zero values instead of random
- Example: Baseboard serial → `00000000`
- Example: System UUID → `{00000000-0000-0000-0000-000000000000}`

**When to enable:**
- Research on null identifier behavior
- Testing software reactions to empty hardware IDs
- Amidewin-style null SMBIOS values

**Warnings:**
- May cause software compatibility issues
- Null MAC address breaks networking
- Windows activation may fail
- Only use in isolated test environments

---

## Step-by-Step Operation

### Standard Randomization

1. **Configure settings (left panel):**
   ```
   [ Persistent (Survives Reboot) ]  ← Select mode
   Motherboard Profile: [MSI MAG X670E TOMAHAWK WIFI]
   [✓] System Restore Point
   [✓] Backup Original Values
   [ ] Null Identifiers
   ```

2. **Review targets (right panel):**
   - All 12 targets shown with checkmarks
   - In current version: all-or-nothing (no per-target toggle)

3. **Click "▶ APPLY FULL RANDOMIZATION":**
   - Button becomes "⏸ PROCESSING..."
   - Progress bar animates (0-100%)
   - Console log shows real-time status

4. **Monitor progress:**
   ```
   [14:30:22] [INFO] Starting HWID randomization (Persistent mode)
   [14:30:22] [SUCCESS] System restore point created
   [14:30:23] [SUCCESS] Backup created
   [14:30:24] [SUCCESS] SMBIOS: Baseboard serial randomized
   [14:30:26] [SUCCESS] MAC Address: Adapter 'Ethernet' randomized
   ...
   [14:30:50] [INFO] Randomization complete!
   ```

5. **Operation completes:**
   - Message box appears:
     ```
     Randomization complete!
     Success: 10 | Warnings: 2 | Errors: 0

     Check log for details.
     [OK]
     ```

6. **If reboot required:**
   - Second message box:
     ```
     A system reboot is required for all changes to take effect.

     Reboot now?
     [Yes] [No]
     ```
   - Click **Yes** to reboot immediately
   - Click **No** to reboot manually later

7. **Post-reboot:**
   - Changes take full effect
   - Verify using Windows tools (see Verification section)

---

## Revert Procedure

### Full Revert from Backup

1. **Click "🔄 FULL REVERT"**

2. **Backup selection dialog:**
   ```
   Available Backups:
   [·] 20240210_143022 (Most Recent)
   [ ] 20240210_120530
   [ ] 20240209_183015

   [Restore] [Cancel]
   ```

3. **Select desired backup:**
   - Most recent backup selected by default
   - Or choose earlier backup if needed

4. **Click "Restore"**

5. **Confirmation prompt:**
   ```
   Restore from backup: 20240210_143022?

   This will revert all hardware identifiers to their original values.

   [Yes] [No]
   ```

6. **Restoration executes:**
   ```
   [15:10:22] [INFO] Reverting to backup: 20240210_143022
   [15:10:23] [INFO] Importing registry file...
   [15:10:25] [SUCCESS] Registry restored successfully
   [15:10:25] [INFO] Restore complete
   ```

7. **Completion message:**
   ```
   Restore complete.

   Please reboot to finalize all changes.

   [OK]
   ```

8. **Reboot system**

9. **Verification:**
   - Original identifiers restored
   - Compare with `backup_[timestamp]_summary.txt`

---

## Verification Methods

### Windows System Information (msinfo32)

1. Press `Win + R`
2. Type `msinfo32` and press Enter
3. Check values:
   - **BIOS Version/Date** → Should show randomized values
   - **System Manufacturer** → MSI or selected profile
   - **System Model** → MAG X670E TOMAHAWK WIFI (or profile)
   - **BaseBoard Product** → Should match profile

### Device Manager

1. Press `Win + X` → Device Manager
2. **Network Adapters:**
   - Right-click adapter → Properties
   - Details tab → Property: "Network Address"
   - Verify randomized MAC address

3. **Display Adapters (GPU):**
   - Expand "Display adapters"
   - Right-click → Properties → Details
   - Property: "Hardware Ids"
   - Check device ID

### Registry Editor

```
Win + R → regedit → Enter
```

Check keys:
- `HKLM\HARDWARE\DESCRIPTION\System\BIOS`
  - BaseBoardSerialNumber
  - BIOSVersion
  - SystemUUID

- `HKLM\SOFTWARE\Microsoft\Cryptography`
  - MachineGuid

### Command Line Verification

```cmd
# MAC Address
ipconfig /all

# Volume Serial
vol C:

# WMI Queries
wmic baseboard get serialnumber
wmic bios get serialnumber,version
wmic cpu get name
wmic computersystem get uuid
```

### PowerShell Verification

```powershell
# SMBIOS Info
Get-WmiObject Win32_BaseBoard | Select-Object SerialNumber, Product
Get-WmiObject Win32_BIOS | Select-Object SerialNumber, Version
Get-WmiObject Win32_ComputerSystemProduct | Select-Object UUID

# Network Adapters
Get-NetAdapter | Select-Object Name, MacAddress
```

---

## Log Interpretation

### Log File Location

`C:\HWIDBackup\operation_log_YYYYMMDD_HHMMSS.txt`

### Log Message Format

```
[HH:MM:SS] [LEVEL] Message text
```

**Levels:**
- `[INFO]` - Informational message
- `[SUCCESS]` - Operation succeeded
- `[WARNING]` - Non-critical issue, operation continued
- `[ERROR]` - Critical error, operation may have failed

### Example Log (Successful Operation)

```
[14:30:22] [INFO] ===============================================
[14:30:22] [INFO]   HWID Randomizer Research Tool v1.0
[14:30:22] [INFO] ===============================================
[14:30:22] [SUCCESS] Running with administrator privileges
[14:30:22] [SUCCESS] Windows 10 detected
[14:30:22] [INFO] Starting HWID randomization (Persistent mode, MSI MAG X670E profile)
[14:30:22] [SUCCESS] System restore point created: RP_HWID_20240210_143022
[14:30:23] [SUCCESS] Backup created: C:\HWIDBackup\backup_20240210_143022.reg
[14:30:23] [INFO] --------------------------------------------------
[14:30:23] [INFO] Module 1/12: SMBIOS / System Information
[14:30:23] [INFO] --------------------------------------------------
[14:30:23] [INFO] SMBIOS: Current baseboard serial: MS7E12AB123456
[14:30:24] [SUCCESS] SMBIOS: Baseboard serial randomized to: MS7E12XY789ABC
[14:30:24] [SUCCESS] SMBIOS: BIOS version randomized to: A.67
[14:30:24] [SUCCESS] SMBIOS: System UUID randomized
[14:30:25] [SUCCESS] SMBIOS: Randomization complete
[14:30:25] [INFO] --------------------------------------------------
[14:30:25] [INFO] Module 2/12: TPM 2.0
[14:30:25] [INFO] --------------------------------------------------
[14:30:26] [WARNING] TPM 2.0: Registry cache modified only (actual TPM hardware unchanged)
[14:30:26] [SUCCESS] TPM 2.0: Simulation complete
...
[14:30:50] [INFO] ==================================================
[14:30:50] [INFO] Randomization complete!
[14:30:50] [INFO] Success: 10 | Warnings: 2 | Errors: 0
[14:30:50] [WARNING] REBOOT REQUIRED for changes to take effect
[14:30:50] [INFO] ==================================================
```

### Common Warnings (Expected)

```
[WARNING] TPM 2.0: Registry cache modified only (actual TPM hardware unchanged)
→ Normal: TPM hardware requires ownership or kernel driver

[WARNING] EFI: Access denied (requires kernel-level access)
→ Normal: EFI variables protected without kernel driver

[WARNING] GPU: Registry modification (driver may protect device IDs)
→ Normal: Some GPU drivers prevent registry changes
```

### Common Errors (Investigate)

```
[ERROR] SMBIOS: Failed to write baseboard serial
→ Check: Administrator privileges, registry permissions

[ERROR] MAC Address: Could not find registry entry for adapter
→ Check: Network adapter enumeration, device status

[ERROR] Pre-flight: Administrator privileges required
→ Solution: Run as administrator
```

---

## Troubleshooting

### Issue: "Administrator privileges required"
**Solution:** Right-click executable → "Run as administrator"

### Issue: "This tool only supports Windows 10"
**Solution:** Verify OS version with `winver` command

### Issue: "Backup creation failed"
**Solution:** Ensure `C:\` drive has > 100 MB free space

### Issue: MAC address not randomized
**Solution:** Network adapter may be protected; check Device Manager

### Issue: Changes revert after reboot (Persistent mode)
**Solution:** Verify scheduled task created: Task Scheduler → HWIDRandomizer_Persistence

### Issue: Windows requires reactivation
**Solution:** MachineGuid change triggers activation; use backup phone/online activation

---

**For additional support, check the main README.md or log files in `C:\HWIDBackup\`**
