function [aggregatedData] = aggregateRawData(input, output, ...
    startDateTime, SInodes)
% processRawData Procosses unprocessed data from PrNet sensors
%   input: name of folder containing txt input files
%   output: name of output file containing aggregated data
%   date: date of the first day of data collection, constructed from
%       datetime(Y,M,D,H,MI,S)
%   startHour: hour at which data collection began
%   startMinute: minute at which data collection began
%   SInodes: number of social interaction network nodes
%   aggregatedData: aggregated data with the following schema:
%       hostID deviceID RSSI regionTracking time

% Create data structure for holding aggregate raw data
aggregatedData = zeros(0, 5);

% Iterate over input txt files
files = dir(strcat(input, '/*.txt'));
for file = files'
    % Open file
    fileID = fopen(file.name);
    % Get receiving sensor ID from file name
    fileNameExtension = strsplit(file.name, '.');
    hostID = str2double(cell2mat(fileNameExtension(1)));
    % Get line from input file
    textLine = fgetl(fileID);
    % Set initial data collection date to startDateTime
    currentDateTime = startDateTime;
    % Iterate over file lines
    while ischar(textLine)
        % Check if line is a numerical representation
        if (~cellfun('isempty', regexp({textLine}, '^-?\d+$')))
            value = str2double(textLine);
            % Consider only valid data entries
            if (value > 1000000)
                % Compute row data components
                deviceID = floor(value / 100000000);
                value = value - deviceID * 100000000;
                rssi = -1 * floor(value / 1000000);
                value = value - rssi * 1000000;
                regionTracking = (hostID > SInodes - 1);
                hours = floor(value / 10000);
                value = value - hours * 10000;
                minutes = floor(value / 100);
                value = value - minutes * 100;
                seconds = value;
                % Check if time rolled over to next business day
                if ((hours < hour(currentDateTime)) || ...
                        (hours == hour(currentDateTime) && ...
                        minutes < minute(currentDateTime)))
                    currentDateTime = busdate(currentDateTime, 1);
                end
                time = datetime(year(currentDateTime), ...
                    month(currentDateTime), ...
                    day(currentDateTime), ...
                    hours, minutes, seconds);
                time = cellstr(time);
                dataRow = [hostID deviceID rssi regionTracking time];
                aggregatedData = [aggregatedData; dataRow];
            end
        end
        % Continue onto next line
        textLine = fgetl(fileID);
    end
    % Close file
    fclose(fileID);
    break;
end

% Save aggregated data for later processing
save(output, 'aggregatedData');

end