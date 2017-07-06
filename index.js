/**
 * @module fuerte
 * @file ArangoDB low level nodejs connection driver.
 * @description This driver is be a low level, high performance
 * connection provider for ArangoDB. 
 * In most cases you should use {@link https://github.com/arangodb/arangojs|ArangoJS} instead.
 * It provides a high level driver that is much more client 
 * programmer friendly.
 * @license http://www.apache.org/licenses/LICENSE-2.0
 * @copyright Copyright 2017 ArangoDB GmbH, Cologne, Germany
 * @author Ewout Prangsma
 */

const fuerte = require('bindings')('arango-node-driver');
const { URL } = require('url');

// ------------------------------------
// connect
// ------------------------------------

/**
 * Open a connection to a database server.
 * @function connect
 * @param {Object} options - options for the connection or a host string.
 * @param {string} options.host - URL of the server. E.g. "http://localhost:8529"
 * @param {string} options.user - Optional username for authentication.
 * @param {string} options.pass - Optional password for authentication.
 * @return {Connection}
 * @example
 * const conn = fuerte.connect("http://localhost:8529");
 * const versionInfo = await conn.get('/_api/version');
 */
fuerte.connect = function(options) {
    if (typeof options == 'string') {
        options = { host: options };
    }
    var builder = new fuerte.ConnectionBuilder();
    if (options.host) {
        builder.host = options.host;
    } else {
        throw new Error("host option missing");
    }
    if (options.user) {
        builder.userName = options.user;
    }
    if (options.pass) {
        builder.password = options.pass;
    }
    return builder.connect();
}

// ------------------------------------
// Connection
// ------------------------------------

/**
 * ConnectionBuilder helper class to build connections.
 * @class ConnectionBuilder
 * @property {string} host - URL of the host. E.g. "http://localhost:8529"
 * @property {string} userName - Name used for authentication of a new connection.
 * @property {string} password - Password used for authentication of a new connection.
 */
const ConnectionBuilder = fuerte.ConnectionBuilder;

/**
 * Open a connection to a database server.
 * @function connect
 * @memberof ConnectionBuilder
 * @instance
 * @return {Connection} - The opened connection.
 * @example
 * const builder = new fuerte.ConnectionBuilder();
 * builder.host = "vst://localhost:8529";
 * const connection = builder.connect();
 */
ConnectionBuilder.prototype.connect = function() {
    let host = this.host;
    if (host) {
        // Extract username+password from host (if set)
        try {
            const u = new URL(host);
            // Save username+password
            if (u.username && !this.userName) this.userName = u.username;
            if (u.password && !this.password) this.password = u.password;
            u.username = undefined;
            u.password = undefined;
            // Save normalized url
            let x = `${u.protocol}//${u.host}`;
            this.host = x;
        } catch (err) {
            // Ignore
        }
    }
    this.nativeHost = this.host || 'localhost';
    return this.nativeConnect();
};


/**
 * Request data send to a database server.
 * @class Request
 * @property {string} path - Local path of the request
 * @property {string} database - Name of the database that this request targets.
 * @property {string} method - (HTTP) method of this request.
 * @property {string} contentType - Content-Type of this request.
 * @property {string} acceptType - Accept-Type of this request.
 */
const Request = fuerte.Request;

/**
 * Add an object|array|value or Buffer containing a velocypack slice to this request.
 * Non-Buffer argument is converted to velocypack automatically.
 * @function addBody
 * @memberof Request
 * @instance
 * @param {any|Buffer} data - Body to add
 * @return {Request} - The request itself.
 * @example
 * const req = new fuerte.Request();
 * req.addBody({name:"Jan"});
 */

/**
 * Add a Buffer containing a velocypack slice to this request.
 * @function addSlice
 * @memberof Request
 * @instance
 * @param {Buffer} slice - Velocypack slice (contained in Buffer) to add
 * @return {Request} - The request itself.
 */

/**
 * Add a Buffer containing binary data to this request.
 * The contents of the buffer is not inspected.
 * @function addBuffer
 * @memberof Request
 * @instance
 * @param {Buffer} data - Binary data (contained in Buffer) to add
 * @return {Request} - The request itself.
 */

/**
 * Add a query parameter to this request.
 * @function addQueryParameter
 * @memberof Request
 * @instance
 * @param {string} key - Parameter name
 * @param {string} value - Parameter value
 * @return {Request} - The request itself.
 * @example
 * const req = new fuerte.Request();
 * req.path = '/mypath'
 * req.addQueryParameter("limit", "5");
 * // Results in a query like '/mypath?limit=5'
 */

/**
 * Add a header key/value pair to this request.
 * @function addHeader
 * @memberof Request
 * @instance
 * @param {string} key - Header name
 * @param {string} value - Header value
 * @return {Request} - The request itself.
 * @example
 * const req = new fuerte.Request();
 * req.addHeader("X-CustomHeader", "123");
 */

/**
 * Response data resulting from a request to a database server.
 * @class Response
 * @property {string} contentType - Content-type of the response payload.
 * @property {number} responseCode - (HTTP) response code of the response.
 * @property {object} header - (HTTP) header of the response.
 * @property {*} body - Entire response payload as a decoded object/array/value.
 * If the response contains multiple slices, they are returned 
 * as a single array containing the decoded values of each slice.
 * If the payload is empty, undefined is returned.
 * @property {Buffer} payload - Entire (raw) payload of the response in a buffer.
 * If the payload is empty, an empty buffer is returned.
 * @property {[]Buffer} slices - Array of buffers, containing the slices of the response.
 * Each slice is contained in a single buffer.
 * If the response does not have a velocypack content type, undefined
 * is returned.
 * @property {Request} request - Request that resulted in this response.
 */
const Response = fuerte.Response;

/**
 * Create a fuerte Request from given arguments.
 * @function
 * @param {SendRequestOptions} options 
 * @param {*} data - Payload of the request (defaults to options.data)
 * @param {string} method - Method of the request (defaults to options.method or 'get')
 * @private
 */
function createRequestFromOptions(options, data, method) {
    if (typeof options == 'string') {
        // String options default to path 
        options = { path: options };
    }
    options = options || {};
    var req = new fuerte.Request();
    req.path = options.path || '/';
    req.method = options.method || method || 'get';
    if (options.database) {
        req.database = options.database;
    }
    if (options.contentType) {
        req.contentType = options.contentType;
    }
    if (options.acceptType) {
        req.acceptType = options.acceptType;
    }
    if (typeof options.query == 'object') {
        const query = options.query;
        for (var property in query) {
            if (query.hasOwnProperty(property)) {
                req.addQueryParameter(property, query[property]);
            }
        }
    }
    if (typeof options.header == 'object') {
        const header = options.header;
        for (var property in header) {
            if (header.hasOwnProperty(property)) {
                req.addHeader(property, header[property]);
            }
        }
    }
    data = data || options.data;
    if (data) {
        req.addBody(data);
    }
    return req;
}

/**
 * Connection to a database server.
 * @class Connection
 * @property {Number} requestsLeft - Number of requests that have not yet finished.
 */
const Connection = fuerte.Connection;

/**
 * Start a new HTTP request to the database server.
 * @function
 * @param {SendRequestOptions} options
 * @param {*} data 
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const versionInfo = await conn.do({
 *   method: 'get',
 *   path: '/_api/version'
 * });
 * @example 
 * const conn = connect("vst://localhost:8529");
 * conn.do({
 *   method: 'get',
 *   path: '/_api/version'
 * }, function(err, res) {
 *    // process response 
 * });
 */
Connection.prototype.do = function(options, data, cb) {
    if ((typeof data == 'function') && (typeof cb == 'undefined')) {
        cb = data; 
        data = undefined;
    }
    var req = createRequestFromOptions(options, data, 'get');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP GET request to the database server.
 * @function
 * @param {SendRequestOptions} options
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const versionInfo = await conn.get('/_api/version');
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const versionInfo = await conn.get({ path: '/_api/version' });
 */
Connection.prototype.get = function(options, cb) {
    var req = createRequestFromOptions(options, undefined, 'get');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP POST request to the database server.
 * @function
 * @param {SendRequestOptions} options
 * @param {*} data 
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const result = await conn.post('/_api/document/people', { name: 'Jan' });
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const result = await conn.post({
 *   path: '/_api/document/people', 
 *   data: { name: 'Jan' }
 * });
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const result = await conn.post(
 *   '/_api/document/people', 
 *   { name: 'Jan' }, 
 *   function(err, res) {
 *     // Process response
 * });
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const result = await conn.post({
 *   path: '/_api/document/people', 
 *   data: { name: 'Jan' }
 * }, function(err, res) {
 *   // Process response
 * });
 */
Connection.prototype.post = function(options, data, cb) {
    if ((typeof data == 'function') && (typeof cb == 'undefined')) {
        cb = data; 
        data = undefined;
    }
    var req = createRequestFromOptions(options, data, 'post');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP PUT request to the database server.
 * @function
 * @param {SendRequestOptions} options
 * @param {*} data 
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 */
Connection.prototype.put = function(options, data, cb) {
    if ((typeof data == 'function') && (typeof cb == 'undefined')) {
        cb = data; 
        data = undefined;
    }
    var req = createRequestFromOptions(options, data, 'put');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP PATCH request to the database server.
 * @param {SendRequestOptions} options
 * @param {*} data 
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 */
Connection.prototype.patch = function(options, data, cb) {
    if ((typeof data == 'function') && (typeof cb == 'undefined')) {
        cb = data; 
        data = undefined;
    }
    var req = createRequestFromOptions(options, data, 'patch');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP DELETE request to the database server.
 * @param {SendRequestOptions} options
 * @param {RequestCallback} cb
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 */
Connection.prototype.delete = function(options, cb) {
    var req = createRequestFromOptions(options, undefined, 'delete');
    return this.sendRequest(req, cb);
}

/**
 * Start a new HTTP API request to the database server.
 * @function
 * @param {Request} req - A fully configured request.
 * @param {RequestCallback} cb - Callback called on completion of the request.
 * @returns {Number|Promise} - When a callback is provided, an identifier for the request is returned, otherwise a {@link Promise} is returned.
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const req = new fuerte.Request();
 * req.path = '/_api/version';
 * req.method = 'get';
 * const result = await conn.sendRequest(req);
 * @example 
 * const conn = connect("vst://localhost:8529");
 * const req = new fuerte.Request();
 * req.path = '/_api/version';
 * req.method = 'get';
 * conn.sendRequest(req, function(err, res) {
 *   // Process response
 * });
 */
Connection.prototype.sendRequest = function(req, cb) {
    if (cb) {
        return this.nativeSendRequest(req, cb);
    }
    return new Promise((resolve, reject) => this.nativeSendRequest(req, function(err, res) {
        if (err) reject(err); 
        else resolve(res);
    }));
};

/**
 * @callback RequestCallback
 * @name RequestCallback
 * @description Callback used by all request methods.
 * @param {Error} err - Error, if undefined request succeeded.
 * @param {Response} response
*/

/**
 * Options passed to sendRequest, do, get, post, put, patch & delete.
 * @typedef {Object} SendRequestOptions
 * @name SendRequestOptions
 * @property {string} path - Local path of the request.
 * @property {string} database - Name of the database the requests targets.
 * @property {string} method - Method of the request (get|post|put|patch|delete).
 * @property {string} acceptType - Content type to accept in response.
 * @property {string} contentType - Content type of the request.
 * @property {Object} query - Query parameters of the request.
 * @property {Object} header - Header meta data of the request.
 */

module.exports = fuerte;
