function [aggregatedData] = aggregateRawData(input, school, ...
    startDateTime, SInodes)
% aggregateRawData Aggregates raw data from PrNet sensors
%   input: name of folder containing txt input files
%   school: name of school the data was collected from
%   startDateTime: date of the first day of data collection, constructed
%       from datetime(Y,M,D,H,MI,S)
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
                rssi = floor(value / 1000000);
                value = value - abs(rssi) * 1000000;
                regionTracking = (hostID > SInodes - 1);
                hours = floor(value / 10000);
                value = value - hours * 10000;
                minutes = floor(value / 100);
                value = value - minutes * 100;
                seconds = value;
                % Check if time rolled over to next business day
                if (size(aggregatedData, 1) > 0)
                    if ((hours < hour(aggregatedData(end, 5))) || ...
                            (hours == hour(aggregatedData(end, 5)) && ...
                            minutes < minute(aggregatedData(end, 5))))
                        currentDateTime = busdate( ...
                            datestr(currentDateTime, 'dd-mmm-yyyy'), 1);
                    end
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

% Generate output file name
outputFileName = blanks(size(school, 2));
outputFileName(1) = upper(school(1));
for i = 2:length(school)
    outputFileName(i) = lower(school(i));
end
outputFileName = horzcat(outputFileName, '-');
outputFileName = horzcat(outputFileName, ...
    int2str(month(aggregatedData(1, 5))));
outputFileName = horzcat(outputFileName, '-');
outputFileName = horzcat(outputFileName, ...
    int2str(day(aggregatedData(1, 5))));
outputFileName = horzcat(outputFileName, '_');
outputFileName = horzcat(outputFileName, ...
    int2str(month(aggregatedData(end, 5))));
outputFileName = horzcat(outputFileName, '-');
outputFileName = horzcat(outputFileName, ...
    int2str(day(aggregatedData(end, 5))));

% Save aggregated data for later processing
save(outputFileName, 'aggregatedData');

end