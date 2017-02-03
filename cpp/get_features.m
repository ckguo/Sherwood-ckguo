function [ features_array ] = get_features(nii_filename, seg_nii_filename, box_sizes, offsets, output_filename)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
nii = loadNii(nii_filename);
img = nii.img;
seg_nii = loadNii(seg_nii_filename);
seg_img = seg_nii.img;
[~,~,cropArray,~] = boundingBox(seg_img);
bounding_box = [cropArray{1,1}(1), cropArray{1,1}(end), cropArray{1,2}(1), cropArray{1,2}(end), cropArray{1,3}(1), cropArray{1,3}(end)];
s = size(img);
half_box_sizes = (box_sizes-1)/2;
features = zeros(s(1)*s(2)*s(3), size(offsets,1)*size(box_sizes,1));
pad_size = ones(1,3)*(max(abs(offsets(:))) + max(half_box_sizes(:)));
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

indices = transpose(0:size(features,1)-1);
k = floor(indices/(s(1)*s(2)))+1;
residual = indices-(k-1)*s(1)*s(2);
j = floor(residual/s(1))+1;
i = residual - (j-1)*s(1)+1;

features_array = horzcat(features, bounding_box - horzcat(i,i,j,j,k,k));

features_array(~any(features, 2 ), : ) = []; % removes all rows with all zero

fileID = fopen(output_filename,'w');

format_string = '';
for i=1:size(features_array,2)-1
    format_string = strcat(format_string, '%4.4f\t');
end
format_string = strcat(format_string, '%4.4f\n');

fprintf(fileID, format_string, transpose(features_array));
end
