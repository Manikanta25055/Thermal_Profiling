#!/usr/bin/env python3
"""
Thermal Profiling System - Unified Dashboard
Combines Data Logger + Real-time GUI + ML Anomaly Detection

Author: Manikanta (230906450)
Course: M&I Lab (ELE 3162)
Date: Feb 2025
"""

import sys
import serial
import time
from datetime import datetime
from collections import deque
import re
import os
import csv
import glob
import shutil

import numpy as np
import pandas as pd
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QGroupBox, QPushButton, QComboBox, QLabel, QSpinBox, QCheckBox,
    QToolBar, QStatusBar, QMessageBox, QFileDialog, QGridLayout,
    QLCDNumber, QTableWidget, QTableWidgetItem, QHeaderView, QSplitter
)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer, QRect
from PyQt5.QtGui import QFont, QColor, QIcon

import pyqtgraph as pg
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler
import matplotlib
matplotlib.use('Agg')  # Use non-GUI backend
import matplotlib.pyplot as plt
import seaborn as sns
import warnings

warnings.filterwarnings('ignore')

# ============================================================================
# CONFIGURATION
# ============================================================================
WINDOW_TITLE = "Thermal Profiling System - Unified Dashboard"
WINDOW_SIZE = (1600, 1000)
UPDATE_RATE = 100  # ms
ANOMALY_CHECK_INTERVAL = 5  # samples
BUFFER_SIZE = 1000
CONTAMINATION = 0.15
CSV_FILENAME = "thermal_data.csv"
BACKUP_FILENAME = "thermal_data.backup.csv"
ANOMALY_CSV_FILENAME = "thermal_data_with_anomalies.csv"

# ============================================================================
# CIRCULAR BUFFER
# ============================================================================
class CircularBuffer:
    def __init__(self, max_size=BUFFER_SIZE):
        self.max_size = max_size
        self.data = deque(maxlen=max_size)

    def append(self, item):
        self.data.append(item)

    def get_last_n(self, n):
        if len(self.data) < n:
            return list(self.data)
        return list(deque(list(self.data)[-n:]))

    def get_all(self):
        return list(self.data)

    def __len__(self):
        return len(self.data)

# ============================================================================
# SERIAL WORKER THREAD
# ============================================================================
class SerialWorker(QThread):
    dataReceived = pyqtSignal(dict)
    statusChanged = pyqtSignal(str)
    errorOccurred = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.is_running = False
        self.ser = None
        self.csv_file = None
        self.csv_writer = None
        self.reconnect_count = 0
        self.max_reconnect_attempts = 5

    def run(self):
        self.is_running = True
        self.reconnect_count = 0

        while self.is_running and self.reconnect_count < self.max_reconnect_attempts:
            try:
                if not self.ser or not self.ser.is_open:
                    self.connect_to_arduino()
                self.acquire_data()

            except Exception as e:
                self.errorOccurred.emit(f"Serial error: {str(e)}")
                self.statusChanged.emit("Disconnected")
                self.reconnect_count += 1
                time.sleep(2)

        self.cleanup()

    def connect_to_arduino(self):
        if self.ser:
            self.ser.close()

        self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
        time.sleep(2)
        self.ser.reset_input_buffer()
        self.statusChanged.emit(f"Connected to {self.port}")
        self.reconnect_count = 0

        # Backup previous thermal_data.csv to thermal_data.backup.csv
        if os.path.exists(CSV_FILENAME):
            shutil.copy2(CSV_FILENAME, BACKUP_FILENAME)
            print(f"Previous data backed up to: {BACKUP_FILENAME}")

        # Create new thermal_data.csv
        self.csv_file = open(CSV_FILENAME, 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        self.csv_writer.writerow([
            "Timestamp", "Elapsed_Time", "Sensor_0", "Sensor_1",
            "Sensor_2", "Sensor_3", "Max_Temp", "Min_Temp", "Avg_Temp", "Gradient", "Hotspot"
        ])
        self.csv_file.flush()
        print(f"Created new CSV file: {CSV_FILENAME}")

    def acquire_data(self):
        in_thermal_block = False
        thermal_data = {}
        start_time = time.time()

        while self.is_running:
            try:
                line = self.ser.readline().decode('utf-8', errors='ignore').strip()

                if not line:
                    continue

                if "Sensor 0:" in line:
                    in_thermal_block = True
                    thermal_data = {}
                    match = re.search(r'([\d.]+)°C', line)
                    if match:
                        thermal_data['sensor_0'] = float(match.group(1))

                elif in_thermal_block:
                    if "Sensor 1:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['sensor_1'] = float(match.group(1))

                    elif "Sensor 2:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['sensor_2'] = float(match.group(1))

                    elif "Sensor 3:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['sensor_3'] = float(match.group(1))

                    elif "Max Temperature:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['max_temp'] = float(match.group(1))

                    elif "Min Temperature:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['min_temp'] = float(match.group(1))

                    elif "Avg Temperature:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['avg_temp'] = float(match.group(1))

                    elif "Thermal Gradient:" in line:
                        match = re.search(r'([\d.]+)°C', line)
                        if match:
                            thermal_data['gradient'] = float(match.group(1))

                    elif "Hotspot Location:" in line:
                        match = re.search(r'Sensor (\d+)', line)
                        if match:
                            thermal_data['hotspot'] = int(match.group(1))

                    elif "==================================================" in line:
                        if len(thermal_data) == 9:
                            elapsed = time.time() - start_time
                            ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                            csv_row = [
                                ts, f"{elapsed:.2f}",
                                f"{thermal_data.get('sensor_0', 0):.2f}",
                                f"{thermal_data.get('sensor_1', 0):.2f}",
                                f"{thermal_data.get('sensor_2', 0):.2f}",
                                f"{thermal_data.get('sensor_3', 0):.2f}",
                                f"{thermal_data.get('max_temp', 0):.2f}",
                                f"{thermal_data.get('min_temp', 0):.2f}",
                                f"{thermal_data.get('avg_temp', 0):.2f}",
                                f"{thermal_data.get('gradient', 0):.2f}",
                                f"{thermal_data.get('hotspot', 0)}"
                            ]
                            self.csv_writer.writerow(csv_row)
                            self.csv_file.flush()

                            thermal_data['elapsed_time'] = elapsed
                            thermal_data['timestamp'] = ts

                            self.dataReceived.emit(thermal_data)

                        in_thermal_block = False
                        thermal_data = {}

            except Exception as e:
                self.errorOccurred.emit(f"Data parsing error: {str(e)}")
                raise

    def stop(self):
        self.is_running = False

    def cleanup(self):
        if self.csv_file:
            self.csv_file.close()
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.statusChanged.emit("Disconnected")

# ============================================================================
# ANOMALY DETECTION WORKER - REAL-TIME
# ============================================================================
class AnomalyWorker(QThread):
    anomalyDetected = pyqtSignal(dict)
    anomalyScoreUpdated = pyqtSignal(float)
    modelReady = pyqtSignal(bool)

    def __init__(self):
        super().__init__()
        self.is_running = False
        self.buffer = CircularBuffer(max_size=100)
        self.model = None
        self.scaler = None
        self.sample_count = 0
        self.anomaly_check_interval = ANOMALY_CHECK_INTERVAL
        self.all_data = []

    def add_data(self, thermal_data):
        feature_vector = {
            'timestamp': thermal_data.get('timestamp'),
            'elapsed_time': thermal_data.get('elapsed_time', 0),
            'sensor_0': thermal_data.get('sensor_0', 0),
            'sensor_1': thermal_data.get('sensor_1', 0),
            'sensor_2': thermal_data.get('sensor_2', 0),
            'sensor_3': thermal_data.get('sensor_3', 0),
            'gradient': thermal_data.get('gradient', 0),
            'max_temp': thermal_data.get('max_temp', 0),
            'min_temp': thermal_data.get('min_temp', 0),
            'avg_temp': thermal_data.get('avg_temp', 0),
            'hotspot': thermal_data.get('hotspot', 0),
        }
        self.all_data.append(feature_vector)
        
        buf_vector = {
            'sensor_0': thermal_data.get('sensor_0', 0),
            'sensor_1': thermal_data.get('sensor_1', 0),
            'sensor_2': thermal_data.get('sensor_2', 0),
            'sensor_3': thermal_data.get('sensor_3', 0),
            'gradient': thermal_data.get('gradient', 0),
            'max_temp': thermal_data.get('max_temp', 0),
            'min_temp': thermal_data.get('min_temp', 0),
        }
        self.buffer.append(buf_vector)
        self.sample_count += 1

        if self.sample_count == 50 and self.model is None:
            self.train_model()

        if self.sample_count > 50 and self.sample_count % self.anomaly_check_interval == 0:
            self.detect_anomalies(thermal_data)

    def train_model(self):
        try:
            if len(self.buffer) < 30:
                return

            buffer_data = self.buffer.get_all()
            X = np.array([
                [
                    d['sensor_0'], d['sensor_1'], d['sensor_2'], d['sensor_3'],
                    d['gradient'], d['max_temp'], d['min_temp']
                ]
                for d in buffer_data
            ])

            self.scaler = StandardScaler()
            X_scaled = self.scaler.fit_transform(X)

            self.model = IsolationForest(
                contamination=CONTAMINATION,
                random_state=42,
                n_estimators=100
            )
            self.model.fit(X_scaled)
            self.modelReady.emit(True)
            print("Real-time ML Model trained!")

        except Exception as e:
            print(f"Model training error: {e}")

    def detect_anomalies(self, thermal_data):
        if self.model is None or self.scaler is None:
            return

        try:
            buffer_data = self.buffer.get_all()
            if len(buffer_data) < 5:
                return

            X = np.array([
                [
                    d['sensor_0'], d['sensor_1'], d['sensor_2'], d['sensor_3'],
                    d['gradient'], d['max_temp'], d['min_temp']
                ]
                for d in buffer_data
            ])

            X_scaled = self.scaler.transform(X)
            predictions = self.model.predict(X_scaled)
            scores = self.model.score_samples(X_scaled)

            latest_score = scores[-1]
            self.anomalyScoreUpdated.emit(latest_score)

            if predictions[-1] == -1:
                print(f"REAL-TIME ANOMALY DETECTED! Score: {latest_score}")
                anomaly_info = {
                    'score': latest_score,
                    'timestamp': thermal_data.get('timestamp'),
                    'sensor_0': thermal_data.get('sensor_0'),
                    'sensor_1': thermal_data.get('sensor_1'),
                    'sensor_2': thermal_data.get('sensor_2'),
                    'sensor_3': thermal_data.get('sensor_3'),
                    'max_temp': thermal_data.get('max_temp'),
                    'gradient': thermal_data.get('gradient'),
                }
                self.anomalyDetected.emit(anomaly_info)

        except Exception as e:
            print(f"Anomaly detection error: {e}")

    def run(self):
        self.is_running = True
        while self.is_running:
            time.sleep(0.1)

    def stop(self):
        self.is_running = False

# ============================================================================
# FULL ANALYSIS WORKER - COMPLETE anomaly_detection.py IMPLEMENTATION
# ============================================================================
class FullAnalysisWorker(QThread):
    analysisComplete = pyqtSignal(pd.DataFrame, str)
    analysisError = pyqtSignal(str)

    def __init__(self, data_list):
        super().__init__()
        self.data_list = data_list

    def run(self):
        try:
            if len(self.data_list) < 50:
                self.analysisError.emit("Need at least 50 samples for full analysis")
                return

            df = pd.DataFrame(self.data_list)

            print("\n" + "="*70)
            print("THERMAL PROFILING SYSTEM - FULL ML ANOMALY ANALYSIS")
            print("Module 5: Isolation Forest Algorithm")
            print("="*70 + "\n")

            # ===== FEATURE ENGINEERING =====
            print("Preparing features for anomaly detection...\n")

            sensor_cols = ['sensor_0', 'sensor_1', 'sensor_2', 'sensor_3']
            analysis_cols = ['max_temp', 'min_temp', 'avg_temp', 'gradient']

            X = df[sensor_cols].copy()

            X['Gradient'] = df['gradient']
            X['Max_Min_Ratio'] = df['max_temp'] / (df['min_temp'] + 0.1)
            X['Temp_Std'] = X[sensor_cols].std(axis=1)
            X['Temp_Range'] = df['max_temp'] - df['min_temp']

            for col in sensor_cols:
                X[f'{col}_Delta'] = df[col].diff().fillna(0)

            print(f"Features created: {X.shape[1]}")
            print(f"Feature columns: {list(X.columns)}\n")

            # ===== STANDARDIZE DATA =====
            print("Standardizing features...\n")

            scaler = StandardScaler()
            X_scaled = scaler.fit_transform(X)

            # ===== TRAIN ISOLATION FOREST =====
            print("Training Isolation Forest model...\n")

            iso_forest = IsolationForest(
                contamination=CONTAMINATION,
                random_state=42,
                n_estimators=100
            )

            predictions = iso_forest.fit_predict(X_scaled)
            anomaly_scores = iso_forest.score_samples(X_scaled)

            df['Anomaly'] = predictions
            df['Anomaly_Score'] = anomaly_scores
            df['Is_Anomaly'] = (predictions == -1)

            # ===== ANALYZE RESULTS =====
            print("="*70)
            print("ANOMALY DETECTION RESULTS")
            print("="*70 + "\n")

            num_anomalies = (df['Is_Anomaly']).sum()
            anomaly_pct = 100 * num_anomalies / len(df)

            result_msg = f"""
ANALYSIS COMPLETE
=================
Total samples: {len(df)}
Normal samples: {len(df) - num_anomalies}
Anomalies detected: {num_anomalies} ({anomaly_pct:.1f}%)

SENSOR STATISTICS (Normal Conditions):
"""

            for col in sensor_cols:
                normal_data = df[~df['Is_Anomaly']]
                result_msg += f"\n  {col}: {normal_data[col].mean():.2f}±{normal_data[col].std():.2f}°C"

            print(result_msg)

            # ===== CREATE VISUALIZATIONS =====
            print("\n" + "="*70)
            print("Creating visualizations...")
            print("="*70 + "\n")

            fig, axes = plt.subplots(2, 2, figsize=(14, 10))

            normal_idx = df[~df['Is_Anomaly']].index
            anomaly_idx = df[df['Is_Anomaly']].index

            axes[0, 0].scatter(df.loc[normal_idx, 'elapsed_time'], 
                              df.loc[normal_idx, 'Anomaly_Score'],
                              color='green', label='Normal', alpha=0.6, s=30)
            axes[0, 0].scatter(df.loc[anomaly_idx, 'elapsed_time'], 
                              df.loc[anomaly_idx, 'Anomaly_Score'],
                              color='red', label='Anomaly', alpha=0.8, s=50, marker='x')
            axes[0, 0].axhline(y=-0.5, color='orange', linestyle='--', linewidth=2, label='Threshold')
            axes[0, 0].set_xlabel('Elapsed Time (s)', fontsize=11)
            axes[0, 0].set_ylabel('Anomaly Score', fontsize=11)
            axes[0, 0].set_title('Isolation Forest Anomaly Scores', fontsize=12, fontweight='bold')
            axes[0, 0].legend()
            axes[0, 0].grid(True, alpha=0.3)

            for i, col in enumerate(sensor_cols):
                axes[0, 1].plot(df['elapsed_time'], df[col], label=col, alpha=0.7, linewidth=1.5)

            for idx in anomaly_idx:
                axes[0, 1].axvline(x=df.loc[idx, 'elapsed_time'], color='red', alpha=0.3, linestyle='--', linewidth=1)

            axes[0, 1].set_xlabel('Elapsed Time (s)', fontsize=11)
            axes[0, 1].set_ylabel('Temperature (°C)', fontsize=11)
            axes[0, 1].set_title('Sensor Readings with Anomalies', fontsize=12, fontweight='bold')
            axes[0, 1].legend()
            axes[0, 1].grid(True, alpha=0.3)

            axes[1, 0].plot(df['elapsed_time'], df['gradient'], color='blue', alpha=0.7, linewidth=1.5, label='Gradient')
            axes[1, 0].scatter(df.loc[anomaly_idx, 'elapsed_time'], 
                              df.loc[anomaly_idx, 'gradient'],
                              color='red', s=50, marker='x', label='Anomaly')
            axes[1, 0].set_xlabel('Elapsed Time (s)', fontsize=11)
            axes[1, 0].set_ylabel('Temperature Difference (°C)', fontsize=11)
            axes[1, 0].set_title('Thermal Gradient with Anomalies', fontsize=12, fontweight='bold')
            axes[1, 0].legend()
            axes[1, 0].grid(True, alpha=0.3)

            sizes = [len(df) - num_anomalies, num_anomalies]
            colors = ['green', 'red']
            labels = [f'Normal ({len(df) - num_anomalies})', f'Anomaly ({num_anomalies})']
            axes[1, 1].pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
            axes[1, 1].set_title('Sample Distribution', fontsize=12, fontweight='bold')

            plt.tight_layout()
            plt.savefig('anomaly_detection_full_analysis.png', dpi=150, bbox_inches='tight')
            print("Visualization saved as: anomaly_detection_full_analysis.png\n")

            # Save anomaly data to separate CSV
            df.to_csv(ANOMALY_CSV_FILENAME, index=False)
            print(f"Anomaly analysis data saved to: {ANOMALY_CSV_FILENAME}\n")

            self.analysisComplete.emit(df, result_msg)

        except Exception as e:
            self.analysisError.emit(f"Analysis error: {str(e)}")
            import traceback
            traceback.print_exc()

# ============================================================================
# MAIN DASHBOARD
# ============================================================================
class ThermalDashboard(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle(WINDOW_TITLE)
        self.setGeometry(50, 50, WINDOW_SIZE[0], WINDOW_SIZE[1])

        self.serial_worker = None
        self.anomaly_worker = AnomalyWorker()
        self.analysis_worker = None

        self.time_series = deque(maxlen=500)
        self.sensor_data = {0: deque(maxlen=500), 1: deque(maxlen=500),
                          2: deque(maxlen=500), 3: deque(maxlen=500)}
        self.anomaly_count = 0
        self.sample_count = 0
        self.session_start = None

        self.setup_ui()
        self.apply_style()
        self.statusBar().showMessage("Ready to connect")

    def setup_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(5, 5, 5, 5)

        self.setup_connection_panel(main_layout)

        splitter = QSplitter(Qt.Vertical)

        # ===== GRAPHS AREA (TOP) - LARGER SENSOR TRENDS =====
        graphs_layout = QGridLayout()
        graphs_layout.setSpacing(10)

        # Large sensor trends plot (takes up 2 rows on left)
        self.trend_plot = pg.PlotWidget(title="Sensor Trends")
        self.trend_plot.setLabel('left', 'Temperature (°C)')
        self.trend_plot.setLabel('bottom', 'Time (s)')
        self.trend_plot.addLegend()
        self.trend_plot.showGrid(True, True)
        graphs_layout.addWidget(self.trend_plot, 0, 0, 2, 1)

        # Real-time stats on right (larger font)
        self.stats_group = QGroupBox("Real-time Statistics")
        stats_layout = QVBoxLayout()
        
        self.label_max = QLabel("Max: --")
        self.label_min = QLabel("Min: --")
        self.label_avg = QLabel("Avg: --")
        self.label_gradient = QLabel("Gradient: --")
        self.label_anomaly_status = QLabel("Anomalies: 0")
        
        font = QFont()
        font.setPointSize(13)
        font.setBold(True)
        for label in [self.label_max, self.label_min, self.label_avg, self.label_gradient, self.label_anomaly_status]:
            label.setFont(font)
        
        stats_layout.addWidget(self.label_max)
        stats_layout.addWidget(self.label_min)
        stats_layout.addWidget(self.label_avg)
        stats_layout.addWidget(self.label_gradient)
        stats_layout.addWidget(self.label_anomaly_status)
        stats_layout.addStretch()
        
        self.stats_group.setLayout(stats_layout)
        graphs_layout.addWidget(self.stats_group, 0, 1, 2, 1)

        graphs_widget = QWidget()
        graphs_widget.setLayout(graphs_layout)
        splitter.addWidget(graphs_widget)

        # ===== DATA TABLE (BOTTOM) =====
        table_group = QGroupBox("Sample-wise Data Log")
        table_layout = QVBoxLayout()

        self.data_table = QTableWidget()
        self.data_table.setColumnCount(11)
        self.data_table.setHorizontalHeaderLabels([
            "Timestamp", "Elapsed (s)", "S0 (°C)", "S1 (°C)", "S2 (°C)", "S3 (°C)",
            "Max (°C)", "Min (°C)", "Avg (°C)", "Gradient (°C)", "Hotspot"
        ])
        self.data_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.data_table.setMaximumHeight(200)

        table_layout.addWidget(self.data_table)
        table_group.setLayout(table_layout)
        splitter.addWidget(table_group)

        splitter.setStretchFactor(0, 3)
        splitter.setStretchFactor(1, 1)

        main_layout.addWidget(splitter, 1)

        self.setup_status_panel(main_layout)
        self.setup_action_buttons(main_layout)

    def setup_connection_panel(self, parent_layout):
        panel = QGroupBox("Connection Panel")
        layout = QHBoxLayout()

        layout.addWidget(QLabel("Serial Port:"))
        self.port_combo = QComboBox()
        self.scan_ports()
        layout.addWidget(self.port_combo)

        self.connect_btn = QPushButton("Connect")
        self.connect_btn.clicked.connect(self.connect_to_arduino)
        layout.addWidget(self.connect_btn)

        self.disconnect_btn = QPushButton("Disconnect")
        self.disconnect_btn.clicked.connect(self.disconnect_from_arduino)
        self.disconnect_btn.setEnabled(False)
        layout.addWidget(self.disconnect_btn)

        layout.addStretch()
        panel.setLayout(layout)
        parent_layout.addWidget(panel)

    def setup_status_panel(self, parent_layout):
        panel = QGroupBox("Status Panel")
        layout = QHBoxLayout()

        self.status_samples = QLabel("Samples: 0")
        self.status_duration = QLabel("Duration: 00:00:00")
        self.status_rate = QLabel("Rate: 0.0 Hz")
        self.status_anomalies = QLabel("Anomalies: 0 (0.0%)")

        layout.addWidget(self.status_samples)
        layout.addWidget(self.status_duration)
        layout.addWidget(self.status_rate)
        layout.addWidget(self.status_anomalies)
        layout.addStretch()

        panel.setLayout(layout)
        parent_layout.addWidget(panel)

    def setup_action_buttons(self, parent_layout):
        layout = QHBoxLayout()

        self.run_analysis_btn = QPushButton("Run Full ML Analysis")
        self.run_analysis_btn.clicked.connect(self.run_full_analysis)
        layout.addWidget(self.run_analysis_btn)

        self.export_btn = QPushButton("Export CSV")
        self.export_btn.clicked.connect(self.export_csv)
        layout.addWidget(self.export_btn)

        self.save_plots_btn = QPushButton("Save Plots")
        self.save_plots_btn.clicked.connect(self.save_plots)
        layout.addWidget(self.save_plots_btn)

        layout.addStretch()

        self.stop_btn = QPushButton("STOP")
        self.stop_btn.setStyleSheet("""
            QPushButton {
                background-color: #C62828;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 4px;
                font-weight: bold;
                font-size: 11px;
            }
            QPushButton:hover {
                background-color: #B71C1C;
            }
            QPushButton:pressed {
                background-color: #A71010;
            }
        """)
        self.stop_btn.clicked.connect(self.stop_application)
        layout.addWidget(self.stop_btn)

        parent_layout.addLayout(layout)

    def apply_style(self):
        self.setStyleSheet("""
            QMainWindow {
                background-color: #ECEFF1;
            }
            QGroupBox {
                color: #1A237E;
                border: 3px solid #1A237E;
                border-radius: 6px;
                margin-top: 12px;
                padding-top: 10px;
                font-weight: bold;
                font-size: 12px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 2px 6px;
            }
            QLabel {
                color: #1A237E;
                font-weight: 500;
            }
            QPushButton {
                background-color: #1565C0;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 4px;
                font-weight: bold;
                font-size: 11px;
            }
            QPushButton:hover {
                background-color: #0D47A1;
            }
            QPushButton:pressed {
                background-color: #0A3D91;
            }
            QPushButton:disabled {
                background-color: #B0BEC5;
                color: #FFFFFF;
            }
            QComboBox {
                background-color: #FFFFFF;
                color: #1A237E;
                border: 2px solid #1A237E;
                padding: 5px;
                border-radius: 3px;
                font-weight: 500;
            }
            QComboBox QAbstractItemView {
                background-color: #FFFFFF;
                color: #1A237E;
                selection-background-color: #1565C0;
            }
            QTableWidget {
                background-color: #FFFFFF;
                color: #1A237E;
                border: 1px solid #1A237E;
            }
            QHeaderView::section {
                background-color: #1565C0;
                color: white;
                padding: 5px;
                border: none;
                font-weight: bold;
            }
            QStatusBar {
                background-color: #ECEFF1;
                color: #1A237E;
            }
        """)

    def scan_ports(self):
        try:
            import serial.tools.list_ports
            ports = [port.device for port in serial.tools.list_ports.comports()]
            self.port_combo.addItems(ports if ports else ["/dev/cu.usbmodem*"])
        except:
            self.port_combo.addItem("/dev/cu.usbmodem*")

    def connect_to_arduino(self):
        port = self.port_combo.currentText()

        if not port:
            QMessageBox.warning(self, "Error", "Please select a serial port")
            return

        self.serial_worker = SerialWorker(port)
        self.serial_worker.dataReceived.connect(self.on_data_received)
        self.serial_worker.statusChanged.connect(self.on_status_changed)
        self.serial_worker.errorOccurred.connect(self.on_error)

        self.anomaly_worker = AnomalyWorker()
        self.anomaly_worker.anomalyDetected.connect(self.on_anomaly_detected)

        self.anomaly_worker.start()
        self.serial_worker.start()

        self.connect_btn.setEnabled(False)
        self.disconnect_btn.setEnabled(True)
        self.port_combo.setEnabled(False)

        self.session_start = time.time()

    def disconnect_from_arduino(self):
        if self.serial_worker:
            self.serial_worker.stop()
            self.serial_worker.wait()

        if self.anomaly_worker:
            self.anomaly_worker.stop()
            self.anomaly_worker.wait()

        self.connect_btn.setEnabled(True)
        self.disconnect_btn.setEnabled(False)
        self.port_combo.setEnabled(True)
        self.statusBar().showMessage("Disconnected")

    def on_data_received(self, thermal_data):
        self.sample_count += 1

        elapsed = thermal_data.get('elapsed_time', 0)
        self.time_series.append(elapsed)

        for i in range(4):
            key = f'sensor_{i}'
            self.sensor_data[i].append(thermal_data.get(key, 0))

        max_temp = thermal_data.get('max_temp', 0)
        min_temp = thermal_data.get('min_temp', 0)
        avg_temp = thermal_data.get('avg_temp', 0)
        gradient = thermal_data.get('gradient', 0)

        self.label_max.setText(f"Max: {max_temp:.2f}°C")
        self.label_min.setText(f"Min: {min_temp:.2f}°C")
        self.label_avg.setText(f"Avg: {avg_temp:.2f}°C")
        self.label_gradient.setText(f"Gradient: {gradient:.2f}°C")

        self.add_table_row(thermal_data)
        self.update_trend_plot()

        self.anomaly_worker.add_data(thermal_data)
        self.update_status()

    def add_table_row(self, thermal_data):
        row_position = self.data_table.rowCount()
        self.data_table.insertRow(row_position)

        row_data = [
            thermal_data.get('timestamp', ''),
            f"{thermal_data.get('elapsed_time', 0):.2f}",
            f"{thermal_data.get('sensor_0', 0):.2f}",
            f"{thermal_data.get('sensor_1', 0):.2f}",
            f"{thermal_data.get('sensor_2', 0):.2f}",
            f"{thermal_data.get('sensor_3', 0):.2f}",
            f"{thermal_data.get('max_temp', 0):.2f}",
            f"{thermal_data.get('min_temp', 0):.2f}",
            f"{thermal_data.get('avg_temp', 0):.2f}",
            f"{thermal_data.get('gradient', 0):.2f}",
            str(thermal_data.get('hotspot', 0)),
        ]

        for col, value in enumerate(row_data):
            self.data_table.setItem(row_position, col, QTableWidgetItem(value))

        if row_position > 100:
            self.data_table.removeRow(0)
        else:
            self.data_table.scrollToBottom()

    def on_anomaly_detected(self, anomaly_info):
        self.anomaly_count += 1
        self.label_anomaly_status.setText(f"Anomalies: {self.anomaly_count}")
        print(f"Real-time Anomaly #{self.anomaly_count}: {anomaly_info}")

    def on_status_changed(self, status):
        self.statusBar().showMessage(status)

    def on_error(self, error):
        QMessageBox.critical(self, "Error", error)

    def update_trend_plot(self):
        self.trend_plot.clear()

        if len(self.time_series) == 0:
            return

        colors = ['#FF5252', '#4CAF50', '#2196F3', '#FFC107']
        for i in range(4):
            if len(self.sensor_data[i]) > 0:
                self.trend_plot.plot(
                    list(self.time_series),
                    list(self.sensor_data[i]),
                    name=f'Sensor {i}',
                    pen=pg.mkPen(colors[i], width=2)
                )

    def update_status(self):
        if self.session_start is None:
            return

        elapsed = time.time() - self.session_start
        hours = int(elapsed // 3600)
        minutes = int((elapsed % 3600) // 60)
        seconds = int(elapsed % 60)

        self.status_samples.setText(f"Samples: {self.sample_count}")
        self.status_duration.setText(f"Duration: {hours:02d}:{minutes:02d}:{seconds:02d}")

        if elapsed > 0:
            rate = self.sample_count / elapsed
            self.status_rate.setText(f"Rate: {rate:.1f} Hz")

        if self.sample_count > 0:
            pct = 100 * self.anomaly_count / self.sample_count
            self.status_anomalies.setText(f"Anomalies: {self.anomaly_count} ({pct:.1f}%)")

    def run_full_analysis(self):
        if self.sample_count < 50:
            QMessageBox.information(self, "Info", "Collect at least 50 samples before running full analysis")
            return

        self.run_analysis_btn.setEnabled(False)
        self.statusBar().showMessage("Running full ML analysis...")

        self.analysis_worker = FullAnalysisWorker(self.anomaly_worker.all_data)
        self.analysis_worker.analysisComplete.connect(self.on_analysis_complete)
        self.analysis_worker.analysisError.connect(self.on_analysis_error)
        self.analysis_worker.start()

    def on_analysis_complete(self, result_df, message):
        self.run_analysis_btn.setEnabled(True)
        self.statusBar().showMessage("Full ML analysis complete!")
        
        msg = f"""
FULL ANOMALY DETECTION ANALYSIS COMPLETE
========================================

{message}

Files created:
- anomaly_detection_full_analysis.png (visualization)
- {ANOMALY_CSV_FILENAME} (data with anomaly scores)

Check the console for detailed results.
"""
        QMessageBox.information(self, "Analysis Complete", msg)

    def on_analysis_error(self, error):
        self.run_analysis_btn.setEnabled(True)
        self.statusBar().showMessage("Analysis failed")
        QMessageBox.critical(self, "Analysis Error", error)

    def export_csv(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Export CSV", "", "CSV Files (*.csv)")
        if filename:
            QMessageBox.information(self, "Success", f"Data would be exported to {filename}")

    def save_plots(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Save Plots", "", "PNG Files (*.png)")
        if filename:
            QMessageBox.information(self, "Success", f"Plots would be saved to {filename}")

    def stop_application(self):
        """Cleanly terminate all threads and close the application"""
        # Stop serial worker
        if self.serial_worker and self.serial_worker.isRunning():
            self.serial_worker.stop()
            self.serial_worker.wait(timeout=2000)

        # Stop anomaly worker
        if self.anomaly_worker and self.anomaly_worker.isRunning():
            self.anomaly_worker.stop()
            self.anomaly_worker.wait(timeout=2000)

        # Stop analysis worker if running
        if self.analysis_worker and self.analysis_worker.isRunning():
            self.analysis_worker.stop()
            self.analysis_worker.wait(timeout=2000)

        print("\nAll threads stopped. Exiting application.")
        QApplication.quit()


def main():
    app = QApplication(sys.argv)
    dashboard = ThermalDashboard()
    dashboard.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
