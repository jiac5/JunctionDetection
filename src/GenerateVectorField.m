function GenerateVectorField(image_path)

    A = imread(image_path);
    img = A;


    if size(A, 3) == 3
        A = rgb2gray(A);
    end

    info = imfinfo(image_path);
    A = wiener2(A,[5 5]);

    % figure; imshow(A);

    A = im2double(A);
    img = A;

    [w h] = size(A);
    field = zeros(w, h, 8);

    field(1:end-1, :, 1) = A(2:end, :) - A(1:end-1, :); 

    v{1} = [1 0];
    field(2:end, :, 2) = -A(2:end, :) + A(1:end-1, :); 
    v{2} = [-1 0];


    field(:, 1:end-1, 3) = A(:, 2:end) - A(:, 1:end-1); 
    v{3} = [0 1];
    field(:, 2:end, 4) = -A(:, 2:end) + A(:, 1:end-1); 
    v{4} = [0 -1];


    field(1:end-1, 1:end-1, 5) = A(2:end, 2:end) - A(1:end-1, 1:end-1); 
    v{5} = [1 1] / sqrt(2);
    field(1:end-1, 2:end, 6) = A(2:end, 1:end-1) - A(1:end-1, 2:end); 
    v{6} = [1 -1] / sqrt(2);

    field(2:end, 1:end-1, 7) = A(1:end-1, 2:end) - A(2:end, 1:end-1); 
    v{7} = [-1 1] / sqrt(2);
    field(2:end, 2:end, 8) = A(1:end-1, 1:end-1) - A(2:end, 2:end); 
    v{8} = [-1 -1] / sqrt(2);

    s = [0 0]';

    field = field(2:end-1, 2:end-1, :);

    % field = abs(field);

    x = zeros( (w-2) * (h-2), 1);
    y = zeros( (w-2) * (h-2), 1);
    dx = zeros( (w-2) * (h-2), 1);
    dy = zeros( (w-2) * (h-2), 1);

    confidences = zeros(w-2, h-2);

    for i = 1 : w - 2
        for j = 1 : h - 2
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
            if S(1,1) > 0.0005 && S(2, 2) > 0.0005
                confidence =  S(1, 1) / S(2, 2);
                confidences(i, j) = 1.0/abs(confidence);

                if confidence < 1.5
    %                 confidence = 0.0;
                else
                    if confidence > 3.0
                        confidence = 3.0;
                    end
                end
            end

            if img(i+1, j+1) < 0.1
                confidence = 0.0;
            end


            s = s + U(:, 2) * confidence;

            x(i * (h-2) + j) = j;
            y(i * (h-2) + j) = i;

            dx(i * (h-2) + j) = U(2, 2) * confidence;

            dy(i * (h-2) + j) = -U(1, 2) * confidence;

            if dx(i * (h-2) + j) < 0 
                dx(i * (h-2) + j) = -dx(i * (h-2) + j);
                dy(i * (h-2) + j) = -dy(i * (h-2) + j);
            end

        end
    end
    figure;
    imshow(img); hold on;
    quiver(x, y, dx, dy); hold on;
    axis([0, w, 0, h + 200]); 

end
