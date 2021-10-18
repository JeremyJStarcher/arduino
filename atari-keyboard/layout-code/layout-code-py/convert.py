import json
from typing import List

import svgwrite
from svgwrite import mm

layout_file = "keyboard-layout.json"

# from sexpdata import loads, dumps

# Keyboard placement aliases
K_TL = 0  # top left
K_TM = 1  # top middle
K_TR = 2  # tp right

K_CL = 3  # center line left
K_CM = 4  # center line middle
K_CR = 5  # center line right

K_BL = 6  # bottom left
K_BM = 7  # bottom middle
K_BR = 8  # bottom right

K_FL = 9  # front left
K_FM = 10  # front middle
K_FR = 11  # front right

K_BLANK = -1  # not used


class KeyDetail:

    def __init__(self):
        self.a: int = -1  # Which magic alignment grid to use

        self.key_names = ['???']
        self.x = 0.  # The offset from the last key (0) if right next to
        self.y = 0.0
        self.h = 1.0
        self.w = 1.0
        self.pos_x = -1.0
        self.pos_y = -1.0
        self.key_legends: List[str] = []
        self.cannon_name: str = ""

    def __str__(self):
        return str(self.__class__) + ' ' + ' '.join(
            str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))

    def __repr__(self):
        return str(self.__class__) + ' ' + ' '.join(
            str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))


def convert_json_to_full_meta(full_data) -> List[List[KeyDetail]]:
    key_list: List[List[KeyDetail]]
    # noinspection PyTypeChecker
    key_list = []

    key_list.clear()

    meta_data = KeyDetail()
    alignment = 4

    for row in full_data:
        if type(row) == dict:
            pass
        elif type(row) == str:
            pass
            # This is some kind of header metadata.
            #   We are going to skip it for now
        else:
            key_list.append([])
            for key in row:
                if type(key) == str:
                    meta_data.key_names = key.split('\n')
                    meta_data.a = alignment

                    key_list[-1].append(meta_data)
                    # Get fresh meta data in case the next line is also a string
                    meta_data = KeyDetail()
                else:
                    meta_data = KeyDetail()
                    vv = key.get('x')
                    if vv:
                        meta_data.x = vv

                    vv = key.get('y')
                    if vv:
                        meta_data.y = vv

                    vv = key.get('h')
                    if vv:
                        meta_data.h = vv

                    vv = key.get('w')
                    if vv:
                        meta_data.w = vv

                    vv = key.get('a')
                    if vv:
                        alignment = vv

    return key_list


def calculate_absolute_position(full_data: List[List[KeyDetail]]):
    pos_y: float = 0

    for row in full_data:
        pos_x: float = 0
        max_y: float = 0

        for key in row:
            max_y = max(max_y, key.y)
            pos_x += key.x
            pos_y += key.y

            key.pos_x = pos_x
            key.pos_y = pos_y

            pos_x += key.w
        pos_y += 1


def render_svg(full_data: List[List[KeyDetail]], name: str):
    key_size: float = 19.04

    dwg = svgwrite.Drawing(filename=name, debug=True)
    shapes = dwg.add(dwg.g(id='shapes', fill='red'))

    text = dwg.add(dwg.g(font_size=key_size / 4 * mm, fill='black'))

    for row in full_data:
        for key in row:
            x = key.pos_x * key_size
            y = key.pos_y * key_size
            w = key.w * key_size
            h = key.h * key_size

            shapes.add(dwg.rect(insert=(x * mm, y * mm), size=(w * mm, h * mm),
                                fill='white', stroke='gray', stroke_width=1 * mm))

            a_text = dwg.text(key.cannon_name,
                              insert=((x + w / 2) * mm, (y + h / 2) * mm),
                              style="text-shadow: 2px 2px;",
                              text_anchor="middle",
                              dominant_baseline="middle",
                              )

            text.add(a_text)

    dwg.save()


def standardize_keymaps(full_data: List[List[KeyDetail]]):
    layout_grid = [
        [  # 0
            0, 8, 2,
            6, 9, 7,
            1, 10, 3,
            4, 11, 5,
        ],
        [  # 1
            -1, 0, -1,
            -1, 6, -1,
            -1, 1, -1,
            4, 11, 5,
        ],
        [  # 2
            -1, -1, 1,
            0, -1, 2,
            -1, -1, -1,
            4, 11, 5,
        ],
        [  # 3
            -1, -1, -1,
            -1, 0, -1,
            -1, -1, -1,
            4, 11, 5
        ],
        [  # 4
            0, 8, 2,
            6, 9, 7,
            1, 10, 3,
            -1, 4, -1,
        ],
        [  # 5
            -1, 0, -1,
            -1, 6, -1,
            -1, 1, -1,
            -1, 4, -1,
        ],
        [  # 6
            -1, -1, -1,
            0, 8, 2,
            -1, -1, -1,
            -1, 4, -1
        ], [  # 7
            -1, -1, -1,
            -1, 0, -1,
            -1, -1, -1,
            -1, 4, -1,
        ]
    ]

    for row in full_data:
        for key in row:
            # print(key)
            xlate = layout_grid[key.a]
            # print(xlate)

            print(xlate)
            for i in range(len(xlate)):
                key.key_legends.append("")

                print("&&&", "i=", i, "xlate[i]=",  xlate[i], "key.a=", key.a, key.key_names, len(key.key_names))
                if xlate[i] != -1 and len(key.key_names) >= xlate[i]:
                    label = key.key_names[xlate[i]]
                    key.key_legends[i] = label

            key.cannon_name = key.key_legends[K_CM] or key.key_legends[K_BM]

            print(key.key_legends)


with open(layout_file) as f:
    data = json.load(f)

    full_list: List[List[KeyDetail]] = convert_json_to_full_meta(data)
    calculate_absolute_position(full_list)

    standardize_keymaps(full_list)

    render_svg(full_list, "layout.svg")

    #    pp = pprint.PrettyPrinter(indent = 4)
    #    pp.pprint(full_list)

    # print(data)
