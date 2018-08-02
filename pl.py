from matplotlib import pyplot as plt
import numpy as np

def getRange(l):
    return int(l[l.find("..")+2:l.find("]")], 16) - int(l[l.find("[")+1:l.find("..")], 16) +1
print(getRange("[0262..0265]  :   00000000000000000000000000000000;"))

s = []
wanted = False
with open("oc2.mif", 'r') as file:
    lines = file.readlines()
    for line in lines:
        if wanted:
            if line.find("..")!=-1:
                for _ in range(getRange(line)):
                    s.append(line[line.rfind(' ')+1:-2])
            else:
                s.append(line[line.rfind(' ')+1:-2])
        if line.find("CONTENT BEGIN")!=-1:
            wanted = True
print(len(s))
s = s[:-261]
"""print(int(s[0][24:32],2))
for i in s:
    print(i, int(i[0:8],2), int(i[8:16],2), int(i[16:24],2), int(i[24:],2))

with open("blue.txt",'w') as f:
    for i in s:
        f.write(i)
        f.write("\t")
        f.write(str(int(i[0:8],2)))
        f.write("\t")
        f.write(str(int(i[8:16],2)))
        f.write("\t")
        f.write(str(int(i[16:24],2)))
        f.write("\t")
        f.write(str(int(i[24:],2)))
        f.write("\n")
"""
counter = 0
r = []
g = []
b = []
for i in s:
    for j in range(4):
        if counter%3==0:
            r.append(int(i[24-8*j:32-8*j],2))
        elif counter%3==1:
            g.append(int(i[24-8*j:32-8*j],2))
        elif counter%3==2:
            b.append(int(i[24-8*j:32-8*j],2))
        counter += 1

r = np.array(r).reshape((88,640))
g = np.array(g).reshape((88,640))
b = np.array(b).reshape((88,640))
print(r.shape, g.shape, b.shape)
rgb = []
rgb.append(r)
rgb.append(g)
rgb.append(b)
rgb = np.array(rgb)
print(rgb.shape)
rgb = np.swapaxes(rgb, 0, 2)
rgb = np.swapaxes(rgb, 1, 0)

print(rgb.shape)
plt.figure(figsize=(12, 12))
plt.axis('off')
plt.imshow(rgb/255)
plt.show()