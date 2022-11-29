WHITESPACE = " \n\r\t"
TOKEN_ENDER = WHITESPACE + ")"

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

    def findObjectsByNoun(self, noun, maxDepth):
        return self.findObjectsByNounInner([self.arr], noun, maxDepth, 0, [])

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
