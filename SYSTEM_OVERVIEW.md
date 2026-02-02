# Thermal Profiling System - Complete Project

A comprehensive real-time thermal monitoring and anomaly detection system for equipment temperature profiling using Arduino, Python SCADA, and MATLAB visualization.

**Author:** Gonugondla Veera Manikanta (230906450)
**Course:** ELE 3162 - Measurements & Instrumentation Laboratory
**Institution:** Manipal Institute of Technology
**Date:** February 2025

---

## Project Overview

This project implements a complete thermal profiling system with:

- **Real-time data acquisition** from 4 temperature sensors via Arduino
- **PyQt5 GUI dashboard** for live visualization and monitoring
- **ML-based anomaly detection** using Isolation Forest algorithm
- **MATLAB visualization scripts** for comprehensive analysis
- **Automatic data logging** with CSV export and backup

---

## Key Features

### Hardware
- Arduino board with 4x temperature sensors (DS18B20)
- Serial communication at 115200 baud
- Thermal block with integrated reference junction

### Software Components

#### 1. Python SCADA Dashboard (`thermal_profiling_unified.py`)
- Real-time sensor trend plots using PyQtGraph
- Live statistics display (Max/Min/Avg/Gradient)
- Sample data table showing recent readings
- Connection management and status monitoring

#### 2. Real-time Anomaly Detection
- Isolation Forest ML algorithm
- Contamination rate: 15% (observable deviations only)
- Rolling buffer of 100 samples for detection
- Checks every 5 new samples for efficiency
- Feature engineering: gradient, ratios, standard deviation, deltas

#### 3. Full ML Analysis
- Complete dataset analysis with feature scaling
- Comprehensive statistics and visualization
- Anomaly categorization (temperature, gradient, sensor failures)
- Export to `thermal_data_with_anomalies.csv`

#### 4. MATLAB Visualization
- `thermal_realtime_plot.m`: Real-time monitoring dashboard
- `thermal_anomaly_visualization.m`: Comprehensive anomaly analysis
- Auto-path detection for cross-folder CSV file access
- 4-subplot analysis with detailed console statistics

### Data Files
- `thermal_data.csv`: Main data log (continuously written)
- `thermal_data.backup.csv`: Automatic backup
- `thermal_data_with_anomalies.csv`: ML analysis results

---

## System Architecture

```
Arduino → Serial Port → SerialWorker Thread → CSV Logger
                              ↓
                        CircularBuffer
                              ↓
                    AnomalyWorker Thread → Anomaly Alerts
                              ↓
                    ThermalDashboard (PyQt5 GUI)
                    ├── Trend Plot (PyQtGraph)
                    ├── Statistics Panel
                    ├── Data Table
                    └── Action Buttons
```

### Multi-threaded Design
- **Main Thread:** PyQt5 GUI event loop
- **SerialWorker:** Arduino communication and CSV writing (non-blocking)
- **AnomalyWorker:** Real-time ML detection on rolling buffer (non-blocking)
- **FullAnalysisWorker:** Complete dataset analysis (non-blocking)

---

## Quick Start

### Prerequisites
```bash
# Python dependencies
pip3 install PyQt5 pyqtgraph pandas numpy scikit-learn pyserial

# MATLAB (optional but recommended)
# - MATLAB R2019b or later
```

### Launch Dashboard
```bash
cd python_scada
python3 thermal_profiling_unified.py
```

### Run MATLAB Visualization
```matlab
cd matlab
thermal_realtime_plot      % Real-time monitoring
thermal_anomaly_visualization  % Anomaly analysis
```

---

## Configuration

Key parameters in `thermal_profiling_unified.py`:

```python
CONTAMINATION = 0.15          # Anomaly detection sensitivity (15%)
ANOMALY_CHECK_INTERVAL = 5    # Check every 5 samples
BUFFER_SIZE = 1000            # Rolling buffer size
UPDATE_RATE = 100             # GUI update rate (ms)
```

---

## GUI Controls

### Connection Panel
- Port selection dropdown
- Connect/Disconnect buttons
- Status indicator

### Action Buttons
- **Run Full ML Analysis:** Complete dataset anomaly detection
- **Export CSV:** Save data to custom location
- **Save Plots:** Export MATLAB visualizations
- **STOP:** Cleanly terminate all threads and exit

### Display Areas
- **Sensor Trends:** Real-time 4-sensor line plot
- **Real-time Statistics:** Max/Min/Avg/Gradient values
- **Sample Data Table:** Recent readings with timestamps

---

## Anomaly Detection Explained

### Algorithm: Isolation Forest
- Unsupervised learning approach
- Isolates anomalies rather than profiling normal data
- Effective for high-dimensional feature spaces

### Feature Engineering
- Thermal gradient (max - min)
- Max/Min ratio
- Temperature standard deviation
- Temperature range
- Sensor deltas (rate of change)

### Detection Criteria
- Contamination rate 15%: Expects ~15% of data to be observable anomalies
- Threshold: Anomaly score < -0.5
- Real-time buffer: Last 100 samples

### Anomaly Types
1. **High Temperature Events:** Max > mean + 2*std
2. **Rapid Temperature Change:** Gradient > mean + 2*std
3. **Low Temperature Events:** Min < mean - 2*std
4. **Sensor Failures:** Out-of-range or stuck readings

---

## Data Format

### thermal_data.csv
```
timestamp,elapsed_time,sensor_0,sensor_1,sensor_2,sensor_3,max_temp,min_temp,avg_temp,gradient,hotspot
2025-02-02 10:30:45.123,0.50,25.12,25.45,25.23,25.34,25.45,25.12,25.29,0.33,sensor_1
```

### thermal_data_with_anomalies.csv
Includes additional columns:
- `Anomaly`: -1 (anomaly) or 1 (normal)
- `Anomaly_Score`: Raw isolation forest score
- `Is_Anomaly`: Boolean anomaly flag

---

## MATLAB Scripts Reference

### thermal_realtime_plot.m
- Updates every 1 second
- 4 subplots: trends, bounds, gradient, statistics
- Automatic CSV path detection
- Continuous loop until window closed

### thermal_anomaly_visualization.m
- Auto-detects `thermal_data_with_anomalies.csv`
- Multi-path searching (handles nested directories)
- Console output: sample stats, sensor analysis, anomaly types
- 4-subplot visualization with anomaly highlighting

---

## Troubleshooting

### MATLAB "Invalid data type" Error
**Solution:** Convert data type explicitly
```matlab
is_anomaly = logical(data.Is_Anomaly);
```

### Arduino Not Detected
**Solution:** Check port permissions and USB connection
```bash
ls -la /dev/cu.usbmodem*
```

### No Anomalies Detected
**Possible causes:**
1. Contamination rate too low (increase from 0.05 to 0.15)
2. Normal operation - system working as designed
3. Need larger dataset (100+ samples minimum)

### MATLAB Can't Find CSV
**Solution:** MATLAB scripts auto-detect multiple paths:
- Current directory
- `../python_scada/`
- Parent directory paths
- Subdirectories

---

## Files and Directory Structure

```
Thermal_Profiling_Project/
├── python_scada/
│   ├── thermal_profiling_unified.py    (Main SCADA dashboard)
│   ├── thermal_data_logger_v2.py       (Legacy data logger)
│   ├── anomaly_detection.py            (Legacy ML module)
│   ├── requirements.txt                (Python dependencies)
│   └── thermal_data.csv                (Generated at runtime)
│
├── matlab/
│   ├── thermal_realtime_plot.m         (Real-time monitoring)
│   ├── thermal_anomaly_visualization.m (Anomaly analysis)
│   └── README.md                       (MATLAB setup guide)
│
├── firmware/
│   ├── thermal_profiling.ino           (Arduino code)
│   ├── README.md                       (Upload instructions)
│   └── circuit_diagram.png             (Hardware setup)
│
├── hardware/
│   ├── WIRING_DIAGRAM.md               (Component connections)
│   ├── SHOPPING_LINKS.md               (BOM with prices)
│   └── ASSEMBLY_GUIDE.md               (Step-by-step setup)
│
├── documentation/
│   ├── TESTING_CHECKLIST.md            (Test procedures)
│   ├── TROUBLESHOOTING.md              (Common issues)
│   └── API_REFERENCE.md                (Code documentation)
│
├── reports/
│   └── REPORT_OUTLINE.md               (Lab report template)
│
├── README.md                           (Quick start guide)
├── QUICK_START.md                      (5-minute setup)
├── PROJECT_SUMMARY.md                  (Project overview)
├── SYSTEM_OVERVIEW.md                  (This file)
├── .gitignore                          (Git configuration)
└── .git/                               (Version control)
```

---

## Performance Specifications

### Data Acquisition
- Sample rate: ~2 Hz
- Latency: <100 ms
- Accuracy: ±0.5°C

### Real-time Processing
- GUI update rate: 100 ms
- Anomaly check interval: Every 5 samples (2.5 seconds)
- ML model training time: <1 second

### System Requirements
- CPU: Multi-core processor (PyQt5 + ML threading)
- Memory: 500 MB RAM minimum
- Disk: 100 MB for logs (depending on duration)
- Python: 3.7+
- MATLAB: R2019b+ (optional)

---

## Educational Value

This project demonstrates:

1. **Embedded Systems:** Arduino programming and sensor integration
2. **Data Acquisition:** Serial communication and real-time logging
3. **GUI Development:** PyQt5 multi-threaded architecture
4. **Machine Learning:** Anomaly detection and feature engineering
5. **Signal Processing:** Thermal gradient analysis
6. **Version Control:** Git workflow and collaboration
7. **Documentation:** Technical writing and project organization

---

## Future Enhancements

- Email/SMS alerts for critical anomalies
- Data replay and playback from CSV files
- Settings dialog for ML model parameters
- Multi-channel export (JSON, HDF5)
- Cloud data logging integration
- Advanced visualization (3D thermal maps)
- Historical trend analysis

---

## Contact & Support

**Author:** Gonugondla Veera Manikanta
**Email:** mgonugondlamanikanta@gmail.com
**GitHub:** github.com/Manikanta25055
**Portfolio:** helicanexplanations.wordpress.com

---

## License

This project is provided as-is for educational purposes in the Measurements & Instrumentation Laboratory course at Manipal Institute of Technology.

---

## References

- Arduino Documentation: https://www.arduino.cc/reference/
- PyQt5 Documentation: https://www.riverbankcomputing.com/static/Docs/PyQt5/
- Scikit-learn Isolation Forest: https://scikit-learn.org/stable/modules/generated/sklearn.ensemble.IsolationForest.html
- MATLAB Documentation: https://www.mathworks.com/help/matlab/
- DS18B20 Datasheet: https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf

---

**Last Updated:** February 2, 2025
