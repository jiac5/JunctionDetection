mex cleanup.cpp;
mex RefineNegativeUsingPositive.cpp;
mex LinkNearEndpoints.cpp;
mex RemoveHolesInPositive.cpp;
for imageID = 1 : 1 %45
    image_path = strcat('../images/Rabbit/', num2str(imageID), '.bmp');
    positive_row_ignore_threshold = 20;
    positive_binary_ratio_threshold = 0.8;

    negative_row_ignore_threshold = 20;
    negative_binary_ratio_threshold = 0.35;

    % Load image
    original_image = imread(image_path);
    if size(original_image, 3) == 3
        original_image = rgb2gray(original_image);
    end

    % Step 1. Convert into binary image
    positive = Binary(original_image, positive_row_ignore_threshold, positive_binary_ratio_threshold, 255, 0);
    negative = Binary(original_image, negative_row_ignore_threshold, negative_binary_ratio_threshold, 0, 255);
    
    % Step 2. Generate skeletons for both positive and negative
    positive = Thin(positive);
    negative = Thin(negative);

    % ShowOriginalNegative(original_image, negative);

    % Step 3. Clean up
    dlmwrite('positive.mat', positive);
    dlmwrite('negative.mat', negative);

    cleanup(1); % cleanup positive
    cleanup(2); % cleanup negative

    positive = load('out_positive.mat', '-ascii');
    negative = load('out_negative.mat', '-ascii');

    fig = ShowPositiveNegative(positive, negative);
    imwrite(fig, strcat('../images/Positive_Negative/before_', num2str(imageID), '.bmp'));
    close;
    fig = ShowOriginalNegative(original_image, negative);
    imwrite(fig, strcat('../images/Original_Negative/before_', num2str(imageID), '.bmp'));
    close;

    % Step 4. Refine negative using positive
    RefineNegativeUsingPositive();

    negative_step4 = load('out_negative_step4.mat', '-ascii');
    fig = ShowPositiveNegative(positive, negative_step4);
    imwrite(fig, strcat('../images/Positive_Negative/after_', num2str(imageID), '.bmp'));
    close;
    
    fig = ShowOriginalNegative(original_image, negative_step4);
    imwrite(fig, strcat('../images/Original_Negative/after_', num2str(imageID), '.bmp'));
    close;
    
    
    % Step 5. Link end points which are near each other, without pixels
    % blocking them
    LinkNearEndpoints();
    
    negative_step5 = load('out_negative_step5.mat', '-ascii');
    positive_step5 = load('out_positive_step5.mat', '-ascii');
    fig = ShowOriginalNegative(original_image, negative_step5);
    imwrite(fig, strcat('../images/Final/Original_Negative_', num2str(imageID), '.bmp'));
    close;
    fig = ShowPositiveNegative(positive_step5, negative_step5);
    imwrite(fig, strcat('../images/Final/Positive_Negative_', num2str(imageID), '.bmp'));
    close;
    
    
    % Step 6. Remove holes in positive space
    RemoveHolesInPositive();
    
    negative_step6 = load('out_negative_step6.mat', '-ascii');
    positive_step6 = load('out_positive_step6.mat', '-ascii');
    positive_step6 = bwmorph(positive_step6, 'thin', 50);
    fig = ShowOriginalNegative(original_image, negative_step6);
    imwrite(fig, strcat('../images/Final/Original_Negative_', num2str(imageID), '.bmp'));
    close;
    fig = ShowPositiveNegative(positive_step6, negative_step6);
    imwrite(fig, strcat('../images/Final/Positive_Negative_', num2str(imageID), '.bmp'));
    close;
    
    
    ShowAll(original_image, negative_step6, positive_step6);
    
    
end 
