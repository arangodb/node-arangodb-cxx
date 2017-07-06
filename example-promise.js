const fuerte = require('.');
const util = require('util');

console.log("creating connections")
var connection_vst = fuerte.connect("vst://127.0.0.1:8529");
var connection_http = fuerte.connect("http://127.0.0.1:8529");

// Show error response
function showError(code, res) {
  console.log("\n#### error ####\n")
  console.log("error code: " + code)
  if (res) {
    console.log("response code: " + res.responseCode);
    console.log("content type:  " + res.contentType);
    console.log("raw payload:   " + res.payload);
  }
}

// Show success response
function showResponse(res) {
  console.log("\n#### succes ####\n")
  // we maybe return a pure js objects (req/res)
  if (res) {
    console.log("response code: " + res.responseCode);
    console.log("content type:  " + res.contentType);
    console.log("body:          " + util.inspect(res.body, {depth: null}));
    console.log("header:        " + util.inspect(res.header));
    console.log("request:       " + util.inspect(res.request, {depth: null}));
    console.log("raw payload:   " + util.inspect(res.payload));
    console.log("raw slices :   " + util.inspect(res.slices));
  }
}

function run_example(connection, proto) {
  const t1 = () => {
    return new Promise((resolve) => {
      console.log("queue 1 " + proto)
      connection.get('/_api/version')
        .then((res) => { showResponse(res); resolve(); })
        .catch((err) => { showError(err); resolve(); });
    });
  }

  // Make lowest level requests
  const t2 = () => {
    return new Promise((resolve) => {
      console.log("queue 2 " + proto);
      let req = new fuerte.Request();
      req.method = 'get';
      req.path = '/_api/version';
      const p1 = connection.sendRequest(req);
      console.log("queue 3 " + proto)
      const p2 = connection.sendRequest(req);
      Promise.all([p1, p2]).then((values) => {
        showResponse(values[0])
        showResponse(values[1])
        resolve();
      }).catch((err) => { showError(err); resolve(); });
    });
  }

  // Post data
  const t3 = () => {
    return new Promise((resolve) => {
      console.log("queue 4 " + proto)
      connection.post('/_api/cursor', {"query": "FOR x IN 1..5 RETURN x"})
        .then((res) => { showResponse(res); resolve(); })
        .catch((err) => { showError(err); resolve(); });
    });
  }

  // Posting using low level request
  const t4 = () => {
    return new Promise((resolve) => {
      console.log("queue 5 " + proto)
      let req = new fuerte.Request();
      const slice = fuerte.vpackEncode({});
      req.method = "post";
      req.path = "/_api/document/_users";
      req.addSlice(slice);
      connection.sendRequest(req) 
        .then((res) => { showResponse(res); resolve(); })
        .catch((err) => { showError(err); resolve(); });
    });
  }

  // Generic get/post/... using do method.
  const t5 = () => {
    return new Promise((resolve) => {
      console.log("queue 6 " + proto)
      connection.do({
        path: '/_api/document/_users',
        method: 'post',
        acceptType: 'application/json',
        data: {},
      })
        .then((res) => { showResponse(res); resolve(); })
        .catch((err) => { showError(err); resolve(); });
    });
  }

  t1().then(t2).then(t3).then(t4).then(t5);
}

console.log("run examples with velocystream")
run_example(connection_vst, "vst");
console.log("run examples with http")
run_example(connection_http, "http");
