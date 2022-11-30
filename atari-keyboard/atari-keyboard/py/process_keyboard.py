import json
from parser import SParser

layout_json_filename = "/home/jjs/Projects/qmk/qmk_firmware_a8/keyboards/atari_a8/info.json"
pcb_name = "../atari-keyboard.kicad_pcb"

STARTING_INDEX = 201

UNIT = 19.05

X_ORIG = 61
Y_ORIG = 177.75

# How much to move the diodes
DIODE_OFFSET_X = 6.5;
DIODE_OFFSET_Y = 5.52; 


class BoundingBox:
    def __init__(self x1: None, y1: None, x1; None, y2: None):
        if (x1 == None):
            self.x1 = float('inf')
            self.y1 = float('inf')
            self.x2 = float('-inf')
            self.y2 = float('-inf')
        else:
            self.x1 = x1
            self.y1 = y1
            self.x2 = x2
            self.y2 = y2

class KeyInfo:
    key_x = 0
    key_y = 0
    w = 0
    h = 0
    matrix_r = 0
    matrix_c = 0
    designator = None
    label = ""
    skip = True
    diode_x = 0
    diode_y = 0
    hole_x = 0
    hole_y = 0
    boundingBox: None

    def __init__(self):
        self.bbox = BoundingBox();

    def __repr__(self):
        l = []

        l.append('label: ' + str(self.label))
        l.append('designator: ' + str(self.designator))
        l.append('x: ' + str(self.key_x))
        l.append('y: ' + str(self.key_y))
        l.append('w: ' + str(self.w))
        l.append('h: ' + str(self.h))
        l.append('matrix_c: ' + str(self.matrix_c))
        l.append('matrix_r: ' + str(self.matrix_r))

        return "{ " +  ", ".join(l) + " }"


def read_sexp(name):
    with open(name, "r") as f:
        data = f.read()
    return data


def read_json(name):
    with open(name, "r") as f:
        data = f.read()
    d_dict = json.loads(data)
    return d_dict['layouts']['LAYOUT']['layout']

def setdesignators(startingIndex, keys):

    filtered = list(filter(lambda key: not key.skip, keys))

    # Just make sure that "xx" and "yy" are far greater than your grid.
    # Any spots that are unused will be skipped, just like there are no
    # references assigned to empty spots on your schematic grid.

    idx = startingIndex
    for col in range(10):
        for row in range(10):
            
            items = filter(lambda zkey: (zkey.matrix_r == row) and (zkey.matrix_c == col), filtered)

            ll = list(items)
            if (len(ll) > 0):
                key = ll[0]
                key.designator = str(idx)
                idx += 1


def get_layout():
    info = read_json(layout_json_filename)
    keys = []

    for key in info:

        keyInfo = KeyInfo()

        keyInfo.skip = key.get('x') == -1

        keyInfo.key_x =  key.get('x') * UNIT
        keyInfo.key_y =  key.get('y') * UNIT
        keyInfo.w =  key.get('w', 1)
        keyInfo.h =  key.get('h', 1)
        keyInfo.label = key.get("label")

        # The math for figuring out the actual bounding box is off, so manually correct for
        # various key sizes.  (By hand/trial and error)
        xfix = 0

        kw = float(keyInfo.w)
        
        if (kw == 1):
          xfix = 0

        if (kw == 1.25):
          xfix = 0.125
        
        if (kw == 1.75):
          xfix = 0.375
        
        if (kw == 2):
          xfix = .5
        
        if (kw == 2.25):
          xfix = .625
        
        if (kw == 6.25):
          xfix = 2.625

        print(kw, 1,xfix, keyInfo.key_x)

        keyInfo.key_x += (xfix * UNIT)
        print(kw, 1,xfix, keyInfo.key_x)
        print()

        keyInfo.diode_x = keyInfo.key_x + DIODE_OFFSET_X
        keyInfo.diode_y = keyInfo.key_y + DIODE_OFFSET_Y

        keyInfo.hole_x = keyInfo.key_x + 10
        keyInfo.hole_y = keyInfo.key_y + 10

        matrix = key.get('matrix')
        keyInfo.matrix_c = matrix[0]
        keyInfo.matrix_r = matrix[1]

        keyInfo.boundingBox = BoundingBox(keyInfo.x, keyInfo.x, keyInfo.x+(keyInfo.w*UNIT), keyInfo.y+(keyInfo.h*UNIT)

        keys.append(keyInfo)

    setdesignators(STARTING_INDEX, keys)
    return keys

#    bbox = BoundingBox();

#    print( bbox.x1)



def run_it():

    layout = get_layout()

    #print (layout)

    pcb_sexp = read_sexp(pcb_name)
    parser = SParser(pcb_sexp)
    parser.toArray()


    #for i in layout:
    #    print(i)

    

    for item in layout:

        if item.designator == None:
            print("skipping " + item.label)
            continue
        else:
            print("Searching for " + item.label + " " + item.designator)

        parser.setObjectLocation("SW" + item.designator, item.key_x, item.key_y, 0)
        parser.setObjectLocation("D" + item.designator, item.diode_x, item.diode_y, -90)
        parser.setObjectLocation("H" + item.designator, item.hole_x, item.hole_y, 0)


    l = parser.arrayToSexp()
    out = "\r\n".join(l)

    with open(pcb_name, 'w') as f:
        f.write(out)


   
if __name__ == '__main__':
    run_it() 