function output = FindCandidates()
    image_path = 'F:\Hawk.bmp';
    A = imread(image_path);
    if size(A, 3) == 3
        A = rgb2gray(A);
    end
    A = wiener2(A,[5 5]);
    img = A;

    A = im2double(A);
    A = A(2:end-1, 2:end-1);


    [w h] = size(A);
    peak_x = [];
    peak_y = [];

    newImg = zeros(w, h);

    peak_x_gaussian = [];
    peak_y_gaussian = [];

    filtered = zeros(w, h);

    window = 3; % 
    for i = 1 + window : h - window-1

        col = A(:, i - window);
        for ww = 1 : 2 * window
            col = col + A(:, i-window + ww );
        end

        for j = 1 : w 
            if confidences(j, i) == 0
                col(j) = 0;      
            end
        end

        newCol = col;
        for j = 2 : w-1
            if col(j) > col(j-1) && col(j) > col(j+1)
                peak_x = [peak_x i+1];
                peak_y = [peak_y j+1];
                newImg(j, i) = A(j, i);            
                newCol(j) = A(j, i);
            else
                newCol(j) = 0;
            end
        end

        col = newCol;

        gaussFilter = gausswin(7);
        gaussFilter = gaussFilter / sum(gaussFilter);

        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);
        col = conv(col, gaussFilter);

        for j = 2 : w-2
            if col(j) > col(j-1) && col(j) > col(j+1)
                peak_x_gaussian = [peak_x_gaussian i+1];
                peak_y_gaussian = [peak_y_gaussian j+1];
            end
        end
    end

    figure;
    plot(peak_x_gaussian, w - peak_y_gaussian, '.'); title('After Gaussian Filtering');
    axis([0, h, 0, w]);


    [xx b] = size(peak_y_gaussian);
    o = zeros(w, h);
    for i = 1 : b
        o(peak_y_gaussian(i), peak_x_gaussian(i)) = 1;
    end
    imwrite(mat2gray(o), 'F:\tt.bmp');
    
    output = mat2gray(o);
end
