w = open('test_1.txt', 'w')
for i in range(0,340,3):
    for j in range(0,340,3):
        for k in range(0,133,3):
            # pretend box is 100-240, 100-240, 30-103
            vals = [0, i, j, k, 100-i, 240-i, 100-j, 240-j, 30-k, 103-k]
            w.write('\t'.join([str(x) for x in vals]) + '\n')
w.write('\n')
