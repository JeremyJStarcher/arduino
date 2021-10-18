import os
import pprint
import re
import urllib.parse
from typing import Dict, List

pp = pprint.PrettyPrinter(indent=4)

cssPath = "./layout.css"
outPath = './svg'
bgColor = "transparent"
fgColor = "black"

if not os.path.isdir(outPath):
    print('Creating:', outPath)
    os.mkdir(outPath)
else:
    print('Already exists:', outPath)


def get_svg_paths() -> Dict[str, str]:
    # Some basic paths used to make sure we keep the same lines for some of the box drawing shapes.

    up = f'<rect x="3" y="0" width="2" height="4" style="fill:{fgColor}"  />'
    left = f'<rect x="0" y="3" width="5" height="2" style="fill:{fgColor}"  />'
    right = f'<rect x="3" y="3" width="5" height="2" style="fill:{fgColor}"  />'
    down = f'<rect x="3" y="4" width="2" height="4" style="fill:{fgColor}"  />'

    bodies = {
        "ctrl-a": f"""
            {up}
            {down}
            {right}
            """,
        "ctrl-b": f"""
            <rect x="7" y="0" width="2" height="8" style="fill:{fgColor}"  />
            """,
        "ctrl-c": f"""
            {up}
            {left}
            """,
        "ctrl-d": f"""
            {up}
            {down}
            {left}
            """,
        "ctrl-e": f"""
            {down}
            {left}
            """,
        "ctrl-f": f"""
            <line x1="0" y1="8" x2="8" y2="0" style="stroke:{fgColor};stroke-width:2" />
            """,
        "ctrl-g": f"""
            <line x1="0" y1="0" x2="8" y2="8" style="stroke:{fgColor};stroke-width:2" />
            """,
        "ctrl-h": f"""
            <polygon points="0,8 8,8 8,0" style="fill:{fgColor};stroke:{fgColor};stroke-width:.1" />
            """,
        "ctrl-i": f"""
            <rect x="4" y="4" width="4" height="4" style="fill:{fgColor}"  />
            """,
        "ctrl-j": f"""
            <polygon points="0,0 0,8 8,8" style="fill:{fgColor};stroke:{fgColor};stroke-width:.1" />
            """,
        "ctrl-k": f"""
            <rect x="4" y="0" width="4" height="4" style="fill:{fgColor}"  />
            """,
        "ctrl-l": f"""
            <rect x="0" y="0" width="4" height="4" style="fill:{fgColor}"  />
            """,
        "ctrl-m": f"""
            <rect x="0" y="0" width="8" height="2" style="fill:{fgColor}"  />
            """,
        "ctrl-n": f"""
            <rect x="0" y="6" width="8" height="2" style="fill:{fgColor}"  />
            """,
        "ctrl-o": f"""
            <rect x="0" y="4" width="4" height="4" style="fill:{fgColor}"  />
            """,
        "ctrl-p": f"""
            <text name="-1" x="0" y="8"
            font="Arial" rotate="0" horizAnchor="middle" vertAnchor="middle" 
            scale="0.25,0.25" width="0.1" fill="{fgColor}" stroke="{fgColor}">&#x2663;</text>
            """,
        "ctrl-q": f"""
            {down}
            {right}
            """,
        "ctrl-r": f"""
            {left}
            {right}
            """,
        "ctrl-s": f"""
            {up}
            {down}
            {left}
            {right}
            """,
        "ctrl-t": f"""
            <circle cx="4" cy="4" r="3" style="fill:{fgColor}" />
            """,
        "ctrl-u": f"""
            <rect x="0" y="4" width="8" height="4" style="fill:{fgColor}"  />
            """,
        "ctrl-v": f"""
            <rect x="0" y="0" width="2" height="8" style="fill:{fgColor}"  />
            """,
        "ctrl-w": f"""
            {down}
            {left}
            {right}
        """,
        "ctrl-x": f"""
            {up}
            {left}
            {right}
            """,
        "ctrl-y": f"""
            <rect x="0" y="0" width="4" height="8" style="fill:{fgColor}"  />
            """,
        "ctrl-z": f"""
            {up}
            {right}
            """,
        "ctrl-comma": f"""
            <text name="-1" x=".5" y="8"
            font="Arial" rotate="0" horizAnchor="middle" vertAnchor="middle" 
            scale="0.25,0.25" width="0.1" fill="{fgColor}" stroke="{fgColor}">&#9829;</text>
            """,
        "ctrl-period": f"""
            <text name="-1" x="2" y="6.5"
            font="Arial" rotate="0" horizAnchor="middle" vertAnchor="middle" 
            scale="0.05,0.05" width="0.01" font-size="8px" fill="{fgColor}" stroke="{fgColor}">&#9830;</text>
            """,
        "ctrl-semi": f"""
            <text name="-1" x="2" y="6.5"
            font="Arial" rotate="0" horizAnchor="middle" vertAnchor="middle" 
            scale="0.05,0.05" width="0.01" font-size="8px" fill="{fgColor}"
            stroke="{fgColor} stroke-width:0.01
            ">&#9824;</text>
            """,
    }
    return bodies


def normalize_svg(svg: str) -> str:
    svg = "             " + svg
    # No leading spaces
    out = svg.strip()
    # No multiple spaces in inside the string
    out = re.sub(r'\s+', " ", out)
    # Normalize the quotes to be single quotes
    out = re.sub(r'"', '\'', out)
    # Don't need a space between opening and closing tags
    out = re.sub(r"> <", "><", out)
    # Or before the closing tag itself
    out = re.sub(r" />", "/>", out)
    return out


def wrap_svg_path(s: str) -> str:
    border_color = "transparent"

    svg_fill = f"""<rect x="0" y="0" width="8" height="8" 
               style="fill:{bgColor};stroke-width:1;stroke:{border_color}"  />"""

    s = f"""<svg xmlns='http://www.w3.org/2000/svg' width="8" height="8">
    {svg_fill}
    {s}
    </svg>"""

    return s


def html_encode(s: str) -> str:
    to_convert: Dict[str, str] = {
        "\r": "%0D",
        "\n": "%0A",
        "%": "%25",
        "#": "#",
        "<": "%3C",
        ">": "%3E",
        "?": "?",
        "[": "%5B",
        "]": "%5D",
        "^": "%5E",
        "`": "%60",
        "{": "%7B",
        "|": "%7C",
        "}": "%7D",
    }

    for c, c2 in to_convert.items():
        s = s.replace(c, c2)

    return s

def make_svgs() -> None:
    paths = get_svg_paths()
    for key, path in paths.items():
        full_svg = wrap_svg_path(path)
        normalized = normalize_svg(full_svg)
        pp.pprint(normalized)
        with open(f"{outPath}/{key}.svg", 'w') as f:
            f.write(normalized)


def make_css() -> None:
    t = """.at-ascii {
  display: inline-block;
  transform: translate(0, 0);
  border: solid 1px;
  
    border: 1px solid red;
    min-height: 20px;
    width: 10px;
}""".splitlines(False)

    out: List[str] = t

    paths = get_svg_paths()
    for key, path in paths.items():
        full_svg = wrap_svg_path(path)
        normalized = normalize_svg(full_svg)
        encoded = normalized # html_encode(normalized)
        out.append(f'.at-ascii-{key}:after {{content: url("data:image/svg+xml,{encoded}"); }}')

    with open(f"{cssPath}", 'w') as f:
        f.writelines("\r\n".join(out))


make_css()
