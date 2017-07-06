import {describe, it, before, after, beforeEach} from 'mocha'
import {expect} from 'chai'
import fuerte from '..';
import {serverURL} from './util.js';

describe('Create graph', () => {
  const name = `testgraph_${Date.now()}`
  const conn = new fuerte.connect(serverURL);
  let result;
  let error;
  beforeEach((done) => {
    conn.post('/_api/gharial', {
      name: name,
      edgeDefinitions: [{
        collection: "edges",
        from: ["startVertices"],
        to: ["endVertices"]
      }]
    }).then((res) => {
      result = res.body;
      done();
    })
    .catch((err) => {
      error = err;
      done();
    });
  })
  it('has properties', () => {
    console.log(result);
    expect(result.error).to.be.false;
    expect(error).to.be.undefined;
  })
})
