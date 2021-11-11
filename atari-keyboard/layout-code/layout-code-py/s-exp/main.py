# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
# import enum
from __future__ import annotations

import string
import typing
from typing import List

TOKEN_END = list("()")


class SExpParser:
    @staticmethod
    def eat_or_error(sexp: List[str], index: int, target: str) -> int:
        if SExpParser.peek(sexp, index) != target:
            raise SyntaxError(f'A "{target}" was expected in position {index}')

        return index + 1

    @staticmethod
    def eat_whitespace(sexp: List[str], index: int) -> int:
        while index < len(sexp) and sexp[index] in string.whitespace:
            index += 1

        return index

    @staticmethod
    def peek(sexp: List[str], index: int) -> str:
        if index < len(sexp):
            return sexp[index]
        return ""

    @staticmethod
    def read_quoted_token(sexp: List[str], index: int) -> typing.Tuple[int, str]:
        q_token_end = list('"')
        index = SExpParser.eat_or_error(sexp, index, '"')
        token: str = ""

        while index < len(sexp) and not sexp[index] in q_token_end:
            token += sexp[index]
            index += 1

        index = SExpParser.eat_or_error(sexp, index, '"')
        return index, f'"{token}"'

    @staticmethod
    def read_token(sexp: List[str], index: int) -> typing.Tuple[int, str | SExp]:
        if sexp[index] == '"':
            return SExpParser.read_quoted_token(sexp, index)

        if sexp[index] == '(':
            return SExp.load_inner(sexp, index)

        token: str = ""

        while index < len(sexp) \
                and not sexp[index] in TOKEN_END \
                and not sexp[index] in string.whitespace:
            token += sexp[index]
            index += 1

        return index, token


class SExp:

    def __init__(self):
        self.name: str = ""
        self.values: List[typing.Union[SExp, str, float]] = []
        self.parent: typing.Union[SExp, None] = None

    def __str__(self):
        return str(self.__class__) + ' ' + ' ' + self.name

    def __repr__(self):
        return str(self.__class__) + ' ' + ' ' + self.name

    @staticmethod
    def load(sexp_s: str, index: int = 0) -> SExp:
        _index, exp = SExp.load_inner(sexp_s, index)
        return exp

    @staticmethod
    def load_inner(sexp_s: str, index: int = 0) -> typing.Tuple[int, SExp]:
        sexp_l = list(sexp_s)
        out = SExp()

        index = SExpParser.eat_whitespace(sexp_l, index)
        index = SExpParser.eat_or_error(sexp_l, index, '(')
        index = SExpParser.eat_whitespace(sexp_l, index)

        index, name = SExpParser.read_token(sexp_l, index)
        out.name = name

        # Look for something, anything to do until the expression is closed
        while SExpParser.peek(sexp_l, index) != ')':
            index = SExpParser.eat_whitespace(sexp_l, index)
            index, value = SExpParser.read_token(sexp_l, index)
            if isinstance(value, SExp):
                value.parent = out

            out.values.append(value)

        index = SExpParser.eat_or_error(sexp_l, index, ')')
        index = SExpParser.eat_whitespace(sexp_l, index)
        return index, out


def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
