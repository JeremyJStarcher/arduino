from enum import Enum

WHITESPACE = " \n\r\t"
TOKEN_ENDER = WHITESPACE + ")"


class Layer(str, Enum):
    F_CU = "\"F.Cu\""
    B_CU = "\"B.Cu\""
    B_Adhesive = "\"B.Adhes\""
    F_Adhesive = "\"F.Adhes\""
    B_Paste = "\"B.Paste\""
    F_Paste = "\"F.Paste\""
    B_Silkscreen = "\"B.SilkS\""
    F_Silkscreen = "\"F.SilkS\""
    B_Mask = "\"B.Mask\""
    F_Mask = "\"F.Mask\""
    User_Drawings = "\"Dwgs.User\""
    User_Comments = "\"Cmts.User\""
    User_Eco1 = "\"Eco1.User\""
    User_Eco2 = "\"Eco2.User\""
    Edge_Cuts = "\"Edge.Cuts\""
    Margin = "\"Margin\""
    B_Courtyard = "\"B.CrtYd\""
    F_Courtyard = "\"F.CrtYd\""
    B_Fab = "\"B.Fab\""
    F_Fab = "\"F.Fab\""
   

class BoundingBox:
    def __init__(self, x1: None, y1: None, x2: None, y2: None):
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

    def update_xy(self, sx, sy):
        x = float(sx)
        y = float(sy)

        self.x1 = min(self.x1, x)
        self.y1 = min(self.y1, y)
        self.x2 = max(self.x2, x)
        self.y2 = max(self.y2, y)

    def __repr__(self):
        l = []

        l.append('x1: ' + str(self.x1))
        l.append('y1: ' + str(self.y1))
        l.append('x2: ' + str(self.x2))
        l.append('y2: ' + str(self.y2))
     
        return "{ " +  ", ".join(l) + " }"



class SParser:
    str = ""
    idx = 0
    arr = []

    def __init__(self, str):
        self.str = str

    def eatSpace(self):
        while self.str[self.idx] in WHITESPACE:
            self.eatNextCharacter()

    def peekNextCharacter(self):
        return self.str[self.idx]

    def getNextCharacter(self):
        ch = self.str[self.idx]
        self.eatNextCharacter()
        return ch

    def eatNextCharacter(self):
        self.idx += 1

    def expectCharacter(self, ch):
            if self.str[self.idx] != ch:
                raise Exception("Expected " + ch + " found " + self.str[self.idx])

    def getNextToken(self):
        token = ""
        self.eatSpace()
        quote = "\""

        if (self.peekNextCharacter() == quote):
            self.eatNextCharacter()
            while self.str[self.idx] != quote:
                token += self.getNextCharacter()
            self.eatNextCharacter()
            return quote + token + quote

        else:
            while not self.str[self.idx] in TOKEN_ENDER:
                token += self.getNextCharacter()
            self.eatSpace()
            return token

    def getNextTokenQuoted(self):
        token = ""
        self.eatSpace()
        while not self.str[self.idx] in TOKEN_ENDER:
            token += self.getNextCharacter()
        self.eatSpace()
        return token

    def peekNextToken(self):
        saveidx = self.idx
        token = self.getNextToken()
        self.idx = saveidx
        return token

    def toArray(self):
        ret = []

        self.eatSpace()
        self.expectCharacter('(')
        self.eatNextCharacter();

        while True:
            self.eatSpace()
            ch = self.peekNextCharacter()
            if (ch == ")"):
                self.eatNextCharacter()
                self.arr = ret
                return ret

            if (ch == "("):
                array = self.toArray()
                ret.append(array)     
                
            if (ch != '(' and ch != ')'):
                token = self.getNextToken()
                ret.append(token)

    def printArray(self, array, depth = 0):

        indent = " " * 2 * depth

        for e in array:
            if isinstance(e, list):
                print(indent + "(")
                self.printArray(e, depth+1)
                print(indent + ")")
            else:
                print(indent + e)


    def arrayToSexp(self):
        return self.arrayToSexpInner([self.arr], 0, [])

    def arrayToSexpInner(self, array, depth = 0, out=[]):

        indent = " " * 2 * depth

        for e in array:
            if isinstance(e, list):
                out.append(indent + "(")
                self.arrayToSexpInner(e, depth+1, out)
                out.append(indent + ")")
            elif isinstance(e, float):
                out.append(indent + str(e))
            elif isinstance(e, int):
                out.append(indent + str(e))
            else:
                out.append(indent + e)
        return out

    def findObjectsByNoun(self, noun, maxDepth = float("inf"), root = None):
        if root == None:
            root = self.arr
        
        return self.findObjectsByNounInner([root], noun, maxDepth, 0, [])

    def findObjectsByNounInner(self, array, noun, maxDepth, depth, out):
        for e in array:
            if isinstance(e, list):
                if (len(e) > 0) and (e[0] == noun):
                    out.append(e)

                if (depth < maxDepth):
                    self.findObjectsByNounInner(e, noun, maxDepth, depth+1, out)
            else:
                pass
        return out

    def findFootprintByReference(self, ref):
        prints = self.findObjectsByNoun("footprint", 1)

        for p in prints:
            #print(p)

            o = []
            self.findObjectsByNounInner(p, "fp_text", 100, 0, o)
            filtered = filter(lambda fp: (fp[1] == "reference") and (fp[2] == "\"" + ref + "\""), o)
            
            lst = list(filtered)
            if (len(lst) > 0):
                return p

    def findAtByReference(self, ref):
        footprint = self.findFootprintByReference(ref)

        #print(footprint)
        o = []
        self.findObjectsByNounInner(footprint, "at", 0, 0, o)

        if (len(o) > 1):
            raise Exception("Found too many top-level ATs for" + ref)

        return o[0]

    def setObjectLocation(self, ref, x, y, rot = 0):
        footprint = self.findFootprintByReference(ref)
        
        all_ats = []
        self.findObjectsByNounInner(footprint, "at", 10_000, 0, all_ats)
        for at1 in all_ats:
            print("Before, at", at1)
            at1.append(0)         # If there isn't a rotation, add it.

            at1_x = at1[1]
            at1_y = at1[2]
            at1_rot = at1[3]


            while (len(at1) > 0):
                at1.pop()
            
            at1.append("at")
            at1.append(at1_x)
            at1.append(at1_y)
            at1.append(int(at1_rot) + rot)
            print("After at", at1)


        # Set the primary location and rotation
        at = self.findAtByReference(ref)
        if at != None:
            while (len(at) > 0):
                at.pop()
            
            at.append("at")
            at.append(x)
            at.append(y)
            at.append(rot)
            
    def addBoundingBox(self, box, width):
        box = ['gr_rect', \
                    ['start', box.x1, box.y1], \
                    ['end',  box.x2, box.y2], \
                    ['layer',  "F.SilkS"], \
                    ["width",  width], \
                    ["fill", "none"] \
                ]
             
        self.arr.append(box)
        #print(self.arr)


    def getBoundingBoxOfLayerLines(self, parent, layerName):
        lines = []

        g_lines = self.findObjectsByNoun("fp_line", float("inf"), parent)
        at = self.findObjectsByNoun("at", 1, parent)

        origin_x = float(at[0][1])
        origin_y = float(at[0][2])
        

        for g_line in g_lines:
            layers = self.findObjectsByNoun("layer", float("inf"), g_line)
            for layer in layers:
                if layer[1] == layerName:
                    lines.append(g_line)

        box = BoundingBox(None, None, None, None)
        for line in lines:
            start = self.findObjectsByNoun("start", float("inf"), line)
            end = self.findObjectsByNoun("end", float("inf"), line)


            x1 = float(start[0][1])
            y1 = float(start[0][2])
            x2 = float(end[0][1])
            y2 = float(end[0][2])

            box.update_xy(x1 + origin_x, y1 + origin_y)
            box.update_xy(x2 + origin_x, y2 + origin_y)

        return box

    