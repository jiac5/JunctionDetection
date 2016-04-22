function shown_image = ShowPositiveNegative()

positive = load('out_positive.mat', '-ascii');
negative = load('out_negative.mat', '-ascii');

[h w] = size(positive);

shown_image = zeros(h, w, 3);

for i = 1 : h
    for j = 1 : w
        if positive(i, j) > 0
            shown_image(i, j, 1) = 255; % red
        elseif negative(i, j) > 0 
            shown_image(i, j, 2) = 255; % green
        end
    end
end

figure; imshow(shown_image);