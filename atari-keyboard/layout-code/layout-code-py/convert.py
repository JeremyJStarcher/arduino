from typing import Dict
import json
import pprint
import svgwrite
from svgwrite import cm, mm
from sexpdata import loads, dumps

class MetaData:
    keyname: str
    x: int  # The xoffset since the last key
    y: int  # The yoffset since the last key
    w: int  # The width of the key
    y: int  # The height of the key
    pos_x: int  # The absolute 'x' position
    pos_y: int  # The absolute 'y' position

    def __init__(self):
        self.keyname = '???'
        self.x = 0
        self.y = 0
        self.h = 1
        self.w = 1
        self.pos_x = -1
        self.pos_y = -1

    def __str__(self):
        return str(self.__class__) + ' ' + ' '.join(str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))

    def __repr__(self):
        return str(self.__class__) + ' ' + ' '.join(str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__))


def convert_json_to_full_meta(data):
    keylist = []
    metaData = MetaData()

    for row in data:
        if (type(row) == str):
            pass
            # This is some kind of header metadata.
            # We are going to skip it for now
        else:
            keylist.append([])
            for key in row:
                if (type(key) == str):
                    metaData.keyname = key.split('\n')
                    keylist[-1].append(metaData)
                    # Get fresh meta data in case the next line is also a string
                    metaData = MetaData()
                else:
                    metaData = MetaData()
                    vv = key.get('x')
                    if (vv):
                        metaData.x = vv

                    vv = key.get('y')
                    if (vv):
                        metaData.y = vv

                    vv = key.get('h')
                    if (vv):
                        metaData.h = vv

                    vv = key.get('w')
                    if (vv):
                        metaData.w = vv
    return keylist


def calculate_absolute_position(data):
    pos_x = 0
    pos_y = 0

    for row in data:
        pos_x = 0
        max_y = 0

        for key in row:
            max_y = max(max_y, key.y)
            pos_x += key.x
            pos_y += key.y

            key.pos_x = pos_x
            key.pos_y = pos_y

            pos_x += key.w
        pos_y += 1


def render_svg(data, name: str):
    KEY_SIZE = 19.04

    dwg = svgwrite.Drawing(filename=name, debug=True)
    shapes = dwg.add(dwg.g(id='shapes', fill='red'))

    text = dwg.add(dwg.g(font_size=(KEY_SIZE)/4*mm, fill='black'))

    for row in data:
        for key in row:
            x = key.pos_x*KEY_SIZE
            y = key.pos_y*KEY_SIZE
            w = key.w*KEY_SIZE
            h = key.h*KEY_SIZE

            shapes.add(dwg.rect(insert=(x*mm, y*mm), size=(w*mm, h*mm),
                                fill='white', stroke='gray', stroke_width=1*mm))

            atext = dwg.text(key.keyname[0],
                             insert=((x + w/2)*mm, (y+h/2)*mm),
                             style="text-shadow: 2px 2px;",
                             text_anchor="middle",
                             dominant_baseline="middle",
                             )

            text.add(atext)

    dwg.save()


with open('./layout-2.json') as f:
    data = json.load(f)

    full_list = convert_json_to_full_meta(data)
    calculate_absolute_position(full_list)
    render_svg(full_list, "layout.svg")

    #    pp = pprint.PrettyPrinter(indent = 4)
    #    pp.pprint(full_list)

    # print(data)
