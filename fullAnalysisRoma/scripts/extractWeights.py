import re
hand = open('datasets.json')            # catalogue from Flashgg

target = open ('weights.list','w')   # output with the list of EOS files to be analyzed

for line in hand:
    line = line.rstrip()
    if re.search('weights', line) :
        line = re.sub('                "weights": ', '', line)
        target.write(line)
        target.write('\n')

target.close()        
