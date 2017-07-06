import {describe, it, before, after, beforeEach} from 'mocha'
import {expect} from 'chai'
import fuerte from '..';
import {serverURL} from './util.js';

describe('Getting the server version', () => {
  describe('using the constructor', () => {
    const conn = new fuerte.connect(serverURL);
    it('has properties', (done) => {
      conn.get('/_api/version')
        .then((res) => {
          expect(res.body).to.haveOwnProperty('server');
          expect(res.body).to.haveOwnProperty('version');
          done();
        }).catch(done);
    })
  })
})
