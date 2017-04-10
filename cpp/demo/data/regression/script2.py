w = open('test_1.txt', 'w')
dimx = 340
dimy = 340
dimz = 133
matlab_bbox = [1, 123, 122,220, 129, 296]
matlab_bbox = [4, 115, 123, 215, 102, 260]
s_bbox = [dimx-matlab_bbox[3], dimx-matlab_bbox[2], dimy-matlab_bbox[5], dimy-matlab_bbox[4], matlab_bbox[0]-1, matlab_bbox[1]-1]
print s_bbox
for i in range(0,340,7):
    for j in range(0,340,7):
        for k in range(0,133,3):
            vals = [0, i, j, k, s_bbox[0]-i, s_bbox[1]-i, s_bbox[2]-j, s_bbox[3]-j, s_bbox[4]-k, s_bbox[5]-k]
            w.write('\t'.join([str(x) for x in vals]) + '\n')
w.close()
