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

async function run_example(connection, proto) {
  try {
    // Use simple get
    console.log("queue 1 " + proto)
    const res = await connection.get('/_api/version');
    showResponse(res);
    console.log("1 done " + proto)
    console.log("------------------------------------------")
  } catch (e) {
    showError(e);
  }

  try {
    // Make lowest level requests
    console.log("queue 2 " + proto);
    let req = new fuerte.Request();
    req.method = 'get';
    req.path = '/_api/version';
    const ares1 = connection.sendRequest(req);
    console.log("queue 3 " + proto)
    const ares2 = connection.sendRequest(req);
    showResponse(await ares1);
    showResponse(await ares2);
    console.log("2,3 done " + proto)
    console.log("------------------------------------------")
  } catch (e) {
    showError(e);
  }

  try {
    // Post data
    console.log("queue 4 " + proto)
    const res = await connection.post('/_api/cursor', {"query": "FOR x IN 1..5 RETURN x"});
    showResponse(res);
    console.log("4 done " + proto)
    console.log("------------------------------------------")
  } catch (e) {
    showError(e);
  }

  try {
    // Posting using low level request
    console.log("queue 5 " + proto)
    let req = new fuerte.Request();
    const slice = fuerte.vpackEncode({});
    req.method = "post";
    req.path = "/_api/document/_users";
    req.addSlice(slice);
    const res = await connection.sendRequest(req);
    showResponse(res);
    console.log("5 done " + proto)
    console.log("------------------------------------------")
  } catch (e) {
    showError(e);
  }

  try {
    // Generic get/post/... using do method.
    console.log("queue 6 " + proto)
    const res = await connection.do({
      path: '/_api/document/_users',
      method: 'post',
      acceptType: 'application/json',
      data: {},
    });
    showResponse(res);
    console.log("6 done " + proto)
    console.log("------------------------------------------")
  } catch (e) {
    showError(e);
  }
}

console.log("run examples with velocystream")
run_example(connection_vst, "vst");
console.log("run examples with http")
run_example(connection_http, "http");
