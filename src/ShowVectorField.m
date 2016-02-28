function ShowVectorField(x, y, dx, dy, image_path)
    figure;
    image = imread(image_path);
    imshow(image); hold on;
    
    [h w] = size(image);
    
    quiver(x, y, dx, dy, 'color', [0 0 1]); hold on;
    axis([0, w, 0, h]); 
end