# HWID Randomizer Research Tool

**Version:** 1.0.0
**Platform:** Windows 10 (x64) Only
**Purpose:** Legitimate hardware identification testing, privacy research, system forensics, and deep configuration validation

---

## ⚠️ CRITICAL DISCLAIMER

This tool is designed **exclusively for legitimate research, testing, and educational purposes** in **controlled environments that you fully own** (virtual machines, dedicated test systems).

**DO NOT use this tool for:**
- Bypassing anti-cheat systems or game/software bans
- Violating terms of service of any application or service
- Creating duplicate machine identities for fraudulent purposes
- Any illegal or unethical activities

**Use at your own risk.** The authors are not responsible for any damage, data loss, Windows re-activation issues, or misuse of this software.

---

## 📋 Features

### Hardware Identifier Randomization (12 Categories)

1. **SMBIOS / System Information** - Baseboard serial, BIOS version, system UUID
2. **TPM 2.0** - TPM serial, endorsement key hash (registry simulation)
3. **Secure Boot / HVCI** - Hypervisor-protected Code Integrity toggle
4. **MAC Address** - Network adapter hardware addresses
5. **Disk / Volume Serial** - Volume serial numbers
6. **CPU Identifier** - Processor name strings
7. **GPU Identifier** - Graphics device IDs
8. **RAM Modules** - DIMM serial numbers
9. **Monitor EDID** - Display serial numbers
10. **EFI / UEFI Variables** - Firmware variables (kernel driver required)
11. **Windows MachineGuid** - Primary Windows machine identifier
12. **USB Controllers** - USB controller serials

### Operation Modes

- **Temporary Mode:** Changes revert after reboot
- **Persistent Mode:** Changes survive reboot

### Safety Features

- Automatic System Restore Point Creation
- Full Backup of Original Values
- One-Click Revert
- Detailed Logging

---

## 🛠️ Compilation Instructions

### Method 1: Visual Studio

```bash
git clone <repository_url>
cd HWID-Randomizer
git submodule update --init --recursive
```

Open `HWIDRandomizer.sln`, select **Release | x64**, and build.

### Method 2: CMake

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

---

## 🚀 Usage

1. Run as administrator
2. Select operation mode (Persistent/Temporary)
3. Select motherboard profile (MSI MAG X670E TOMAHAWK WIFI default)
4. Click "APPLY FULL RANDOMIZATION"
5. Reboot if prompted

See `docs/USAGE.md` for detailed instructions.

---

## 📖 Documentation

- **README.md** - This file
- **docs/USAGE.md** - Detailed usage guide
- **docs/KERNEL_DRIVER_SETUP.md** - Driver installation
- **docs/API_REFERENCE.md** - Developer documentation

---

## 🛡️ Legal & Ethical Use

**Acceptable:** Privacy research, forensics, VM testing, educational purposes
**Prohibited:** Anti-cheat bypass, ban evasion, fraud, ToS violations

---

## 📜 License

MIT License - **For legitimate research and educational purposes only**

---

## 🙋 Support

Check logs in `C:\HWIDBackup\operation_log_*.txt` for errors.

**Last Updated:** 2024 | **Version:** 1.0.0 | **Compatibility:** Windows 10
