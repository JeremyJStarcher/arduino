import json
from parser import SParser

layout_json_filename = "/home/jjs/Projects/qmk/qmk_firmware_a8/keyboards/atari_a8/info.json"
pcb_name = "../atari-keyboard.kicad_pcb"

STARTING_INDEX = 201

X_ORIG = 61
Y_ORIG = 177.75

# How much to move the diodes
DIODE_OFFSET_X = 6.5;
DIODE_OFFSET_Y = 5.52; 


class BoundingBox:
    def __init__(self):
        self.x1 = float('inf')
        self.y1 = float('inf')
        self.x2 = float('-inf')
        self.y2 = float('-inf')

class KeyInfo:
    x = 0
    y = 0
    w = 0
    h = 0
    matrix_r = 0
    matrix_c = 0
    designator = "--"
    label = ""

    def __init__(self):
        self.bbox = BoundingBox();

    def __repr__(self):
        l = []

        l.append('label: ' + str(self.label))
        l.append('designator: ' + str(self.designator))
        l.append('x: ' + str(self.x))
        l.append('y: ' + str(self.y))
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

    filtered = list(filter(lambda key: key.x != -1, keys))

    # Just make sure that "xx" and "yy" are far greater than your grid.
    # Any spots that are unused will be skipped, just like there are no
    # references assigned to empty spots on your schematic grid.

    idx = startingIndex
    for row in range(10):
        for col in range(10):
            
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

        keyInfo.x =  key.get('x')
        keyInfo.y =  key.get('y')
        keyInfo.w =  key.get('w', 1)
        keyInfo.h =  key.get('h', 1)
        keyInfo.label = key.get("label")

        matrix = key.get('matrix')
        keyInfo.matrix_c = matrix[0]
        keyInfo.matrix_r = matrix[1]

        keys.append(keyInfo)

    setdesignators(STARTING_INDEX, keys)
    return keys

#    bbox = BoundingBox();

#    print( bbox.x1)



def run_it():

    layout = get_layout()

    print (layout)

    pcb_sexp = read_sexp(pcb_name)
    parser = SParser(pcb_sexp)
    a = parser.toArray()
    l = parser.arrayToSexp([a])
    out = "\r\n".join(l)

    with open(pcb_name, 'w') as f:
        f.write(out)


   
if __name__ == '__main__':
    run_it() 