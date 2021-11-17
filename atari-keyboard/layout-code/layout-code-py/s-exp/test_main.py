from main import SExp, SExpParser
import unittest

big_sexp = """
  (module Diode_THT:D_A-405_P7.62mm_Horizontal (layer B.Cu) (tedit 5AE50CD5) (tstamp 5FAA4446)
    (at 26.19375 76.2 90)
    (descr "Diode, A-405 series, Axial, Horizontal, pin pitch=7.62mm, , length*diameter=5.2*2.7mm^2, ,
        http://www.diodes.com/_files/packages/A-405.pdf")
    (tags "Diode A-405 series Axial Horizontal pin pitch 7.62mm  length 5.2mm diameter 2.7mm")
    (path /5FCB41B8)
    (fp_text reference D41 (at 3.81 2.47 90) (layer B.SilkS)
      (effects (font (size 1 1) (thickness 0.15)) (justify mirror))
    )
    (fp_text value CTRL (at 3.81 -2.47 90) (layer B.SilkS)
      (effects (font (size 1 1) (thickness 0.15)) (justify mirror))
    )
    (fp_text user K (at 0 1.9 90) (layer B.SilkS)
      (effects (font (size 1 1) (thickness 0.15)) (justify mirror))
    )
    (fp_text user K (at 0 1.9 90) (layer B.Fab)
      (effects (font (size 1 1) (thickness 0.15)) (justify mirror))
    )
    (fp_text user %R (at 4.2 0 90) (layer B.Fab)
      (effects (font (size 1 1) (thickness 0.15)) (justify mirror))
    )
    (fp_line (start 8.77 1.6) (end -1.15 1.6) (layer B.CrtYd) (width 0.05))
    (fp_line (start 8.77 -1.6) (end 8.77 1.6) (layer B.CrtYd) (width 0.05))
    (fp_line (start -1.15 -1.6) (end 8.77 -1.6) (layer B.CrtYd) (width 0.05))
    (fp_line (start -1.15 1.6) (end -1.15 -1.6) (layer B.CrtYd) (width 0.05))
    (fp_line (start 1.87 1.47) (end 1.87 -1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 2.11 1.47) (end 2.11 -1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.99 1.47) (end 1.99 -1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 6.53 -1.47) (end 6.53 -1.14) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.09 -1.47) (end 6.53 -1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.09 -1.14) (end 1.09 -1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 6.53 1.47) (end 6.53 1.14) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.09 1.47) (end 6.53 1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.09 1.14) (end 1.09 1.47) (layer B.SilkS) (width 0.12))
    (fp_line (start 1.89 1.35) (end 1.89 -1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 2.09 1.35) (end 2.09 -1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 1.99 1.35) (end 1.99 -1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 7.62 0) (end 6.41 0) (layer B.Fab) (width 0.1))
    (fp_line (start 0 0) (end 1.21 0) (layer B.Fab) (width 0.1))
    (fp_line (start 6.41 1.35) (end 1.21 1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 6.41 -1.35) (end 6.41 1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 1.21 -1.35) (end 6.41 -1.35) (layer B.Fab) (width 0.1))
    (fp_line (start 1.21 1.35) (end 1.21 -1.35) (layer B.Fab) (width 0.1))
    (pad 2 thru_hole oval (at 7.62 0 90) (size 1.8 1.8) (drill 0.9) (layers *.Cu *.Mask)
      (net 57 "Net-(D41-Pad2)"))
    (pad 1 thru_hole rect (at 0 0 90) (size 1.8 1.8) (drill 0.9) (layers *.Cu *.Mask)
      (net 5 R5))
    (model ${KISYS3DMOD}/Diode_THT.3dshapes/D_A-405_P7.62mm_Horizontal.wrl
      (at (xyz 0 0 0))
      (scale (xyz 1 1 1))
      (rotate (xyz 0 0 0))
    )
  )
"""


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
        index, data = SExp.load_inner(sexp_list)
        self.assertEqual(data.name, 'name')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, 'A', 'next token')

    def test_load_name_only_close_paren_white_space_skipped(self):
        sexp_str = '(name)  A'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_list)
        self.assertEqual(data.name, 'name')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, 'A', 'next token')

    def test_load_one_word_data(self):
        sexp_str = '(name word)'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_list)
        self.assertEqual(data.name, 'name')
        self.assertEqual(data.values[0], 'word')

        next_token = SExpParser.peek(sexp_list, index)
        self.assertEqual(next_token, '', 'next token')

    def test_load_many_word_data(self):
        sexp_str = '( name a  b  c  d e   )'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_list)
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
            SExp.load_inner(list('     '))

    def test_load_quote_token_data(self):
        sexp_str = '(name "q"  a b  "word()!   " )'
        sexp_list = list(sexp_str)
        index, data = SExp.load_inner(sexp_list)
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
        index, data = SExp.load_inner(sexp_list)
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

    def test_query_load(self):
        module = SExp.load(big_sexp)
        self.assertEqual(module.name, 'module')

    def test_query_star(self):
        module = SExp.load(big_sexp)
        res = module.query('*')
        self.assertIsInstance(res, list)
        self.assertEqual(len(res), 177)
        res2 = all(isinstance(el, SExp) for el in res)
        self.assertTrue(res2)


if __name__ == '__main__':
    unittest.main()
