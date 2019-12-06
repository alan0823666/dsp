
#encoding = big5-hkscs
ZY = {}
with open('Big5-ZhuYin.map', 'r', encoding = 'big5-hkscs') as f:
    for line in f:
        data = line.split(' ')
        chi = data[0]   # the Chinese character
        zy_list = data[1].split('/')    # the corresponding list of Zhu-Yin notations
        zy_list[-1] = zy_list[-1][:-1]  # remove '\n' in the last Zhu-Yin notation
           # print(zy_list[0][0])
        for each_zy in zy_list:
            if each_zy[0] in ZY:
                ZY[each_zy[0]].append(chi)
            else:
                ZY[each_zy[0]] = [chi]

for each_key in ZY:
    ZY[each_key] = list(set(ZY[each_key]))
for key in ZY:
    print (key, end=" ")
    for value in ZY[key]:
        print(value, end=" ")
    print("")
    for value in ZY[key]:
        print(value, value)

