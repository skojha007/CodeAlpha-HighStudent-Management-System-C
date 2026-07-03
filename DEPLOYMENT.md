# Student Management System - Deployment Configuration

## Quick Start

### Prerequisites
- GCC (C compiler)
- Make build tool
- Linux/macOS/Windows (MinGW)

### Build & Deploy (3 steps)

**Option 1: Using the Deploy Script**
```bash
chmod +x deploy.sh
./deploy.sh
```

**Option 2: Using Make**
```bash
make build    # Compile only
make run      # Compile and run
make install  # Install system-wide
```

**Option 3: Manual Compilation**
```bash
gcc -Wall -std=c11 -O2 -o student_mgmt main.c
./student_mgmt
```

---

## Deployment Modes

### 1. **Standalone (Portable)**
```bash
make dist
# Creates: student_mgmt-release.tar.gz
# Extract on any machine and run ./student_mgmt
```

### 2. **System-Wide Installation**
```bash
make install
# Binary installed to /usr/local/bin/
# Run from anywhere: student_mgmt
```

### 3. **Development Mode**
```bash
make debug
# Includes debug symbols and verbose output
# Binary: ./build/student_mgmt_debug
```

---

## File Structure After Build

```
project/
├── main.c                    # Source code
├── Makefile                  # Build configuration
├── deploy.sh                 # Deployment script
├── build/
│   └── student_mgmt         # Compiled executable
└── deployment/
    ├── student_mgmt         # Ready-to-deploy binary
    ├── README.md
    └── run.sh               # Startup script
```

---

## Runtime Files (Generated)

| File | Purpose | Auto-Created |
|------|---------|--------------|
| `students.dat` | Binary database | Yes (first save) |
| `students.csv` | CSV export | On export (menu #7) |
| `students_backup_*.dat` | Timestamped backups | On backup (menu #12) |

---

## Environment & Dependencies

- **Compiler**: GCC 4.9+ (or any C11 compliant compiler)
- **Libraries**: Standard C library (libc) - no external deps
- **Memory**: ~1-2 MB for 100 student records
- **Disk**: ~100 KB per 100 records

---

## Troubleshooting

### Issue: `gcc: command not found`
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Fedora/RHEL
sudo dnf install gcc make
```

### Issue: `make: command not found`
```bash
# Ubuntu/Debian
sudo apt-get install make

# macOS
brew install make

# Windows (MinGW)
# Download from: https://www.mingw-w64.org/
```

### Issue: Permission denied when running binary
```bash
chmod +x student_mgmt
./student_mgmt
```

---

## Uninstall

```bash
# If installed system-wide
make uninstall

# Manual removal
sudo rm /usr/local/bin/student_mgmt
```

---

## Testing the Installation

After building/installing, verify it works:
```bash
./student_mgmt
# You should see the main menu
```

Type `17` to exit.

---

## CI/CD Integration

For GitHub Actions, use:
```yaml
- name: Build Student Management System
  run: |
    make clean
    make build
    ls -la build/student_mgmt
```

---

## Performance Notes

- **Single-file binary**: No dependencies to manage
- **Fast startup**: <100ms
- **Memory efficient**: Loads only active session data
- **File I/O optimized**: 4KB chunks for backup/restore

---

**Questions?** See `README.md` for feature documentation.
