from main import SExp, SExpParser
import unittest


class SExpParserTestCase(unittest.TestCase):
    def test_eat_whitespace(self):
        test_string = list("!")

        res = SExpParser.eat_whitespace(test_string, 0)
        self.assertEqual(0, res, "No white space at start of string")

        test_string = list(" !")
        res = SExpParser.eat_whitespace(test_string, 0)
        self.assertEqual(1, res, "One character of white space")

        test_string = list("")
        res = SExpParser.eat_whitespace(test_string, 0)
        self.assertEqual(0, res, "empty string")

        test_string = list("     ")
        res = SExpParser.eat_whitespace(test_string, 0)
        self.assertEqual(len(test_string), res, "string of spaces")

    def test_read_token_with_no_leading_whitespace(self):
        test_string = list("akron")

        index, token = SExpParser.read_token(test_string, 0)
        next_token = SExpParser.peek(test_string, index)
        self.assertEqual('akron', token, 'Token should equal "akron"')
        self.assertEqual(index, 5, 'index should advance')
        self.assertEqual(next_token, '', 'next token should be ""')

    def test_read_token_but_not_close_paren(self):
        test_string = list("akron)")

        index, token = SExpParser.read_token(test_string, 0)
        next_token = SExpParser.peek(test_string, index)
        self.assertEqual('akron', token, 'Token should equal "akron"')
        self.assertEqual(index, 5, 'index should advance')
        self.assertEqual(next_token, ')', 'next token should be ""')


class SExpTestCase(unittest.TestCase):
    def test_something(self):
        self.assertEqual(True, True)

    def test_sexp_does_create(self):
        sexp = SExp()

        self.assertEqual(sexp.name, '')
        self.assertEqual(sexp.values, [])
        self.assertIs(sexp.parent, None)


class SExpLoadTestCase(unittest.TestCase):

    def test_load_name_only(self):
        data = SExp.load('(name)')
        self.assertEqual(data.name, 'name')

    def test_load_name_only_close_paren_skipped(self):
        sexp_str = '(name)  A'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'name')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, 'A', 'next token')

    def test_load_name_only_close_paren_white_space_skipped(self):
        sexp_str = '(name)  A'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'name')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, 'A', 'next token')

    def test_load_empty_string(self):
        with self.assertRaises(SyntaxError):
            SExp.load_inner('     ')


if __name__ == '__main__':
    unittest.main()
