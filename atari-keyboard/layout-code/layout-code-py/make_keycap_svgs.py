import os
import pprint
import re
import urllib.parse
from typing import Dict, List

pp = pprint.PrettyPrinter(indent=4)

cssPath = "./layout.css"
outPath = './svg'
bgColor = "transparent"
fgColor = "gray"

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
            <g transform="translate(-1)">
            <g transform="matrix(0.20000002 0 0 0.20000002 1 -0)">
            <path d="M20 0C 15.269 0 11.429 4.032 11.429 9C 11.47 12.125999 13.083 14.768 14.762 17.281C 12.891 15.865 
            10.811 15.009 8.5714 15C 3.8399997 15 0 19.032 0 24C 0 28.968 3.84 33 8.5714 33C 12.403999 32.936 15.469999 
            30.254 18.481998 28C 17.942999 34.733 16.846998 38.514 10.475999 40L10.475999 40L29.523998 40C 23.152998 
            38.514 22.057 34.733 21.517998 28C 24.494997 30.552 27.617998 32.717 31.428997 33C 36.159996 33 39.999996 
            28.968 39.999996 24C 39.999996 19.032 36.159996 15 31.428997 15C 29.131996 15 27.147997 16.057 25.237997 
            17.281C 27.137997 14.794001 28.388996 12.111 28.570997 9C 28.570997 4.032 24.730997 0 19.999996 0L19.999996
             0L20 0z" stroke="none" fill="{fgColor}" fill-rule="nonzero" />
            </g>
            </g>
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
            <g transform="translate(-1)">
            <g transform="matrix(0.011111112 0 0 0.011111112 1 -0)">
            <path d="M180 45C 80.64 45 0 125.64 0 225C 0 272.8 18.66 316.26 49.094 348.5L49.094 348.5L359.004 
            675L674.344 345.69C 705.664 307.68 716.194 275.48 720.00397 225C 720.00397 125.64 639.36395 45 540.00397
             45C 448.45398 45 372.79395 113.48 361.47397 201.97L361.47397 201.97L358.53397 201.97C 347.21396 113.48
              271.55396 45 180.00397 45L180.00397 45L180 45z" stroke="none" fill="{fgColor}" fill-rule="nonzero" />
            </g>
            </g>
          """,
        "ctrl-period": f"""
            <g transform="rotate(45)">
            <rect x="3" y="-2.5" width="5" height="5" style="fill:{fgColor}"  />
            </g>
          """,
        "ctrl-semi": f"""
           <g transform="translate(-.5, -.2)">
            <g transform="matrix(0.09345795 0 0 0.09345795 0 0)">
            <g transform="matrix(1 0 0 1 -296.25 7.5)">
                <g transform="matrix(1 0 0 1 62.596 -152.93)">
                <path d="M283.41 149.59C 272.46 168.20999 230.03 201.59999 255.53 211.72C 271.01 216.38 278.38 206.84
                281.5 200.53L281.5 200.53L282.12 200.53C 282.1 201.72 281.55 229.91 279.91 231.28C 276.31 234.29
                271.75 234.06 271.75 234.06L271.75 234.06L271.84 237.34L295.66 237.25L295.75 234C 295.75 234 291.22
                234.2 287.62 231.19C 285.97998 229.81 285.4 201.67 285.38 200.5L285.38 200.5L285.62 200.5C 288.15 
                205.66 297.71 216.33 308.22998 212.27C 338.99 200.87001 292.75998 170.3 283.40997 149.59L283.40997 
                149.59L283.41 149.59z" stroke="{fgColor}" stroke-width="1.25" fill="{fgColor}" fill-rule="nonzero" />
                </g>
                <path d="M345.98 -3.1575C 335.03 15.462501 292.6 48.8505 318.1 58.9685C 333.58002 63.6255 340.95 
                54.0915 344.07 47.7805L344.07 47.7805L344.7 47.7805C 344.67 48.9675 344.12003 77.156494 342.48 78.5305C
                338.88 81.5445 334.32 81.3115 334.32 81.3115L334.32 81.3115L334.42 84.5935L358.23 84.4995L358.32
                81.2495C 358.32 81.2495 353.79 81.45049 350.2 78.43649C 348.56 77.06249 347.97 48.920494 347.95
                47.749493L347.95 47.749493L348.2 47.749493C 350.72 52.906494 360.28 63.582493 370.80002 59.516495C
                401.56003 48.119495 355.33002 17.551495 345.98 -3.157505L345.98 -3.157505L345.98 -3.1575z"
                stroke="none" fill="{fgColor}" />
            </g>
        </g>
        </g>
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
    border_color = fgColor

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
    t = """
.at-ascii {
}

.ctrl {
    background-color: navy;
    color: white;
    padding:0.15em;
}
""".splitlines(False)

    out: List[str] = t

    paths = get_svg_paths()
    for key, path in paths.items():
        full_svg = wrap_svg_path(path)
        normalized = normalize_svg(full_svg)
        encoded = normalized  # html_encode(normalized)
        out.append(f'.at-ascii-{key}:after {{content: url("data:image/svg+xml,{encoded}"); }}')

    with open(f"{cssPath}", 'w') as f:
        f.writelines("\r\n".join(out))


make_css()
make_svgs()
