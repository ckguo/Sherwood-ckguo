nii = loadNii('../../dataset_full/training_axial_full_resampled_pat0.nii.gz');
img = nii.img;
s = size(img);
box_sizes = [[3,3,3];[5,5,5]];
half_box_sizes = (box_sizes-1)/2;
a = 1; % 2a+1 is the side length of the cube around the pixel
offsets = [[3,3,3]; [-3,3,3]; [3,-3,3]; [3,3,-3]; [-3,-3,3]; [3,-3,-3]; [-3,3,-3]; [-3,-3,-3]];
fileID = fopen('test.txt','w');
features = zeros(s(1)*s(2)*s(3), size(offsets,1)*size(box_sizes,1));
pad_size = [7,7,7];
pad = padarray(img, pad_size, 'replicate');
for ind1=1:size(box_sizes,1)
    half_box_size = half_box_sizes(ind1,:);
    box_size = box_sizes(ind1,:);
    means = convn(pad, ones(box_size), 'valid')/(box_size(1)*box_size(2)*box_size(3));
    for ind2=1:size(offsets,1)
        offset = offsets(ind2,:);
        a = pad_size - half_box_size + offset + 1;
        b = pad_size - half_box_size - offset;
        c = pad_size - half_box_size + 1;
        d = pad_size - half_box_size;
        diff = means(a(1):end-b(1), a(2):end-b(2), a(3):end-b(3)) - means(c(1):end-d(1), c(2):end-d(2), c(3):end-d(3));
        features(:,(ind1-1)*size(offsets,1) + ind2) = diff(:);
    end
end

format_string = '';
for i=1:size(features,2)-1
    format_string = strcat(format_string, '%4.4f\t ');
end
format_string = strcat(format_string, '%4.4f\n');

fprintf(fileID, format_string, features);
