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

    def test_load_one_word_data(self):
        sexp_str = '(name word)'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'name')
        self.assertEqual(data.values[0], 'word')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, '', 'next token')

    def test_load_many_word_data(self):
        sexp_str = '( name a  b  c  d e   )'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'name')
        self.assertEqual(data.values[0], 'a')
        self.assertEqual(data.values[1], 'b')
        self.assertEqual(data.values[2], 'c')
        self.assertEqual(data.values[3], 'd')
        self.assertEqual(data.values[4], 'e')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, '', 'next token')

    def test_load_empty_string(self):
        with self.assertRaises(SyntaxError):
            SExp.load_inner('     ')

    def test_load_quote_token_data(self):
        sexp_str = '(name "q"  a b  "word()!   " )'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'name')
        self.assertEqual(data.values[0], '"q"')
        self.assertEqual(data.values[1], 'a')
        self.assertEqual(data.values[2], 'b')
        self.assertEqual(data.values[3], '"word()!   "')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, '', 'next token')

    def test_load_nested_data(self):
        sexp_str = '(pad "" np_thru_hole circle (at 9.36 -1.92) (size 3.05 3.05) (drill 3.05) (layers *.Cu *.Mask))'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_str)
        self.assertEqual(data.name, 'pad')
        self.assertEqual(data.values[0], '""')
        self.assertEqual(data.values[1], 'np_thru_hole')
        self.assertEqual(data.values[2], 'circle')
        self.assertIsInstance(data.values[3], SExp)
        self.assertEqual(data.values[3].name, 'at')
        self.assertEqual(data.values[3].values[0], '9.36')
        self.assertEqual(data.values[4].name, 'size')

        self.assertIs(data.values[3].parent, data)

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, '', 'next token')




if __name__ == '__main__':
    unittest.main()
