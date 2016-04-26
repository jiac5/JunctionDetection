function shown_image = ShowAll(original_image, negative, positive)
[h w c] = size(negative);
shown_image = zeros(h, w, 3, 'uint8');

for i = 1 : h
    for j = 1 : w
        if negative(i, j) > 0
            shown_image(i, j, 1) = 0;
            shown_image(i, j, 2) = 255;
            shown_image(i, j, 3) = 0;
        elseif positive(i, j) > 0
            shown_image(i, j, 1) = 255; 
            shown_image(i, j, 2) = 0;
            shown_image(i, j, 3) = 0;
        else
            shown_image(i, j, 1) = original_image(i, j) / 2;
            shown_image(i, j, 2) = original_image(i, j) / 2;
            shown_image(i, j, 3) = original_image(i, j) / 2;
        end
    end
end

figure; imshow(shown_image);