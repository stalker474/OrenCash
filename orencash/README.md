# OrenCash

## Building

### Dependencies

Mandatory:

* [boost](http://www.boost.org/) - used for serialization and tests
* [cryptopp](https://www.cryptopp.com/) - used as the cryptographic library
* [ed25519](https://github.com/orlp/ed25519) - used to complete the cryptographic library with key validation
* [restbed](https://github.com/Corvusoft/restbed) - used for the rest api
* [rpclib](https://github.com/rpclib/rpclib) - used for the node connections

Optional :

* [sqlite3](https://www.sqlite.org/) - used with odb for database storage
* [odb](https://www.codesynthesis.com/products/odb/download.xhtml) - used for easy object persistency implementation

### Install

#### Linux

I'm not working with linux so feel free to complete this information.

* clone the repository
* install and compile dependencies with sudo autoupdate.sh
* Build OrenCash with cmake
* Use PersisentObjects.sql located in dependencies.zip to generate the sqlite database.

So far the auto config tools are working without persistency, meaning you'll have to get your hands dirty to get ODB et sqlite3 and compile the project in -DBUILD_WITH_ODB=ON.

#### Windows

Use Visual Studio 2017.
* Get OrenCash source code and extract the folder "orencash" from "dependencies.zip" to "orencash" so you have the folders "vendor","bin-vs15" and "lib-vs15" available next to "src".
* run cmake -G"Visual Studio 15 2017 Win64" to generate the visual studio project files.
* open in visual studio and compile as "Release x64"
* you can use orencash/vendor/odb/odb-compile-orencash.bat to regenerate PersistentObjects-odb.* files, as well as PersistentObjects.sql
