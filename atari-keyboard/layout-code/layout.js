import { readFile, writeFileSync } from 'fs';
import jsdom from 'jsdom';
import * as d3 from 'd3';
const { JSDOM } = jsdom;

const LAYOUTFILE = "../layout-2.json";
const SVG_FILE = "test.svg";


readFile(`${LAYOUTFILE}`, function (err, data) {
  if (err) {
    throw err;
  }

  const layoutData = JSON.parse(data);

  const convertedData = convertMixedJson(layoutData);
  calculteAbsolutePositions(convertedData);

  console.log(JSON.stringify(convertedData, null, 2));

  const svg = plotSvg(convertedData);

  writeFileSync(SVG_FILE, svg);

  //console.log(svg);
});


function calculteAbsolutePositions(data) {
  const pos = {
    x: 0,
    y: 0
  };

  data.forEach(row => {
    pos.x = 0;
    let max_y = 0;

    row.forEach(key => {
      max_y = Math.max(max_y, key.y);
      pos.x += key.x;
      pos.y += key.y;

      key.pos = { ...pos };

      pos.x += key.w;
    });
    pos.y += 1;

  });
};



function convertMixedJson(layoutData) {
  const getInitMetaData = () => {
    return {
      keyname: "???",
      x: 0,
      y: 0,
      h: 1,
      w: 1,
    };

  }

  let metaData = getInitMetaData();

  const convertedData = [];

  layoutData.forEach(row => {

    if (!Array.isArray(row)) {
      // Skip keyboard metadata we don't really need.
      return;
    }

    // Start a new row
    convertedData.push([]);

    row.forEach(key => {

      if (typeof key !== 'string') {
        metaData = getInitMetaData();

        metaData.x = key.x || metaData.x;
        metaData.y = key.y || metaData.y;
        metaData.h = key.h || metaData.h;
        metaData.w = key.w || metaData.w;
      }

      if (typeof key === 'string') {
        metaData.keyname = key.split("\n");
        convertedData[
          convertedData.length - 1
        ].push(metaData);

        metaData = getInitMetaData();
      }

    });

  });

  return convertedData;
}


function plotSvg(data) {
  // find the bounding box.
  const boundingBox = {
    x1: Number.MAX_VALUE,
    y1: Number.MAX_VALUE,
    x2: Number.MIN_VALUE,
    y2: Number.MIN_VALUE,
  };

  const keyPos = [];

  data.forEach(row => {
    row.forEach(key => {
      keyPos.push(key);
      boundingBox.x1 = Math.min(boundingBox.x1, key.pos.x);
      boundingBox.y1 = Math.min(boundingBox.y1, key.pos.y);

      boundingBox.x2 = Math.max(boundingBox.x2, key.pos.x + key.w);
      boundingBox.y2 = Math.max(boundingBox.y2, key.pos.y + key.h);
    });
  });


  const dom = new JSDOM(`<!DOCTYPE html><body>
  <div id="svgcontainer"></div>
  <p>Hello world</body></p>`, { pretendToBeVisual: true });

  const doc = dom.window.document;

  const KEY_SIZE = 19.04;

  const width = boundingBox.x2 * KEY_SIZE;
  const height = boundingBox.y2 * KEY_SIZE;


  //Create SVG element
  var svg = d3.select(doc.querySelector("#svgcontainer"))
    .append("svg")
    .attr("width", width)
    .attr("height", height);

  const rects = svg.selectAll("foo")
    .data(keyPos)
    .enter()
    .append("rect")
    .attr("x", d => d.pos.x * KEY_SIZE)
    .attr("y", d => d.pos.y * KEY_SIZE)
    .attr("width", d => d.w * KEY_SIZE)
    .attr("height", d => d.h * KEY_SIZE)
    .attr("fill", "white")
    .attr("stroke", "teal");


  const text = svg.selectAll("foo")
    .data(keyPos)
    .enter()
    .append("text")
    .attr("x", d => (d.pos.x + d.w / 2) * KEY_SIZE)
    .attr("y", d => (d.pos.y + d.h / 2) * KEY_SIZE)
    .attr("text-anchor", "middle")
    .attr("dominant-baseline", "middle")
    .style("font-size", `${KEY_SIZE / 4}px`)
    .attr("font-weight", "light")
    //  .style("text-decoration", "underline")
    .attr("stroke", "red")
    .attr("stroke-width", "1")
    .attr("font-family", "Helvetica")
    .text(d => d.keyname[0]);

  /*
     <text
            x={x + width / 2}
            y={y + height / 2}
            dominant-baseline="middle"
            text-anchor="middle"
        >*/


  const guts = doc.querySelector("#svgcontainer").innerHTML
    .replace(/<\/rect>/g, "</rect>\n")
    .replace(/<\/text>/g, "</text>\n");


  return `<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="${width}" height="${height}"  xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">

${guts}

</svg>
`


}

const K_TL = 0;
const K_TM = 1;
const K_TR = 2;

const K_CL = 3;
const K_CM = 4;
const K_CR = 5;

const K_BL = 6;
const K_BM = 7;
const K_BR = 8;

const K_FL = 9;
const K_FM = 10;
const K_FR = 11;

const K_BLANK = -1;

const alignGrid = [
  [
    K_TL,
    K_BL,
    K_TR,
    K_FL,
    K_FR,
    K_CL,
    K_CR,
    K_TM,
    K_CM,
    K_BM,
    K_FM,
  ],
  [
    K_TM,
    K_BLANK,
    K_BLANK,
    K_FL,
    K_FR,
    K_CM,
    K_BLANK,
    K_BLANK,
    K_BLANK,
    K_BLANK,
    K_FM,
  ],

];

