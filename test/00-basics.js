import {describe, it, before, after, beforeEach} from 'mocha'
import {expect} from 'chai'
import fuerte from '..';

describe('Creating a connection builder', () => {
  describe('using the constructor', () => {
    const builder = new fuerte.ConnectionBuilder();
    it('has properties', () => {
      expect(builder).to.have.a.property('nativeHost', 'localhost')
    })
  })
})

describe('Creating an HTTP connection', () => {
  describe('using the ConnectionBuilder', () => {
    const builder = new fuerte.ConnectionBuilder();
    builder.host = 'http://localhost:8529'
    const conn = builder.connect();
    it('has properties', () => {
      expect(conn).to.have.a.property('requestsLeft', 0)
    })
  })
  describe('using the connect()', () => {
    const conn = fuerte.connect('http://localhost:8529');
    it('has properties', () => {
      expect(conn).to.have.a.property('requestsLeft', 0)
    })
  })
})

describe('Creating a VST connection', () => {
  describe('using the ConnectionBuilder', () => {
    const builder = new fuerte.ConnectionBuilder();
    builder.host = 'vst://localhost:8529'
    const conn = builder.connect();
    it('has properties', () => {
      expect(conn).to.have.a.property('requestsLeft', 0)
    })
  })
  describe('using the connect()', () => {
    const conn = fuerte.connect('vst://localhost:8529');
    it('has properties', () => {
      expect(conn).to.have.a.property('requestsLeft', 0)
    })
  })
})
