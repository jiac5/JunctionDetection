function RotatedGaussian(candidates, image_path)
    img = candidates;
    
    

    % img = imresize(img, 8);
    [h w] = size(img);

    img = im2double(img);

    maxResponse = zeros(h, w);
    direction = zeros(h,w);

    gaussian_size = 6;

    for i = 1 : 12

        Theta = pi / 12 * i; 
        x0 = gaussian_size/2; y0 = gaussian_size/2;
        [x,y] = meshgrid(1:gaussian_size,1:gaussian_size);
        x2 = cos(Theta)*(x-x0)-sin(Theta)*(y-y0)+x0;
        y2 = sin(Theta)*(x-x0)+cos(Theta)*(x-x0)+y0;
        z2 = (exp(-((x2-x0)/1.5).^2).*(exp(-((y2-y0)/3).^2)));

        z2 = z2 / norm(z2);

        smoothed = filter2(z2, img);

        direction(smoothed > maxResponse) = Theta;
        maxResponse(smoothed > maxResponse) = smoothed(smoothed > maxResponse);


    %     figure;
    %     imagesc(smoothed); colormap(gray);
    % %     imshow(smoothed);
    % 
    %     figure;
    %     imagesc(z2); colormap(gray);
    %     
    %     path = strcat('F:/output/', num2str(i),'.jpg');
    %     smoothed(smoothed < 4) = 0;
    %     imwrite(smoothed, path);
    end

    count = 0;
    for i = 1 : h
        for j = 1 : w
            if maxResponse(i, j) > 1.0
                count = count + 1;
            end
        end
    end

    x = zeros(count, 1);
    y = zeros(count, 1);
    dx = zeros(count, 1);
    dy = zeros(count, 1);

    count = 0;
    for i = 1 : h
        for j = 1 : w
            if maxResponse(i, j) > 1.0 && img(i, j) > 0.1
                count = count + 1;

                x(count) = j;
                y(count) = i - 5;
                dx(count) = sin(direction(i, j));
                dy(count) = cos(direction(i, j));
            end
        end
    end

    figure;
    originalImage = imread(image_path);
    %imshow(originalImage); hold on;
    q = quiver(x, y, dx, dy); hold on;
    q.Color = 'blue';
    axis([0, w, 0, h + 100]); 
    title(strcat('gaussian shaped window size=', num2str(gaussian_size)));


    output = [x'; y'; dx'; dy'];
    fileID = fopen('output.txt','w');
    fprintf(fileID, '%f %f %f %f\n', output);

end
