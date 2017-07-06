import {describe, it, before, after, beforeEach} from 'mocha'
import {expect} from 'chai'
import fuerte from '..';
import {serverURL} from './util.js';

describe('Create database', () => {
  const name = `testdb_${Date.now()}`
  const conn = new fuerte.connect(serverURL);
  let result;
  let error;
  beforeEach((done) => {
    conn.post('/_api/database', {
      name: name
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
    expect(result.result).to.be.true;
    expect(error).to.be.undefined;
  })
})

describe('Create collection', () => {
  const name = `testcol_${Date.now()}`
  const conn = new fuerte.connect(serverURL);
  let result;
  let error;
  beforeEach((done) => {
    conn.post('/_api/collection', {
      name: name
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
    expect(result.error).to.be.false;
    expect(result.type).to.equal(2);
    expect(error).to.be.undefined;
  })
})
