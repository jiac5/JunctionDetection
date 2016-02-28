function ShowCandidates(candidates, image_path, gaussian_size)
    figure;
    image = imread(image_path);
    
    [h w] = size(candidates);
    
    
    offset = 2 * gaussian_size + 10;
    
    for i = 1 : h
        for j = 1 : w
            if candidates(i, j) > 0.1 && i > offset
                image(i-offset, j+1, 1) = 0;
                image(i-offset, j+1, 2) = 255;
                image(i-offset, j+1, 3) = 0;
            
            end
        end
    end
    
    imshow(image); 
end