function candidates = FindCandidates(image_path, confidences, gaussian_size, window_size)
    
    image = imread(image_path);
    if size(image, 3) == 3
        image = rgb2gray(image);
    end
    A = wiener2(image,[5 5]);
    A = im2double(A);
    A = A(2:end-1, 2:end-1);
    [height width] = size(A);
    
    
    peak_r = [];
    peak_c = [];

    newImg = zeros(height, width);

    peak_r_gaussian = [];
    peak_c_gaussian = [];

    filtered = zeros(height, width);

    window = window_size; 
    for i = 1 + window : width - window-1

        col = confidences(:, i - window);
        for ww = 1 : 2 * window
            col = col + confidences(:, i-window + ww );
        end

        for j = 1 : height 
            if confidences(j, i) == 0
                col(j) = 0;      
            end
        end

        newCol = col;
        for j = 2 : height-1
            if col(j) > col(j-1) && col(j) > col(j+1)
                peak_c = [peak_c i+1];
                peak_r = [peak_r j+1];
                newImg(j, i) = A(j, i);            
                newCol(j) = A(j, i);
            else
                newCol(j) = 0;
            end
        end

        col = newCol;

        gaussFilter = gausswin(gaussian_size);
        gaussFilter = gaussFilter / sum(gaussFilter);

        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);

        for j = 2 : height-2
            if col(j) > col(j-1) && col(j) > col(j+1)
                peak_c_gaussian = [peak_c_gaussian i+1];
                peak_r_gaussian = [peak_r_gaussian j+1];
            end
        end
    end
% 
%     figure;
%     plot(peak_c_gaussian, height - peak_r_gaussian, '.'); title('After Gaussian Filtering');
%     axis([0, width, 0, height]);


    [xx b] = size(peak_r_gaussian);
    o = zeros(height, width);
    for i = 1 : b
        o(peak_r_gaussian(i), peak_c_gaussian(i)) = 1;
    end 
    candidates = mat2gray(o);
end
