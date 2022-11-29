import unittest
from parser import SParser

def read_file():
    data_file = open("sample_pcb", "r")
    data = data_file.read()
    data_file.close()
    return data

class TestStringMethods(unittest.TestCase):

    def test_upper(self):
        self.assertEqual('foo'.upper(), 'FOO')

    def test_isupper(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

    def test_loadparser(self):
        parser = SParser("     Hello")
        self.assertEqual(parser.idx, 0)

    def test_eatSpace(self):
        parser = SParser("     Hello")
        parser.eatSpace()
        self.assertEqual(parser.idx, 5)
        self.assertEqual(parser.getNextCharacter(), "H")

    def test_eatSpaceWorksIfNoSpaces(self):
        parser = SParser("Hello")
        parser.eatSpace()
        self.assertEqual(parser.idx, 0)
        self.assertEqual(parser.getNextCharacter(), "H")

    def test_eatSpaceIncludesOthers(self):
        parser = SParser("  \r \n \t   Hello")
        parser.eatSpace()
        self.assertEqual(parser.idx, 10)
        self.assertEqual(parser.getNextCharacter(), "H")

    def test_eatNextCharacter(self):
        parser = SParser("12335")
        self.assertEqual(parser.getNextCharacter(), "1")
        self.assertEqual(parser.getNextCharacter(), "2")

    def test_expectCharPass(self):
        parser = SParser("  ()")
        parser.eatSpace()
        parser.expectCharacter('(')
        self.assertEqual(parser.getNextCharacter(), "(")

    def test_getNextToken1(self):
        parser = SParser("(hello)")
        parser.eatNextCharacter()
        token = parser.getNextToken()
        self.assertEqual(token, "hello")

    def test_getNextToken2(self):
        parser = SParser("(  hello   )")
        parser.eatNextCharacter()
        token = parser.getNextToken()
        self.assertEqual(token, "hello")

    def test_getNextToken3(self):
        parser = SParser("(  hello   world )")
        parser.eatNextCharacter()
        token = parser.getNextToken()
        self.assertEqual(token, "hello")
        token = parser.getNextToken()
        self.assertEqual(token, "world")

    def test_getNextTokenNoToken(self):
        parser = SParser("(   )")
        parser.eatNextCharacter()
        token = parser.getNextToken()
        self.assertEqual(token, "")


    def test_getNexTokenQuoted(self):
        parser = SParser("(\"hello world\")")
        parser.eatNextCharacter()
        token = parser.getNextToken()
        self.assertEqual(token, "\"hello world\"")


    def test_readFile(self):
        s = read_file()
        parser = SParser(s)
        parser.expectCharacter('(')
        self.assertEqual(parser.getNextCharacter(), "(")

    def test_peekNextToken(self):
        s = read_file()
        parser = SParser(s)
        parser.eatNextCharacter()
        token = parser.peekNextToken()
        self.assertEqual(token, "kicad_pcb")
        # Make sure the token was not eaten
        token = parser.peekNextToken()
        self.assertEqual(token, "kicad_pcb")

    def test_toArrayReturnArray(self):
        parser = SParser("()")
        a = parser.toArray();
        self.assertTrue(hasattr(a, "__len__"))

    def test_toArrayParsesOneToken(self):
        parser = SParser("(hello)")
        a = parser.toArray();
        self.assertTrue(hasattr(a, "__len__"))
        self.assertEqual(a[0], "hello")

    def test_toArrayParsesMultipleTokens(self):
        parser = SParser("(  hello   world  )")
        a = parser.toArray();
        self.assertTrue(hasattr(a, "__len__"))
        self.assertEqual(a[0], "hello")
        self.assertEqual(a[1], "world")

    def test_toArrayParsesNestedTokens(self):
        parser = SParser("(  (hello)   world  )")
        a = parser.toArray();
        self.assertTrue(hasattr(a, "__len__"))
        self.assertEqual(a[0][0], "hello")
        self.assertEqual(a[1], "world")

    def test_toArrayFinal(self):
        s = read_file()
        parser = SParser(s)
        #parser = SParser("(  (hello)   world  )")

        a = parser.toArray()
        l = parser.arrayToSexp(a)
        print("\r\n".join(l))


if __name__ == '__main__':
    unittest.main()