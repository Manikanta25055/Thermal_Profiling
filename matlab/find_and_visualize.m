%% Thermal Profiling System - Auto CSV Finder & Visualizer
% Automatically finds the latest CSV file and visualizes it
% Works from any directory
%
% Author: Manikanta (230906450)
% Course: M&I Lab (ELE 3162)

clear all; close all; clc;

%% ========================================================================
% CONFIGURATION
% ========================================================================
NUM_SENSORS = 4;
GRID_ROWS = 2;
GRID_COLS = 2;

% Base directory
BASE_DIR = '/Users/manikantagonugondla/Desktop/MIT/MIT/3rd Year/6th sem/M&I Lab/Thermal_Profiling_Project';

%% ========================================================================
% FIND CSV FILE
% ========================================================================
fprintf('\n========================================\n');
fprintf('Thermal Profiling System - Auto Visualizer\n');
fprintf('========================================\n\n');

% Search in python_scada directory first
scada_dir = fullfile(BASE_DIR, 'python_scada');
matlab_dir = fullfile(BASE_DIR, 'matlab');

fprintf('Searching for CSV files...\n\n');

csv_file = '';

% Try python_scada directory first (consistent filename)
if isfolder(scada_dir)
    consistent_file = fullfile(scada_dir, 'thermal_data.csv');
    if isfile(consistent_file)
        csv_file = consistent_file;
        fprintf('Found consistent filename in python_scada directory:\n');
        fprintf('  thermal_data.csv\n\n');
    else
        % Fallback to timestamped files
        files = dir(fullfile(scada_dir, 'thermal_data_*.csv'));
        if ~isempty(files)
            [~, idx] = max([files.datenum]);
            csv_file = fullfile(scada_dir, files(idx).name);
            fprintf('Found timestamped file in python_scada directory:\n');
            fprintf('  %s\n\n', files(idx).name);
        end
    end
end

% If not found, try matlab directory
if isempty(csv_file) && isfolder(matlab_dir)
    consistent_file = fullfile(matlab_dir, 'thermal_data.csv');
    if isfile(consistent_file)
        csv_file = consistent_file;
        fprintf('Found consistent filename in matlab directory:\n');
        fprintf('  thermal_data.csv\n\n');
    else
        % Fallback to timestamped files
        files = dir(fullfile(matlab_dir, 'thermal_data_*.csv'));
        if ~isempty(files)
            [~, idx] = max([files.datenum]);
            csv_file = fullfile(matlab_dir, files(idx).name);
            fprintf('Found timestamped file in matlab directory:\n');
            fprintf('  %s\n\n', files(idx).name);
        end
    end
end

% If still not found, search recursively
if isempty(csv_file)
    fprintf('Searching recursively in all subdirectories...\n\n');

    % Try consistent filename first
    files = dir(fullfile(BASE_DIR, '**', 'thermal_data.csv'));
    if ~isempty(files)
        csv_file = fullfile(files(1).folder, files(1).name);
        fprintf('Found consistent filename:\n');
        fprintf('  %s\n\n', csv_file);
    else
        % Fallback to timestamped files
        files = dir(fullfile(BASE_DIR, '**', 'thermal_data_*.csv'));
        if ~isempty(files)
            [~, idx] = max([files.datenum]);
            csv_file = fullfile(files(idx).folder, files(idx).name);
            fprintf('Found timestamped file:\n');
            fprintf('  %s\n\n', csv_file);
        end
    end
end

% If still not found, error
if isempty(csv_file)
    fprintf('ERROR: No CSV file found!\n');
    fprintf('Searched in:\n');
    fprintf('  1. %s\n', scada_dir);
    fprintf('  2. %s\n', matlab_dir);
    fprintf('  3. All subdirectories\n\n');
    fprintf('Please run the Python logger first to generate a CSV file.\n');
    return;
end

%% ========================================================================
% READ CSV FILE
% ========================================================================
fprintf('Reading CSV file...\n');

try
    opts = detectImportOptions(csv_file);
    opts.VariableNamingRule = 'preserve';
    data_table = readtable(csv_file, opts);

    num_samples = height(data_table);
    fprintf('✓ Successfully loaded %d samples\n\n', num_samples);

catch ME
    fprintf('ERROR reading CSV file: %s\n', ME.message);
    return;
end

%% ========================================================================
% EXTRACT DATA
% ========================================================================
elapsed_time = table2array(data_table(:, 2));
sensor_0 = table2array(data_table(:, 3));
sensor_1 = table2array(data_table(:, 4));
sensor_2 = table2array(data_table(:, 5));
sensor_3 = table2array(data_table(:, 6));
max_temp = table2array(data_table(:, 7));
min_temp = table2array(data_table(:, 8));
avg_temp = table2array(data_table(:, 9));
gradient = table2array(data_table(:, 10));
hotspot = table2array(data_table(:, 11));

sensor_data = [sensor_0, sensor_1, sensor_2, sensor_3];

%% ========================================================================
% CREATE FIGURE
% ========================================================================
fig = figure('Name', 'Thermal Profiling System - MATLAB Visualization', ...
             'NumberTitle', 'off', ...
             'Position', [100 100 1400 900]);

%% ========================================================================
% SUBPLOT 1: CURRENT THERMAL HEATMAP
% ========================================================================
ax1 = subplot(2, 3, [1 4]);

latest_temps = sensor_data(end, :);
temp_grid = reshape(latest_temps, GRID_ROWS, GRID_COLS);

im = imagesc(ax1, temp_grid, [min(min(sensor_data))-0.5, max(max(sensor_data))+0.5]);
colormap(ax1, 'hot');
cb = colorbar(ax1);
cb.Label.String = 'Temperature (C)';
cb.Label.FontSize = 11;

hold(ax1, 'on');
for i = 1:GRID_ROWS
    for j = 1:GRID_COLS
        text(j, i, sprintf('%.2fC', temp_grid(i,j)), ...
            'HorizontalAlignment', 'center', ...
            'VerticalAlignment', 'middle', ...
            'Color', 'white', ...
            'FontWeight', 'bold', ...
            'FontSize', 12);
    end
end

set(ax1, 'XTick', 1:GRID_COLS, 'YTick', 1:GRID_ROWS);
set(ax1, 'XTickLabel', {'S0', 'S2'}, 'YTickLabel', {'Row1', 'Row2'});
xlabel(ax1, 'Column', 'FontSize', 11);
ylabel(ax1, 'Row', 'FontSize', 11);
title(ax1, 'Current Thermal Heatmap (Latest Sample)', 'FontSize', 13, 'FontWeight', 'bold');
grid(ax1, 'off');

%% ========================================================================
% SUBPLOT 2: TEMPERATURE TRENDS
% ========================================================================
ax2 = subplot(2, 3, [2 3]);

colors = [0.2 0.8 1; 1 0.2 0.2; 0.2 1 0.2; 1 0.6 0];

for i = 1:NUM_SENSORS
    plot(ax2, elapsed_time, sensor_data(:, i), ...
        'Color', colors(i, :), ...
        'LineWidth', 2, ...
        'DisplayName', sprintf('Sensor %d', i-1));
    hold(ax2, 'on');
end

xlabel(ax2, 'Elapsed Time (s)', 'FontSize', 11);
ylabel(ax2, 'Temperature (C)', 'FontSize', 11);
title(ax2, 'Individual Sensor Temperature Trends', 'FontSize', 13, 'FontWeight', 'bold');
legend(ax2, 'Location', 'best', 'FontSize', 10);
grid(ax2, 'on');
set(ax2, 'FontSize', 10);

%% ========================================================================
% SUBPLOT 3: THERMAL ANALYSIS
% ========================================================================
ax3 = subplot(2, 3, 5);

plot(ax3, elapsed_time, max_temp, 'r-', 'LineWidth', 2, 'DisplayName', 'Max Temp');
hold(ax3, 'on');
plot(ax3, elapsed_time, min_temp, 'b-', 'LineWidth', 2, 'DisplayName', 'Min Temp');
plot(ax3, elapsed_time, avg_temp, 'g-', 'LineWidth', 2, 'DisplayName', 'Avg Temp');

xlabel(ax3, 'Elapsed Time (s)', 'FontSize', 11);
ylabel(ax3, 'Temperature (C)', 'FontSize', 11);
title(ax3, 'Thermal Analysis Over Time', 'FontSize', 13, 'FontWeight', 'bold');
legend(ax3, 'Location', 'best', 'FontSize', 10);
grid(ax3, 'on');
set(ax3, 'FontSize', 10);

%% ========================================================================
% SUBPLOT 4: THERMAL GRADIENT
% ========================================================================
ax4 = subplot(2, 3, 6);

plot(ax4, elapsed_time, gradient, 'Color', [1 0.5 0], 'LineWidth', 2);
xlabel(ax4, 'Elapsed Time (s)', 'FontSize', 11);
ylabel(ax4, 'Temperature Difference (C)', 'FontSize', 11);
title(ax4, 'Thermal Gradient (Max - Min)', 'FontSize', 13, 'FontWeight', 'bold');
grid(ax4, 'on');
set(ax4, 'FontSize', 10);

%% ========================================================================
% DISPLAY STATISTICS
% ========================================================================
fprintf('\n========== THERMAL DATA STATISTICS ==========\n\n');

fprintf('Time Range:\n');
fprintf('  Start: 0.00 seconds\n');
fprintf('  End: %.2f seconds\n', elapsed_time(end));
fprintf('  Duration: %.2f seconds\n\n', elapsed_time(end));

fprintf('Temperature Statistics (All Sensors):\n');
fprintf('  Overall Max: %.2fC\n', max(max(sensor_data)));
fprintf('  Overall Min: %.2fC\n', min(min(sensor_data)));
fprintf('  Overall Avg: %.2fC\n\n', mean(mean(sensor_data)));

fprintf('Per-Sensor Statistics:\n');
for i = 1:NUM_SENSORS
    fprintf('  Sensor %d:\n', i-1);
    fprintf('    Max: %.2fC\n', max(sensor_data(:, i)));
    fprintf('    Min: %.2fC\n', min(sensor_data(:, i)));
    fprintf('    Avg: %.2fC\n', mean(sensor_data(:, i)));
    fprintf('    StdDev: %.4fC\n\n', std(sensor_data(:, i)));
end

fprintf('Thermal Analysis:\n');
fprintf('  Max Temperature Range: %.2f - %.2fC\n', min(max_temp), max(max_temp));
fprintf('  Min Temperature Range: %.2f - %.2fC\n', min(min_temp), max(min_temp));
fprintf('  Avg Temperature Range: %.2f - %.2fC\n', min(avg_temp), max(avg_temp));
fprintf('  Thermal Gradient Range: %.2f - %.2fC\n', min(gradient), max(gradient));
fprintf('  Hotspot Frequency:\n');

for i = 0:(NUM_SENSORS-1)
    count = sum(hotspot == i);
    pct = 100 * count / num_samples;
    fprintf('    Sensor %d: %d times (%.1f%%)\n', i, count, pct);
end

fprintf('\n============================================\n\n');

fprintf('Data loaded from:\n');
fprintf('  %s\n\n', csv_file);

fprintf('Visualization complete!\n\n');
