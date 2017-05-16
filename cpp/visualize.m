i = 0;
nii_filename = sprintf('../../dataset_full/training_axial_full_resampled_pat%d.nii.gz', i);

nii = loadNii(nii_filename);
img = nii.img;
bb_img = zeros(size(img));
% bb_img(1:127, 339-222+1:339-120+1, 339-223+1:339-65+1) = 1;
bb_img(1:127, 339-222+1:339-120+1, 339-223+1-2:339-223+1) = 1;
bb_img(1:127, 339-222+1:339-120+1, 339-65+1:339-65+1+2) = 1;
bb_img(1:3, 339-222+1:339-120+1, 339-223+1:339-65+1) = 1;
bb_img(127:127+2, 339-222+1:339-120+1, 339-223+1:339-65+1) = 1;
bb_img(1:127, 339-222+1-2:339-222+1, 339-223+1:339-65+1) = 1;
bb_img(1:127, 339-120+1:339-120+1+2, 339-223+1:339-65+1) = 1;

bb_nii_filename = sprintf('../training_axial_full_resampled_pat%d_bb_label.nii.gz', i);
saveNii(make_nii(bb_img), bb_nii_filename);
% bb_img_truth = zeros(size(img));
% bb_img_truth(1:123, 339-218+1:339-120+1, 339-211+1:339-44+1) = 1;
% bb_nii_truth_filename = sprintf('../training_axial_full_resampled_pat%d_bb_label_truth.nii.gz', i);
% saveNii(make_nii(bb_img_truth), bb_nii_truth_filename);