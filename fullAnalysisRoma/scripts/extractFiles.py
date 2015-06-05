import re
hand = open('datasets.json')    # catalogue from Flashgg

target = open ('list.list','w')     # output with the list of EOS files to be analyzed

for line in hand:
    line = line.rstrip()
    if re.search('store', line) :
        line = re.sub('                "name": "', '', line)
        line = re.sub('",', '\n', line)
        target.write(line)

target.close()        
