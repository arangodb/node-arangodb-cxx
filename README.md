# Node-arangodb-cxx aka Fuerte for NodeJS.

This package contains a low-level NodeJS connection driver 
that enables you to make an HTTP(s) or Velocystream connection
to an ArangoDB server.

It wraps the [C++ Fuerte](https://github.com/arangodb/fuerte) driver.

In most cases you do not use this driver directly.
Instead you should consider using [ArangoJS](https://github.com/arangodb/arangojs). 
`ArangoJS` is a high level driver that contains a much more client 
programmer oriented API.

## API

See [API](./docs/index.md)

## Installation

Install node and npm and execute

```
npm install --save fuerte
```

Then to see Fuerte in action, make sure you have ArangoDB running at localhost on port 8529, and run `examples.js` from this repository.

```
node example.js
```

## Threading

Fuerte is moving work away from the NodeJS event loop as soon as possible and tries to do as much as possible in
its own threadpool. 
The number of threads in this pool relates to the number of CPU cores available in your machine.

To override this default behavior, you can specify it in a `FUERTE_THREAD_COUNT` environment variable.
E.g. `FUERTE_THREAD_COUNT=3` causes the threadpool to use 3 threads.

## License

Fuerte is published under the Apache 2 License. Please see
the files [LICENSE](LICENSE) and
[LICENSES-OTHER-COMPONENTS.md](LICENSES-OTHER-COMPONENTS.md)
for details.
