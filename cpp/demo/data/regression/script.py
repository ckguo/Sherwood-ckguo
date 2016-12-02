r = open('test1.txt', 'r')
w = open('test.txt', 'w')
for line in r:
	w.write('\t'.join(line.split('    ')))
