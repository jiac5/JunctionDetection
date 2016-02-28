% clear

image_path = '../images/Hawk.bmp'; gaussian_size = 11; window_size = 4;

% image_path = '../images/Rabbit.bmp'; gaussian_size = 9; window_size = 2;

% [x y dx dy confidences] = GenerateVectorField(image_path);
% 
% ShowVectorField(x, y, dx, dy, image_path);

candidates = FindCandidates(image_path, confidences, gaussian_size, window_size);
                                                                                                                                                                             
% ShowCandidates(candidates, image_path, gaussian_size);


figure;
imshow(candidates); title(strcat('gaussian size=', num2str(gaussian_size), ', window size=', num2str(window_size)));

RotatedGaussian(candidates, image_path);

                                    