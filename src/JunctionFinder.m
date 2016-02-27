image_path = '1.bmp';
image_path = 'River.png';

oringinal_image = imread(image_path);

if size(oringinal_image, 3) == 3
    oringinal_image = rgb2gray(oringinal_image);
end
% 1. gaussian filter to remove noise
image = imgaussfilt(oringinal_image, 2);
[w h] = size(image);

image = wiener2(image,[5 5]);

image(image < 0.1) = 0;

Y = zeros(w-2, h-2);

% 2. 
[gx, gy] = gradient(double(image));
[gxx, gxy] = gradient(gx);
[gxy, gyy] = gradient(gy);


for i = 1 : w-2
    for j=1 : h-2
        A = [gxx(i+1, j+1)  gxy(i+1, j+1); gxy(i+1, j+1) gyy(i+1, j+1)];
        
        [U S V] = svd(A);
        
        Y(i, j) = abs(S(2,2) / S(1,1));
        
        if S(2, 2) < 0.1
            Y(i, j) = 0;
        end
    end
end


% figure; imshow(Y);


A = log(confidences + 2);
B = -exp(Y);

X = confidences;
C = X .* X ./ Y;
C(Y == 0) = 0;


phi = 80 * A + 4 * B + C;

phi_copy = phi;

thres = 100;
phi(phi < thres) = 0;

% figure;
% imshow(phi);

x = [];
y = [];
for i = 1 : w-2
    for j=1 : h-2
        if phi(i, j) > 0
            x = [x j+1];
            y = [y i+1];
        end       
    end
end
% figure;
% imshow(oringinal_image); hold on;
plot(x, y, '*r'); title(strcat('threshold = ', num2str(thres)));