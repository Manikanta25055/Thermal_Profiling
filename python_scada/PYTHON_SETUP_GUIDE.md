# Python Environment Setup Guide

## Important: Fix the Installation Issue

The error you encountered is because you tried to install `python-qt5` (incorrect package). We need `PyQt5` instead.

### Quick Fix

If you have a virtual environment with the failed installation, remove it first:

```bash
# If using venv:
rm -rf .venv
python3 -m venv .venv
source .venv/bin/activate

# If using conda:
conda remove --name thermal-env --all
conda create -n thermal-env python=3.11
conda activate thermal-env
```

### Option 1: Automatic Installation (Recommended)

```bash
cd python_scada
bash install_dependencies.sh
```

This script will:
1. Upgrade pip
2. Install all required packages from requirements.txt
3. Verify installation

### Option 2: Manual Installation

```bash
# Upgrade pip first
pip3 install --upgrade pip

# Install packages in order
pip3 install PyQt5==5.15.9
pip3 install pyqtgraph==0.13.3
pip3 install numpy==1.24.3
pip3 install pandas==2.0.3
pip3 install scikit-learn==1.3.2
pip3 install pyserial==3.5
pip3 install matplotlib==3.8.2
pip3 install seaborn==0.13.0
```

### Option 3: Using requirements.txt

```bash
cd python_scada
pip3 install -r requirements.txt
```

## Verification

After installation, verify all packages are installed:

```bash
python3 << 'VERIFY'
import sys
print(f"Python: {sys.version}")

modules = {
    'PyQt5': 'PyQt5',
    'pyqtgraph': 'pyqtgraph',
    'numpy': 'numpy',
    'pandas': 'pandas',
    'sklearn': 'scikit-learn',
    'serial': 'pyserial',
    'matplotlib': 'matplotlib'
}

for import_name, package_name in modules.items():
    try:
        __import__(import_name)
        print(f"✓ {package_name}")
    except ImportError:
        print(f"✗ {package_name} - NOT INSTALLED")

print("\nIf all have ✓, installation is successful!")
VERIFY
```

## Troubleshooting

### PyQt5 Installation Issues on macOS

If you get Qt-related errors:

```bash
# Clear pip cache
pip3 install --no-cache-dir PyQt5

# Or use brew if installed
brew install qt5
pip3 install PyQt5
```

### scikit-learn Installation Issues

```bash
# Might need Xcode command line tools
xcode-select --install

# Then retry
pip3 install scikit-learn
```

### Module Import Errors

If you get "ModuleNotFoundError":

```bash
# Check Python is using correct environment
which python3

# Reinstall the package
pip3 uninstall [package_name]
pip3 install [package_name]
```

## Running the Dashboard

Once dependencies are installed:

```bash
cd python_scada
python3 thermal_dashboard_gui.py
```

## Using Virtual Environment (Recommended)

### Create Environment
```bash
python3 -m venv thermal_env
source thermal_env/bin/activate  # On macOS/Linux
# or
thermal_env\Scripts\activate  # On Windows
```

### Install Packages
```bash
pip3 install -r requirements.txt
```

### Deactivate Environment
```bash
deactivate
```

## Package Versions

Tested on:
- Python 3.9, 3.10, 3.11, 3.12
- macOS, Linux, Windows

Exact versions in requirements.txt are tested and known to work together.

## If Installation Still Fails

Try this complete clean installation:

```bash
# 1. Remove old virtual environment
rm -rf thermal_env

# 2. Create fresh environment
python3 -m venv thermal_env
source thermal_env/bin/activate

# 3. Upgrade pip, setuptools, wheel
pip3 install --upgrade pip setuptools wheel

# 4. Install from requirements
pip3 install -r requirements.txt

# 5. Verify
python3 -c "import PyQt5; print('PyQt5 OK')"
```

## Performance Tips

- Use Python 3.11+ for better performance
- On macOS: Install Xcode command line tools for faster builds
- Use venv instead of conda for smaller environment size
- Keep pip updated: `pip3 install --upgrade pip`

## Contact

If installation issues persist, check:
1. Python version (3.9+ required)
2. pip is up to date
3. Enough disk space (requires ~500MB)
4. Internet connection is stable
5. No proxy/firewall blocking PyPI
