import { createRequire } from 'module';

const require = createRequire(import.meta.url);
const fs = require('fs');
const getBounds = require('svg-path-bounds')


const codelist =
    [
        "ctrl-a",
        "ctrl-b",
        "ctrl-c",
        "ctrl-d",
        "ctrl-e",
        "ctrl-f",
        "ctrl-g",
        "ctrl-h",
        "ctrl-i",
        "ctrl-j",
        "ctrl-k",
        "ctrl-l",
        "ctrl-m",
        "ctrl-n",
        "ctrl-o",
        "ctrl-p",
        "ctrl-q",
        "ctrl-r",
        "ctrl-s",
        "ctrl-t",
        "ctrl-u",
        "ctrl-v",
        "ctrl-w",
        "ctrl-x",
        "ctrl-y",
        "ctrl-z",
    ].map((s, i) => ({
        pos: i + 362,
        name: s,
    }));

codelist.push({ pos: 355, name: "ctrl-;" });
codelist.push({ pos: 357, name: "ctrl-," });
codelist.push({ pos: 358, name: "ctrl-." });
codelist.push({ pos: 1129, name: "fuji" });

// console.log(codelist);

function ripit() {
    // Calling the readFileSync() method
    // to read 'input.txt' file
    const data = fs.readFileSync('./EightBit-Atari-Regular.svg', { encoding: 'utf8', flag: 'r' })
        .replace(/\n|\r/g, " ")

    const start = "<glyph"
    const end = "/>";

    let idx = 0;
    let count = 0;
    while (true) {
        const s = data.indexOf(start, idx);
        if (s === -1) {
            break;
        }
        const e = data.indexOf(end, s) + end.length;


        const g = data.substring(s, e);
        // console.log(JSON.stringify(g), { idx, s, e, l: e-s  });
        // console.log(g);
        idx = s + 1;

        count += 1;
        makeglyph(count, g)
    }
    makeHtml(count);
}

function padNumber(n) {
    const cc = ("0000000000000000000" + n).substr(-4);
    return cc;
}

function makeglyph(count, g) {
    const t = g.replace("<glyph", "<path");


    // console.log("t = ", t);
    let path = t.split('d="')[1];
    if (!path) {
        // some elements have no path
        return;
    }
    path = path.split('"')[0]
    // console.log('path = ', path);

    const [left, top, right, bottom] = getBounds(path);
    // console.log({left, top, right, bottom})

    const height = bottom + top;
    const width = left + right;

    const newHeight = 15;
    const newWidth = 15; // (newHeight / height) * width;

    const t1 = `<g fill="grey" transform="scale(1, -1) translate(0, -${height}">
        ${t}
   </g>`;


    const s = `<?xml version="1.0" encoding="utf-8" ?>
    <svg
         xmlns="http://www.w3.org/2000/svg" 
         xmlns:xlink="http://www.w3.org/1999/xlink" 
         width="${newWidth}px"
         height="${newHeight}px"
         viewBox="${left} ${top} ${right} ${bottom}"
         version="1.1">
    xmlns:ev="http://www.w3.org/2001/xml-events" xmlns:xlink="http://www.w3.org/1999/xlink">
    <defs />
    ${t1}
</svg>`;


    const cc = padNumber(count);
    fs.writeFileSync(`svgs/svg_${cc}.svg`,
        s,
        {
            encoding: "utf8",
            // flag: "a+",
            // mode: 0o666
        });
}


function makeHtml(count) {
    let b = [];
    for (let i = 1; i < count + 1; i++) {

        let keydata = codelist.find(p => p.pos === i);
        if (!keydata) {
            keydata = {
                pos: i,
                name: i,
            }
            //  continue;
        }

        const f = `svg_${padNumber(i)}.svg`;
        b.push(`<section>
                    <div>
                        <img src='${f}' />
                        <br />
                        <span>${f} - ${keydata.name}</span>
                    </div>
                </section>
                <br />
                <br />
                <br />
                <br />
`)
    }

    const str = `
    <html>
        <head>
            <title>Demo page</title>
            <style>
              section {
                  border: 2px solid blue;           
                }
              div {
              }
              img {
                  border: 2px solid red;
                  min-height: 50px;
                  min-width: auto;
                  transform: scaleY(-1);
              }
            </style>
        </head>
        <body>
            <h1>DEMO PAGE</h1>
            ${b.join("\r\n")}
        </body>
    </html>`

    fs.writeFileSync(`svgs/index.html`,
        str,
        {
            encoding: "utf8",
            // flag: "a+",
            // mode: 0o666
        });

}

ripit();