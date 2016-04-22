function shown_image = ShowOriginalNegative(image_path)

original_image = imread(image_path);


negative = load('out_negative.mat', '-ascii');

[h w c] = size(negative);
shown_image = zeros(h, w, 3, 'uint8');

for i = 1 : h
    for j = 1 : w
        if negative(i, j) > 0
            shown_image(i, j, 2) = 255; % green
        else
            shown_image(i, j, 1) = original_image(i, j); % red
        end
    end
end

figure; imshow(shown_image);