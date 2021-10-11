from typing import Dict
import json
import pprint

class MetaData:
    keyname: str
    x: int  # The xoffset since the last key
    y: int  # The yoffset since the last key
    w: int  # The width of the key
    y: int  # The height of the key
    pos_x: int  # The absolute 'x' position
    pos_y: int  # The absolute 'y' position

    def __init__(self):
        self.keyname = '???'
        self.x = 0
        self.y = 0
        self.h = 1
        self.w = 1
        self.pos_x = -1
        self.pos_y = -1

    def __str__(self):
        return str(self.__class__) + ' ' + ' '.join(str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))

    def __repr__(self):
        return str(self.__class__) + ' ' + ' '.join(str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))

def convert_json_to_full_meta(data):
    keylist = []
    metaData = MetaData()
    
    for row in data:
        if (type(row) == str):
            pass
            # This is some kind of header metadata.
            # We are going to skip it for now
        else:
            keylist.append([])
            for key in row:
                if (type(key) == str):
                    metaData.keyname = key.split('\n')
                    keylist[-1].append(metaData);
                    metaData = MetaData()
                else:
                    metaData = MetaData()
                    # print("m")
                    # print(key.values())
                    vv = key.get('x')
                    if (vv):
                        metaData.x = vv
                        
                    vv = key.get('y')
                    if (vv):
                        metaData.y = vv
              
                    vv = key.get('h')
                    if (vv):
                        metaData.h = vv
                 
                    vv = key.get('w')
                    if (vv):
                        metaData.w = vv
                        
                    # print(metaData)

    # print(keylist)
    return keylist  

def calculate_absolute_position(data):
    pos_x = 0
    pos_y = 0
  
    for row in data:
        pos_x = 0
        max_y = 0

        for key in row:
            max_y = max(max_y, key.y)
            pos_x += key.x
            pos_y += key.y

            key.pos_x = pos_x
            key.pos_y = pos_y
            
            pos_x += key.w
        pos_y += 1

with open('./layout-2.json') as f:
    data  = json.load(f)

    full_list = convert_json_to_full_meta(data)
    calculate_absolute_position(full_list)

    pp = pprint.PrettyPrinter(indent = 4)
    pp.pprint(full_list)

    # print(data)
