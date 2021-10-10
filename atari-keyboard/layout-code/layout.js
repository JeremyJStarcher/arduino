const fs = require('fs');
const LAYOUTFILE = "../layout-2.json";


fs.readFile(`${__dirname}/${LAYOUTFILE}`, function (err, data) {
  if (err) {
    throw err;
  }

  const layoutData = JSON.parse(data);

  const convertedData = convertMixedJson(layoutData);
  calculteAbsolutePositions(convertedData);

  console.log(JSON.stringify(convertedData, null, 2));
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

      key.pos = {... pos};

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

  metaData = getInitMetaData();

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
