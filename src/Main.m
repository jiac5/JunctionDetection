% Parameters
image_path = '../images/Rabbit.bmp';

positive_row_ignore_threshold = 20;
positive_binary_ratio_threshold = 0.8;

negative_row_ignore_threshold = 20;
negative_binary_ratio_threshold = 0.2;

% Load image
original_image = imread(image_path);
if size(original_image, 3) == 3
    original_image = rgb2gray(original_image);
end

% Step 1. Convert into binary image
positive = Binary(original_image, positive_row_ignore_threshold, positive_binary_ratio_threshold, 255, 0);
negative = Binary(original_image, negative_row_ignore_threshold, negative_binary_ratio_threshold, 0, 255);

% Step 2. Generate skeletons for both positive and negative
positive = Thin(positive);
negative = Thin(negative);

% Step 3. Clean up
dlmwrite('positive.mat', positive);
dlmwrite('negative.mat', negative);

cleanup(1); % cleanup positive
cleanup(2); % cleanup negative

ShowPositiveNegative();
ShowOriginalNegative(image_path);