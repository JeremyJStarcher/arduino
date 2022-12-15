import json
from parser import SParser
from parser import BoundingBox
from parser import Layer

layout_json_filename = "/home/jjs/Projects/qmk/qmk_firmware_a8/keyboards/atari_a8/info.json"
pcb_name = "../atari-keyboard.kicad_pcb"
key_sch_name = "../keyboard.kicad_sch"
openscad_file = "../../keycaps/keyboard-position.scad"

STARTING_INDEX = 201

UNIT = 19.05

X_ORIG = 61
Y_ORIG = 177.75
BOARD_BORDER = 10

# How much to move the diodes
DIODE_OFFSET_X = 6.5
DIODE_OFFSET_Y = 5.52 


class KeyInfo:
    # Logoical position, based on key units
    l_x = 0
    l_y = 0

    # Absolute position
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
        self.bbox = BoundingBox(float('inf'), float('inf'), float('-inf'), float('-inf'));

    def __repr__(self):
        l = []

        l.append('label: ' + str(self.label))
        l.append('designator: ' + str(self.designator))
        l.append('x: ' + str(self.key_x))
        l.append('y: ' + str(self.key_y))
        l.append('w: ' + str(self.w))
        l.append('h: ' + str(self.h))

        l.append('l_x: ' + str(self.l_x))
        l.append('l_y: ' + str(self.l_y))

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

    KEYSWITCH_FIX_X = -11.565
    KEYSWITCH_FIX_Y = -3.946


    for key in info:

        keyInfo = KeyInfo()

        keyInfo.skip = key.get('x') == -1
        keyInfo.label = key.get("label")

        keyInfo.w =  float(key.get('w', 1))
        keyInfo.h =  float(key.get('h', 1))        
        
        xfix = -1
        if (keyInfo.w == 1.0):
            #print("**1.0**")
            xfix = 0

        if (keyInfo.w == 1.5):
            #print("**1.5**")
            xfix = 0

        if (keyInfo.w == 1.25):
            #print("**1.25**")
            xfix = 0.125
        
        if (keyInfo.w == 1.75):
            #print("**1.75**")
            xfix = 0.375
        
        if (keyInfo.w == 2):
            #print("**2.0**")
            xfix = .5
        
        if (keyInfo.w == 2.25):
            #print("**2.25**")
            xfix = .625
        
        if (keyInfo.w == 6.25):
            #print("**6.250**")
            xfix = 2.625

        if xfix == -1:
            raise Exception("Unknown width of" + str(keyInfo.w) + " found " + keyInfo.label)


        keyInfo.l_x = key.get('x')
        keyInfo.l_y = key.get('y')


        x = (float(key.get('x')) + (KeyInfo.w/2)) * UNIT
        y = (float(key.get('y')) + (KeyInfo.h/2)) * UNIT

        x += X_ORIG
        y += Y_ORIG


        ww = (keyInfo.w-1) * UNIT
        hh = (keyInfo.h-1) * UNIT
        # print("keyInfo: w, h", keyInfo.h, keyInfo.w, ww, hh)


        keyInfo.key_x = x + ww/2
        keyInfo.key_y = y + hh/2


        keyInfo.diode_x = keyInfo.key_x + DIODE_OFFSET_X
        keyInfo.diode_y = keyInfo.key_y + DIODE_OFFSET_Y

        # The math for figuring out the actual bounding box is off, so manually correct for
        # various key sizes.  (By hand/trial and error)
     

        
        x1 = (keyInfo.key_x - keyInfo.w/2) - ww/2 + KEYSWITCH_FIX_X
        y1 = (keyInfo.key_y - keyInfo.h/2) - hh/2 + KEYSWITCH_FIX_Y
        x2 = x1 + UNIT + ww
        y2 = y1 + UNIT + hh

        x1 += xfix
    
        keyInfo.hole_x = keyInfo.key_x + 10
        keyInfo.hole_y = keyInfo.key_y + 10

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

    #print (layout)

    pcb_sexp = read_sexp(pcb_name)
    pcb_parser = SParser(pcb_sexp)
    pcb_parser.toArray()


    key_sch_sexp = read_sexp(key_sch_name)
    key_parser = SParser(key_sch_sexp)
    key_parser.toArray()

    # print(key_parser.printArray(key_parser.arr))


    #for i in layout:
    #    print(i)    

    bbox = BoundingBox(None, None, None, None)

    for item in layout:

        if item.designator == None:
            print("skipping " + item.label)
            continue
        else:
            print("Searching for " + item.label + " " + item.designator)

        pcb_parser.setObjectLocation("SW" + item.designator, item.key_x, item.key_y, 0)

        switch = pcb_parser.findFootprintByReference("SW" + item.designator)
        at = pcb_parser.findAtByReference("SW" + item.designator)
        pcb_parser.setHiddenFootprintTextByReference("SW" + item.designator, "value", False)
        pcb_parser.moveTextToLayer("SW" + item.designator, "value", Layer.F_Silkscreen)

        pcb_parser.copyToBackSilkscreen("SW" + item.designator, "value")

        item.boundingBox = pcb_parser.getBoundingBoxOfLayerLines(switch, Layer.User_Drawings)

        pcb_parser.addBoundingBox(item.boundingBox, 0.3, Layer.F_Silkscreen)
        pcb_parser.addBoundingBox(item.boundingBox, 0.3, Layer.B_Silkscreen)

        bbox.update_xy(item.boundingBox.x1, item.boundingBox.y1)
        bbox.update_xy(item.boundingBox.x2, item.boundingBox.y2)

        pcb_parser.setObjectLocation("D" + item.designator, item.diode_x, item.diode_y, -90)
      
        hx = item.boundingBox.x1+0.0 + key_parser.getSymbolPropertyAsFloat("H" + item.designator, "PCB_X", 0)
        hy = item.boundingBox.y1+1.5 + key_parser.getSymbolPropertyAsFloat("H" + item.designator, "PCB_Y", 0)
        pcb_parser.setHiddenFootprintTextByReference("H" + item.designator, "reference", True)

        pcb_parser.setObjectLocation("H" + item.designator, hx, hy, 0)
        pcb_parser.drawKeepoutZone("H" + item.designator)

    bbox.addBorder(BOARD_BORDER)
    pcb_parser.addBoundingBox(bbox, 0.3, Layer.Edge_Cuts)


    bbox.addBorder(-5)
    pcb_parser.setObjectLocation("H101", bbox.x1, bbox.y1, 0)
    pcb_parser.setObjectLocation("H102", bbox.x1, bbox.y2, 0)
    pcb_parser.setObjectLocation("H103", bbox.x2, bbox.y1, 0)
    pcb_parser.setObjectLocation("H104", bbox.x2, bbox.y2, 0)

    l = pcb_parser.arrayToSexp()
    out = "\r\n".join(l)

    with open(pcb_name, 'w') as f:
        f.write(out)


def calcPnP():
    layout = get_layout()

    #print (layout)

    pcb_sexp = read_sexp(pcb_name)
    pcb_parser = SParser(pcb_sexp)
    pcb_parser.toArray()

    for item in layout:

        if item.designator == None:
            print("skipping " + item.label)
            continue
        else:
            print("Searching for " + item.label + " " + item.designator)

   
        diode = pcb_parser.findFootprintByReference("D" + item.designator)
        print(diode)

def makeOpnscad():
    layout = get_layout()
    out = []

    for item in layout:

        if item.designator == None:
            print("skipping " + item.label)
            continue
        else:
            print("Searching for " + item.label + " " + item.designator)
            
            oo = [
                    str(item.l_x), 
                    str(item.l_y),
                    str(item.key_x),
                    str(item.key_y),
                ]

            out.append("KEY_" + item.label + " = [" + ", ".join(oo) +  "];")
   
    #print(out)
    out = "\r\n".join(out)

    with open(openscad_file, 'w') as f:
        f.write(out)



if __name__ == '__main__':
    #run_it()
    #calcPnP()
    makeOpnscad()
