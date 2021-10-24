from main import SExp
import unittest


class SExpTestCase(unittest.TestCase):
    def test_something(self):
        self.assertEqual(True, True)

    def test_sexp_does_create(self):
        sexp = SExp('root', list('(test)'))

        self.assertEqual(sexp.name, 'root')
        self.assertEqual(sexp.values, [])
        self.assertIs(sexp.parent, None)

    def test_eat_whitespace(self):
        test_string = list("!")

        res = SExp.eat_whitespace(test_string, 0)
        self.assertEqual(0, res, "No white space at start of string")

        test_string = list(" !")
        res = SExp.eat_whitespace(test_string, 0)
        self.assertEqual(1, res, "One character of white space")

        test_string = list("")
        res = SExp.eat_whitespace(test_string, 0)
        self.assertEqual(0, res, "empty string")

        test_string = list("     ")
        res = SExp.eat_whitespace(test_string, 0)
        self.assertEqual(len(test_string), res, "string of spaces")


if __name__ == '__main__':
    unittest.main()
