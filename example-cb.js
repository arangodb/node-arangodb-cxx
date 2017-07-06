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
  function test1(next) {
    console.log("queue 1 " + proto)
    connection.get('/_api/version', (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
      console.log("1 done " + proto)
      console.log("------------------------------------------")
      next();
    });
  }

  function test2(next) {
    // Make lowest level requests
    console.log("queue 2 " + proto);
    let req = new fuerte.Request();
    req.method = 'get';
    req.path = '/_api/version';
    connection.sendRequest(req, (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
    });
    console.log("queue 3 " + proto)
    connection.sendRequest(req, (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
      console.log("2,3 done " + proto)
      console.log("------------------------------------------")
      next();
    });
  }

  function test3(next) {
    // Post data
    console.log("queue 4 " + proto)
    connection.post('/_api/cursor', {"query": "FOR x IN 1..5 RETURN x"}, (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
      console.log("4 done " + proto)
      console.log("------------------------------------------")
      next();      
    });
  }

  function test4(next) {
    // Posting using low level request
    console.log("queue 5 " + proto)
    let req = new fuerte.Request();
    const slice = fuerte.vpackEncode({});
    req.method = "post";
    req.path = "/_api/document/_users";
    req.addSlice(slice);
    connection.sendRequest(req, (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
      console.log("5 done " + proto)
      console.log("------------------------------------------")
      next();      
    });
  }

  function test5(next) {
    // Generic get/post/... using do method.
    console.log("queue 6 " + proto)
    connection.do({
      path: '/_api/document/_users',
      method: 'post',
      acceptType: 'application/json',
      data: {},
    }, (err, res) => {
      if (err) showError(err, res);
      else showResponse(res);
      console.log("6 done " + proto)
      console.log("------------------------------------------")
      next();  
    });
  }

  const t5 = () => test5(() => {});
  const t4 = () => test4(t5);
  const t3 = () => test3(t4);
  const t2 = () => test2(t3);
  const t1 = () => test1(t2);
  t1();
}

console.log("run examples with velocystream")
run_example(connection_vst, "vst");
console.log("run examples with http")
run_example(connection_http, "http");
