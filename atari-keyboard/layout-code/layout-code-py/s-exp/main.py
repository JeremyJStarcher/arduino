# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
# import enum
import string
import typing
from typing import List


class SExp:

    def __init__(self, name: str, sexp: List[str], index: int = 0):

        if type(sexp) != list:
            raise Exception("sexp must be a character list")

        self.name: str = name
        self.values: List[typing.Union[SExp, str, float]] = []
        self.parent: typing.Union[SExp, None] = None

    def __str__(self):
        return str(self.__class__) + ' ' + ' ' + self.name

    def __repr__(self):
        return str(self.__class__) + ' ' + ' ' + self.name

    @staticmethod
    def eat_whitespace(sexp: List[str], index: int) -> int:
        while index < len(sexp) and sexp[index] in string.whitespace:
            index += 1

        return index


def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
