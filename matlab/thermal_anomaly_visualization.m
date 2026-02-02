%% Thermal Profiling System - Anomaly Visualization & Analysis
% Reads and visualizes thermal_data_with_anomalies.csv with comprehensive analysis
% Automatically locates the file in python_scada folder
%
% Author: Manikanta (230906450)
% Course: M&I Lab (ELE 3162)
% Date: Feb 2025

clear; close all; clc;

fprintf('\n%s\n', repmat('=', 1, 80));
fprintf('THERMAL PROFILING SYSTEM - COMPREHENSIVE ANOMALY ANALYSIS\n');
fprintf('%s\n', repmat('=', 1, 80));

% ========================================================================
% FIND AND READ ANOMALY DATA FILE
% ========================================================================

% Get current directory and look for python_scada folder
current_dir = pwd;
parent_dir = fileparts(current_dir);

% Try different possible paths
possible_paths = {
    'thermal_data_with_anomalies.csv';  % Current directory
    fullfile(current_dir, '..', 'python_scada', 'thermal_data_with_anomalies.csv');
    fullfile(parent_dir, 'python_scada', 'thermal_data_with_anomalies.csv');
    fullfile(fileparts(parent_dir), 'python_scada', 'thermal_data_with_anomalies.csv');
};

anomaly_csv_file = '';
for i = 1:length(possible_paths)
    if isfile(possible_paths{i})
        anomaly_csv_file = possible_paths{i};
        fprintf('Found anomaly data file at:\n  %s\n\n', anomaly_csv_file);
        break;
    end
end

if isempty(anomaly_csv_file)
    fprintf('ERROR: Could not find thermal_data_with_anomalies.csv\n');
    fprintf('Searched in:\n');
    for i = 1:length(possible_paths)
        fprintf('  - %s\n', possible_paths{i});
    end
    fprintf('\nRun the Full ML Analysis button in the dashboard first.\n\n');
    return;
end

% ========================================================================
% READ DATA
% ========================================================================
fprintf('Loading anomaly analysis data...\n\n');

try
    opts = detectImportOptions(anomaly_csv_file, 'FileType', 'text');
    data = readtable(anomaly_csv_file, opts);
    fprintf('✓ Loaded %d samples\n\n', height(data));
catch ME
    fprintf('ERROR: Could not read file - %s\n', ME.message);
    return;
end

% ========================================================================
% EXTRACT DATA
% ========================================================================
elapsed_time = data.elapsed_time;
sensor_0 = data.sensor_0;
sensor_1 = data.sensor_1;
sensor_2 = data.sensor_2;
sensor_3 = data.sensor_3;
max_temp = data.max_temp;
min_temp = data.min_temp;
avg_temp = data.avg_temp;
gradient = data.gradient;
anomaly_score = data.Anomaly_Score;
is_anomaly = logical(data.Is_Anomaly);

% ========================================================================
% COMPREHENSIVE ANALYSIS
% ========================================================================

fprintf('%s\n', repmat('=', 1, 80));
fprintf('ANOMALY DETECTION RESULTS\n');
fprintf('%s\n', repmat('=', 1, 80));

% Basic statistics
num_total = length(is_anomaly);
num_normal = sum(~is_anomaly);
num_anomalies = sum(is_anomaly);
pct_anomalies = 100 * num_anomalies / num_total;

fprintf('\nSample Statistics:\n');
fprintf('  Total Samples: %d\n', num_total);
fprintf('  Normal Samples: %d\n', num_normal);
fprintf('  Anomalous Samples: %d (%.2f%%)\n\n', num_anomalies, pct_anomalies);

% ========================================================================
% SENSOR ANALYSIS
% ========================================================================
fprintf('Sensor Temperature Analysis:\n');
fprintf('  Sensor 0: Mean=%.2f°C, Std=%.2f°C, Min=%.2f°C, Max=%.2f°C\n', ...
    mean(sensor_0), std(sensor_0), min(sensor_0), max(sensor_0));
fprintf('  Sensor 1: Mean=%.2f°C, Std=%.2f°C, Min=%.2f°C, Max=%.2f°C\n', ...
    mean(sensor_1), std(sensor_1), min(sensor_1), max(sensor_1));
fprintf('  Sensor 2: Mean=%.2f°C, Std=%.2f°C, Min=%.2f°C, Max=%.2f°C\n', ...
    mean(sensor_2), std(sensor_2), min(sensor_2), max(sensor_2));
fprintf('  Sensor 3: Mean=%.2f°C, Std=%.2f°C, Min=%.2f°C, Max=%.2f°C\n\n', ...
    mean(sensor_3), std(sensor_3), min(sensor_3), max(sensor_3));

% ========================================================================
% ANOMALY CHARACTERIZATION
% ========================================================================
fprintf('Anomaly Characterization:\n');

if num_anomalies > 0
    % Get anomaly indices
    anomaly_idx = find(is_anomaly);
    anomaly_sensor_0 = sensor_0(anomaly_idx);
    anomaly_sensor_1 = sensor_1(anomaly_idx);
    anomaly_sensor_2 = sensor_2(anomaly_idx);
    anomaly_sensor_3 = sensor_3(anomaly_idx);
    anomaly_gradient = gradient(anomaly_idx);
    anomaly_time = elapsed_time(anomaly_idx);
    anomaly_score_vals = anomaly_score(anomaly_idx);
    
    fprintf('  Anomalies detected at timestamps:\n');
    for i = 1:min(10, length(anomaly_idx))
        fprintf('    Time: %.1fs, Score: %.3f, S0:%.2f, S1:%.2f, S2:%.2f, S3:%.2f, Grad:%.2f\n', ...
            anomaly_time(i), anomaly_score_vals(i), ...
            anomaly_sensor_0(i), anomaly_sensor_1(i), anomaly_sensor_2(i), anomaly_sensor_3(i), ...
            anomaly_gradient(i));
    end
    if length(anomaly_idx) > 10
        fprintf('    ... and %d more anomalies\n', length(anomaly_idx) - 10);
    end
    
    fprintf('\n  Anomaly Statistics:\n');
    fprintf('    Mean Score: %.3f\n', mean(anomaly_score_vals));
    fprintf('    Min Score: %.3f\n', min(anomaly_score_vals));
    fprintf('    Max Score: %.3f\n\n', max(anomaly_score_vals));
    
    % Anomaly types
    fprintf('  Anomaly Type Classification:\n');
    
    % High temperature anomalies
    high_temp_anomalies = sum((max_temp(anomaly_idx) > mean(max_temp) + 2*std(max_temp)));
    fprintf('    High Temperature Events: %d\n', high_temp_anomalies);
    
    % High gradient anomalies (rapid changes)
    high_grad_anomalies = sum((anomaly_gradient > mean(gradient) + 2*std(gradient)));
    fprintf('    Rapid Temperature Change Events: %d\n', high_grad_anomalies);
    
    % Low temperature anomalies
    low_temp_anomalies = sum((min_temp(anomaly_idx) < mean(min_temp) - 2*std(min_temp)));
    fprintf('    Low Temperature Events: %d\n\n', low_temp_anomalies);
else
    fprintf('  No anomalies detected\n\n');
end

% ========================================================================
% GRADIENT ANALYSIS
% ========================================================================
fprintf('Thermal Gradient Analysis:\n');
fprintf('  Mean Gradient: %.2f°C\n', mean(gradient));
fprintf('  Std Gradient: %.2f°C\n', std(gradient));
fprintf('  Max Gradient: %.2f°C\n', max(gradient));
fprintf('  Min Gradient: %.2f°C\n\n', min(gradient));

% ========================================================================
% NORMAL vs ANOMALY COMPARISON
% ========================================================================
fprintf('Normal Conditions vs Anomalous Conditions:\n');

normal_sensor_0 = sensor_0(~is_anomaly);
normal_sensor_1 = sensor_1(~is_anomaly);
normal_sensor_2 = sensor_2(~is_anomaly);
normal_sensor_3 = sensor_3(~is_anomaly);

if num_anomalies > 0
    fprintf('  Sensor 0:\n');
    fprintf('    Normal:    Mean=%.2f±%.2f°C\n', mean(normal_sensor_0), std(normal_sensor_0));
    fprintf('    Anomalous: Mean=%.2f±%.2f°C\n\n', mean(anomaly_sensor_0), std(anomaly_sensor_0));
    
    fprintf('  Sensor 1:\n');
    fprintf('    Normal:    Mean=%.2f±%.2f°C\n', mean(normal_sensor_1), std(normal_sensor_1));
    fprintf('    Anomalous: Mean=%.2f±%.2f°C\n\n', mean(anomaly_sensor_1), std(anomaly_sensor_1));
    
    fprintf('  Sensor 2:\n');
    fprintf('    Normal:    Mean=%.2f±%.2f°C\n', mean(normal_sensor_2), std(normal_sensor_2));
    fprintf('    Anomalous: Mean=%.2f±%.2f°C\n\n', mean(anomaly_sensor_2), std(anomaly_sensor_2));
    
    fprintf('  Sensor 3:\n');
    fprintf('    Normal:    Mean=%.2f±%.2f°C\n', mean(normal_sensor_3), std(normal_sensor_3));
    fprintf('    Anomalous: Mean=%.2f±%.2f°C\n\n', mean(anomaly_sensor_3), std(anomaly_sensor_3));
end

% ========================================================================
% CREATE FIGURE
% ========================================================================
fig = figure('Name', 'Thermal Profiling - Anomaly Analysis', ...
             'NumberTitle', 'off', ...
             'Position', [100 100 1400 900]);

% ========================================================================
% PLOT 1: SENSOR VALUES WITH ANOMALIES HIGHLIGHTED
% ========================================================================
ax1 = subplot(2, 2, 1);
hold on;

colors = [1 0 0; 0 1 0; 0 0 1; 1 0.65 0];  % Red, Green, Blue, Orange

plot(ax1, elapsed_time, sensor_0, 'Color', colors(1,:), 'LineWidth', 2, 'DisplayName', 'Sensor 0');
plot(ax1, elapsed_time, sensor_1, 'Color', colors(2,:), 'LineWidth', 2, 'DisplayName', 'Sensor 1');
plot(ax1, elapsed_time, sensor_2, 'Color', colors(3,:), 'LineWidth', 2, 'DisplayName', 'Sensor 2');
plot(ax1, elapsed_time, sensor_3, 'Color', colors(4,:), 'LineWidth', 2, 'DisplayName', 'Sensor 3');

% Highlight anomaly regions
if num_anomalies > 0
    anomaly_times = elapsed_time(is_anomaly);
    for i = 1:length(anomaly_times)
        xline(ax1, anomaly_times(i), '--r', 'Alpha', 0.3, 'LineWidth', 1);
    end
end

xlabel(ax1, 'Elapsed Time (s)', 'FontSize', 11, 'FontWeight', 'bold');
ylabel(ax1, 'Temperature (°C)', 'FontSize', 11, 'FontWeight', 'bold');
title(ax1, 'Sensor Readings with Anomalies (Red Lines)', 'FontSize', 12, 'FontWeight', 'bold');
grid(ax1, 'on', 'alpha', 0.3);
legend(ax1, 'Location', 'best');

% ========================================================================
% PLOT 2: ANOMALY SCORES
% ========================================================================
ax2 = subplot(2, 2, 2);
hold on;

% Normal points (green)
normal_idx_bool = ~is_anomaly;
scatter(ax2, elapsed_time(normal_idx_bool), anomaly_score(normal_idx_bool), ...
        30, 'g', 'filled', 'DisplayName', 'Normal');

% Anomaly points (red)
if num_anomalies > 0
    anomaly_idx_bool = is_anomaly;
    scatter(ax2, elapsed_time(anomaly_idx_bool), anomaly_score(anomaly_idx_bool), ...
            50, 'r', 'x', 'LineWidth', 2, 'DisplayName', 'Anomaly');
end

% Add threshold line
yline(ax2, -0.5, '--orange', 'LineWidth', 2, 'DisplayName', 'Threshold (-0.5)');

xlabel(ax2, 'Elapsed Time (s)', 'FontSize', 11, 'FontWeight', 'bold');
ylabel(ax2, 'Anomaly Score', 'FontSize', 11, 'FontWeight', 'bold');
title(ax2, 'Isolation Forest Anomaly Scores', 'FontSize', 12, 'FontWeight', 'bold');
grid(ax2, 'on', 'alpha', 0.3);
legend(ax2, 'Location', 'best');

% ========================================================================
% PLOT 3: MIN/MAX/AVG TEMPERATURE
% ========================================================================
ax3 = subplot(2, 2, 3);
hold on;

plot(ax3, elapsed_time, max_temp, 'r-', 'LineWidth', 2.5, 'DisplayName', 'Max');
plot(ax3, elapsed_time, avg_temp, 'b-', 'LineWidth', 2.5, 'DisplayName', 'Avg');
plot(ax3, elapsed_time, min_temp, 'g-', 'LineWidth', 2.5, 'DisplayName', 'Min');

% Highlight anomalies
if num_anomalies > 0
    for i = 1:length(anomaly_times)
        xline(ax3, anomaly_times(i), '--r', 'Alpha', 0.3, 'LineWidth', 1);
    end
end

xlabel(ax3, 'Elapsed Time (s)', 'FontSize', 11, 'FontWeight', 'bold');
ylabel(ax3, 'Temperature (°C)', 'FontSize', 11, 'FontWeight', 'bold');
title(ax3, 'Temperature Bounds (Max/Avg/Min)', 'FontSize', 12, 'FontWeight', 'bold');
grid(ax3, 'on', 'alpha', 0.3);
legend(ax3, 'Location', 'best');

% ========================================================================
% PLOT 4: COMPREHENSIVE STATISTICS
% ========================================================================
ax4 = subplot(2, 2, 4);
axis(ax4, 'off');

% Create comprehensive statistics text
stats_text = sprintf(...
    ['COMPREHENSIVE ANOMALY ANALYSIS\n' ...
     '================================\n\n' ...
     'SAMPLE STATISTICS:\n' ...
     '  Total Samples: %d\n' ...
     '  Normal: %d (%.1f%%)\n' ...
     '  Anomalous: %d (%.1f%%)\n' ...
     '  Duration: %.1f seconds\n' ...
     '  Sample Rate: %.2f Hz\n\n' ...
     'SENSOR TEMPERATURE RANGES:\n' ...
     '  S0: %.2f - %.2f°C (Mean: %.2f)\n' ...
     '  S1: %.2f - %.2f°C (Mean: %.2f)\n' ...
     '  S2: %.2f - %.2f°C (Mean: %.2f)\n' ...
     '  S3: %.2f - %.2f°C (Mean: %.2f)\n\n' ...
     'THERMAL GRADIENT:\n' ...
     '  Mean: %.2f°C\n' ...
     '  Range: %.2f - %.2f°C\n' ...
     '  Std Dev: %.2f°C\n\n' ...
     'ANOMALY STATISTICS:\n' ...
     '  Detection Rate: %.2f%%\n' ...
     '  Avg Anomaly Score: %.3f\n' ...
     '  Score Range: %.3f - %.3f'], ...
    num_total, num_normal, 100*num_normal/num_total, ...
    num_anomalies, pct_anomalies, ...
    elapsed_time(end), num_total/elapsed_time(end), ...
    min(sensor_0), max(sensor_0), mean(sensor_0), ...
    min(sensor_1), max(sensor_1), mean(sensor_1), ...
    min(sensor_2), max(sensor_2), mean(sensor_2), ...
    min(sensor_3), max(sensor_3), mean(sensor_3), ...
    mean(gradient), min(gradient), max(gradient), std(gradient), ...
    pct_anomalies, ...
    mean(anomaly_score), min(anomaly_score), max(anomaly_score));

text(ax4, 0.05, 0.95, stats_text, ...
     'VerticalAlignment', 'top', ...
     'HorizontalAlignment', 'left', ...
     'FontSize', 9.5, ...
     'FontFamily', 'monospace', ...
     'BackgroundColor', [0.95 0.95 0.95], ...
     'EdgeColor', [0.2 0.2 0.8], ...
     'LineWidth', 1.5, ...
     'Margin', 8);

% ========================================================================
% OVERALL TITLE
% ========================================================================
sgtitle(fig, sprintf('Thermal Profiling - Anomaly Detection Analysis (%d samples, %d anomalies = %.1f%%)', ...
                     num_total, num_anomalies, pct_anomalies), ...
        'FontSize', 14, 'FontWeight', 'bold');

% ========================================================================
% SAVE FIGURE
% ========================================================================
fprintf('%s\n', repmat('=', 1, 80));
fprintf('SAVING VISUALIZATIONS\n');
fprintf('%s\n', repmat('=', 1, 80));

savefig(fig, 'thermal_anomaly_visualization.fig');
saveas(fig, 'thermal_anomaly_visualization.png');

fprintf('\n✓ Figures saved:\n');
fprintf('  - thermal_anomaly_visualization.fig (MATLAB figure)\n');
fprintf('  - thermal_anomaly_visualization.png (image)\n');
fprintf('\n%s\n', repmat('=', 1, 80));
fprintf('ANALYSIS COMPLETE\n');
fprintf('%s\n\n', repmat('=', 1, 80));
