function [x y dx dy confidences] = GenerateVectorField(image_path)

    image = imread(image_path);
    if size(image, 3) == 3
        image = rgb2gray(image);
    end
    
    % smooth
    image = im2double(wiener2(image,[5 5]));
    A = image;
    
    [height width] = size(A);
    field = zeros(height, width, 8);

    field(1:end-1, :, 1) = A(2:end, :) - A(1:end-1, :); v{1} = [1 0];
    field(2:end, :, 2) = -A(2:end, :) + A(1:end-1, :); v{2} = [-1 0];
    field(:, 1:end-1, 3) = A(:, 2:end) - A(:, 1:end-1); v{3} = [0 1];
    field(:, 2:end, 4) = -A(:, 2:end) + A(:, 1:end-1); v{4} = [0 -1];
    field(1:end-1, 1:end-1, 5) = A(2:end, 2:end) - A(1:end-1, 1:end-1); 
    v{5} = [1 1] / sqrt(2);
    field(1:end-1, 2:end, 6) = A(2:end, 1:end-1) - A(1:end-1, 2:end); 
    v{6} = [1 -1] / sqrt(2);
    field(2:end, 1:end-1, 7) = A(1:end-1, 2:end) - A(2:end, 1:end-1); 
    v{7} = [-1 1] / sqrt(2);
    field(2:end, 2:end, 8) = A(1:end-1, 1:end-1) - A(2:end, 2:end); 
    v{8} = [-1 -1] / sqrt(2);

    field = field(2:end-1, 2:end-1, :);
    
    %s = [0; 0];

    x = zeros( (height-2) * (width-2), 1);
    y = zeros( (height-2) * (width-2), 1);
    dx = zeros( (height-2) * (width-2), 1);
    dy = zeros( (height-2) * (width-2), 1);

    confidences = zeros(height-2, width-2);

    for i = 1 : height - 2
        for j = 1 : width - 2
            A = zeros(2, 2);
            for k = 1 : 8
                val = field(i, j, k);
                if val > 0
                    val = 0;
                end
                A = A + v{k}' * v{k} .* (val * val);
            end

            [U S V] = svd(A);

    %         S(2,2)
            confidence = 0.0;
            if S(2,2) > 0
                confidence = S(1, 1) / S(2, 2);
            else
                confidence = 1;
            end

            x(i * (width-2) + j) = j;
            y(i * (width-2) + j) = i;
            
            if image(i+1, j+1) < 0.05
                confidence = 0.0;
            end
            
            confidences(i, j) = confidence;
            
            if confidence > 5
                confidence = 5;
            end
            
            dx(i * (width-2) + j) = U(2, 2) * confidence;
            dy(i * (width-2) + j) = -U(1, 2) * confidence;

            if dx(i * (width-2) + j) < 0 
                dx(i * (width-2) + j) = -dx(i * (width-2) + j);
                dy(i * (width-2) + j) = -dy(i * (width-2) + j);
            end

        end
    end
end
