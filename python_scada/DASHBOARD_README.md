# Thermal Profiling Dashboard - PyQt5 GUI

Professional real-time data acquisition and analysis dashboard for thermal profiling system with ML-based anomaly detection.

## Features

### Real-time Data Visualization
- Live sensor temperature trends (4 sensors, color-coded)
- Thermal heatmap display (2x2 sensor grid)
- Real-time statistical analysis (Max, Min, Avg, Gradient)
- Auto-scrolling 60-second trend view

### Multi-threaded Architecture
- **Serial Worker Thread**: Non-blocking USB data acquisition from Arduino
- **Anomaly Worker Thread**: Real-time ML anomaly detection (Isolation Forest)
- **Main GUI Thread**: Responsive UI updates at 100ms intervals

### Machine Learning Integration
- Isolation Forest anomaly detection (5% contamination rate)
- Automatic model training on first 100 samples
- Real-time anomaly scoring and alerts
- Anomaly detection within 10-20 seconds of occurrence

### Data Management
- Automatic CSV logging with timestamps
- Session-based data storage
- Export functionality
- Full analysis report generation

### Professional Interface
- Modern PyQt5 design with responsive layout
- Color-coded status indicators
- Real-time statistics display (LCD numbers)
- Connection status monitoring
- Error handling with user-friendly messages

## Installation

### Prerequisites
```bash
pip3 install -r requirements.txt
```

### Required Packages
- PyQt5>=5.15.0 (GUI framework)
- pyqtgraph>=0.12.0 (High-performance plotting)
- numpy>=1.21.0 (Numerical computing)
- pandas>=1.3.0 (Data management)
- scikit-learn>=1.0.0 (Machine learning)
- pyserial>=3.5 (Serial communication)

## Usage

### Launch Dashboard
```bash
cd python_scada
python3 thermal_dashboard_gui.py
```

### Connection Process
1. Select serial port from dropdown (auto-detected)
2. Click "Connect" button
3. Dashboard becomes active when connected
4. Real-time data starts flowing

### During Data Acquisition
- Monitor live sensor values in trend plot
- Watch anomaly scores update in real-time
- Track statistics (Max/Min/Avg/Gradient)
- Session duration and sample rate displayed
- Anomaly count and percentage updated automatically

### After Data Collection
1. Click "Disconnect" to stop acquisition
2. Click "Export CSV" to save session data
3. Click "Run Full ML Analysis" for comprehensive report
4. Click "Save Plots" to export visualizations

## Architecture

### Data Flow
```
Arduino → Serial Port → SerialWorker Thread → CircularBuffer → GUI Updates
                            ↓
                        CSV Logger
                            ↓
                    AnomalyWorker Thread → ML Model → Alerts
```

### Key Components

#### SerialWorker (QThread)
- Connects to Arduino via USB serial
- Parses thermal data blocks
- Writes data to CSV file
- Auto-reconnect on disconnect (max 5 attempts)
- Emits signals: `dataReceived`, `statusChanged`, `errorOccurred`

#### AnomalyWorker (QThread)
- Maintains 100-sample rolling buffer
- Trains Isolation Forest model on first 100 samples
- Runs prediction every 10 samples
- Detects anomalies based on ML score
- Emits signals: `anomalyDetected`, `anomalyScoreUpdated`, `modelReady`

#### ThermalDashboard (QMainWindow)
- Central UI component
- Manages all widgets and layouts
- Connects signals/slots
- Handles user interactions
- Updates displays in real-time

### Configuration
```python
WINDOW_SIZE = (1400, 900)              # Dashboard dimensions
UPDATE_RATE = 100                      # ms between GUI updates
ANOMALY_CHECK_INTERVAL = 10            # samples between ML predictions
BUFFER_SIZE = 1000                     # maximum stored samples
CONTAMINATION = 0.05                   # Expected anomaly rate (5%)
```

## Widget Layout

```
┌─────────────────────────────────────────────────────────┐
│  Connection Panel: [Port ▼] [Connect] [Disconnect]      │
├─────────────────────────────────────────────────────────┤
│  ┌──────────────────────┐  ┌──────────────────────────┐ │
│  │  Thermal Heatmap     │  │  Sensor Trends (60s)     │ │
│  │  2x2 Grid Display    │  │  4 color-coded curves    │ │
│  └──────────────────────┘  └──────────────────────────┘ │
│  ┌──────────────────────┐  ┌──────────────────────────┐ │
│  │  Thermal Analysis    │  │  Anomaly Detection       │ │
│  │  Max/Min/Avg/Grad    │  │  Status & Score Plot     │ │
│  └──────────────────────┘  └──────────────────────────┘ │
├─────────────────────────────────────────────────────────┤
│  Status: Samples:1234 | Duration:00:05:23 | Rate:1.2Hz  │
│  Anomalies: 12 (5.2%) | Status: Acquiring data...       │
├─────────────────────────────────────────────────────────┤
│  [Export CSV] [Run Full ML] [Save Plots]                │
└─────────────────────────────────────────────────────────┘
```

## Data Format

### CSV Output
```
Timestamp,Elapsed_Time,Sensor_0,Sensor_1,Sensor_2,Sensor_3,Max_Temp,Min_Temp,Avg_Temp,Gradient,Hotspot
2025-02-02 15:30:45,0.00,25.12,25.45,25.78,26.01,26.01,25.12,25.59,0.89,3
2025-02-02 15:30:46,1.05,25.15,25.48,25.81,26.04,26.04,25.15,25.62,0.89,3
...
```

## Anomaly Detection

### ML Model
- **Algorithm**: Isolation Forest
- **Features**: Sensor values, gradient, max/min temps
- **Training**: 100 samples (automatic)
- **Retraining**: Every 500 samples (optional)
- **Contamination**: 5% expected anomalies

### Detection Criteria
- Isolation Forest anomaly score < -0.5
- Triggered for thermal runaway events
- High sensor gradient spikes
- Sudden temperature changes
- Out-of-range readings

### Anomaly Response
1. Anomaly score displayed in real-time plot
2. Alert message shown in GUI
3. Count incremented in status panel
4. Details logged to CSV

## Performance

### Target Specifications
- **Update Rate**: 1-2 Hz (depending on Arduino)
- **GUI Responsiveness**: No freezing or lag
- **ML Detection Latency**: 20-30 samples (~20 seconds)
- **Memory Usage**: <200 MB (typical)
- **CPU Usage**: <5% (idle), <15% (active)

### Optimization
- Circular buffers for memory efficiency
- PyQtGraph for GPU-accelerated rendering
- Threading to prevent GUI blocking
- Efficient CSV writing with buffering

## Troubleshooting

### Connection Issues
- Verify Arduino is connected via USB
- Check port selection in dropdown
- Ensure correct Arduino firmware is running
- Look for error messages in status bar

### No Data Appearing
- Wait 2-3 seconds after connect (Arduino reset delay)
- Verify Arduino is sending data (test with serial monitor)
- Check Arduino code for correct format

### GUI Freezing
- Ensure both worker threads started successfully
- Check console for error messages
- Verify dependencies installed correctly

### ML Model Not Training
- Need at least 100 samples before model trains
- Check that sensor values are numeric
- Model ready signal should appear in status

## Advanced Usage

### Custom Configuration
Edit `thermal_dashboard_gui.py` constants:
```python
ANOMALY_CHECK_INTERVAL = 10  # Change detection frequency
CONTAMINATION = 0.05         # Change anomaly threshold
BUFFER_SIZE = 1000          # Change data retention
```

### Data Analysis
Exported CSV can be analyzed with:
- pandas: `pd.read_csv('thermal_data.csv')`
- matplotlib: Plot temperature trends
- scikit-learn: Run custom ML models

### Integration
Dashboard can be extended with:
- Email/sound alerts
- Database logging
- Remote monitoring
- Multi-device support

## File Structure
```
python_scada/
├── thermal_dashboard_gui.py      (Main dashboard - THIS FILE)
├── thermal_data_logger_v2.py     (Data logger - reused by dashboard)
├── anomaly_detection.py          (ML model - reused by dashboard)
├── requirements.txt              (Python dependencies)
└── thermal_data_*.csv            (Generated session files)
```

## Performance Benchmarks

### Tested Hardware
- macOS with Python 3.9+
- PyQt5 5.15+
- Arduino R4 WiFi @ 115200 baud

### Results
- Initial load: <1 second
- First model train (100 samples): ~2 seconds
- Steady-state UI update: <100ms
- CSV write latency: <10ms

## Known Limitations

1. Single Arduino connection only (current version)
2. Heatmap requires manual temperature range adjustment
3. CSV export limited to current session data
4. No real-time plotting beyond 500 samples (configurable)
5. Anomaly detection requires 100 samples minimum

## Future Enhancements

- Multi-Arduino support
- Historical data replay
- Email/webhook alerts
- Advanced anomaly categorization
- Custom threshold configuration UI
- Data encryption and security
- Cloud data synchronization

## Credits
- Author: Manikanta (230906450)
- Course: M&I Lab (ELE 3162)
- Institution: Manipal Institute of Technology & IITM

## License
Academic use only
