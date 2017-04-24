matlab_bboxes = [[1, 123, 122, 220, 129, 296],
                 [4, 115, 123, 215, 102, 260],
                 [4, 155, 115, 205, 81, 253],
                 [5, 162, 147, 285, 131, 312],
                 [10, 76, 103, 177, 118, 217],
                 [5, 104, 103, 186, 84, 191],
                 [1, 105, 163, 241, 140, 260],
                 [8, 137, 154, 255, 136, 310],
                 [12, 115, 132, 229, 115, 271],
                 [17, 149, 128, 247, 95, 271]]
matlab_dimensions = [[133, 340, 340],
                     [129, 320, 320],
                     [178, 340, 340],
                     [184, 440, 440],
                     [91, 270, 270],
                     [117, 270, 270],
                     [113, 386, 386],
                     [150, 386, 386],
                     [125, 340, 340],
                     [160, 340, 340]]
pat = 0
w = open('pat' + str(pat) + '.txt', 'w')
matlab_bbox = matlab_bboxes[pat]
matlab_dimension = matlab_dimensions[pat]
dimx = matlab_dimension[1]
dimy = matlab_dimension[2]
dimz = matlab_dimension[0]
s_bbox = [dimx-matlab_bbox[3], dimx-matlab_bbox[2], dimy-matlab_bbox[5], dimy-matlab_bbox[4], matlab_bbox[0]-1, matlab_bbox[1]-1]
print s_bbox
for i in range(0,matlab_dimension[1],1):
    for j in range(0,matlab_dimension[2],1):
        for k in range(0,matlab_dimension[0],1):
            vals = [pat, i, j, k, s_bbox[0]-i, s_bbox[1]-i, s_bbox[2]-j, s_bbox[3]-j, s_bbox[4]-k, s_bbox[5]-k]
            w.write('\t'.join([str(x) for x in vals]) + '\n')
w.close()
