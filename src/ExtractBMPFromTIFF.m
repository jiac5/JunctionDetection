image_path = '../images/Adult_chicken_central reduced.tif';
info = imfinfo(image_path);
num_images = numel(info);

for k=1:num_images
    k
    A = imread(image_path, k, 'Info', info);
    
    imwrite(A, strcat('../images/Chicken/', num2str(k), '.bmp'));
end