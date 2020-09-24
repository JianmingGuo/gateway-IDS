import numpy as np
import sys
PAGE_CATEGORIES_PRE = [
  '04f2', '00a0', '0370', '05a2', '0690', '0110', '04b0', '02b0', '05a0', '0165', '0081', '018f', '02a0', '0220', '0080', '01f1', '0120', '0153', '0260', '0545', '04f1', '0043', '059b', '0587', '05e4', '05f0', '04b1', '0329', '0382', '043f', '0510', '02c0', '051a', '0050', '00a1', '0018', '0034', '0044', '0440', '0042', '04f0', '0517', '0164', '0350', '0316'
]

num = np.load("numEn.npy")
id = np.load("idEn.npy")
c = len(num)
d = len(id)
count = 0
# sdrs = np.zeros([c*d,4096], dtype='uint8')
sdrs = {}
for i1 in range(45):
    for i2 in range(3):
        np.random.shuffle(num[i2])
        sdr = np.zeros(1024, dtype='uint8')
        for i3 in range(1024):
            if i3%4:
                sdr[i3] = id[i1][i3/2]
            else:
                sdr[i3] = num[i2][i3/2]
        sdrs[PAGE_CATEGORIES_PRE[i1]+'_'+str(i2)] = sdr
        count += 1
        sys.stdout.write("\rLearned {} Sessions".format(count + 1))
        sys.stdout.flush()
np.save("En.npy", sdrs)

p = np.load("En.npy").item()
pass
