function binary_image = Binary(image, row_ignore_threshold, binary_ratio_threshold, foreground_value, background_value)
% Convert gray image into binary image
% image :               input image
% row_ignore_threshold :    the lines with mean value less than this threshold will be
%                       considered as background (the whole row)
% binary_ratio_threshold: the pixels with value less than
%                       avg_line_pixel_value * binary_ratio_threshold are
%                       considered as background
% foreground_value : foreground_value in the result image
% background_value : background value in the result image

[h w] = size(image);
for i = 1 : h
    row = image(i, :);
    mean_val = mean(row);
    if mean_val < row_ignore_threshold
       row = background_value; % ignore the whole row
    else
        binary_threshold = mean_val * binary_ratio_threshold;
        fore = row > binary_threshold;
        back = row <= binary_threshold;
        row(fore) = foreground_value;
        row(back) = background_value;
    end
    
    image(i, :) = row;
end

binary_image = image;