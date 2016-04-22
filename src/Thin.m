function output = Thin(image)
    image = medfilt2(image);
    image = medfilt2(image);
    
    bw_thin = bwmorph(image, 'thin', 50);
    bw_thin(image < 10) = 0;
    
    output = bw_thin;