box_sizes = [[3,3,3];[5,5,5]];
offsets = [[3,3,3]; [-3,3,3]; [3,-3,3]; [3,3,-3]; [-3,-3,3]; [3,-3,-3]; [-3,3,-3]; [-3,-3,-3]];

for i=0:9
    nii_filename = sprintf('../../dataset_full/training_axial_full_resampled_pat%d.nii.gz', i);
    seg_nii_filename = sprintf('../../labels_full/training_axial_full_resampled_pat%d-label.nii.gz', i);
    features_array = get_features(nii_filename, seg_nii_filename, box_sizes, offsets, sprintf('test%d.txt', i));
end
