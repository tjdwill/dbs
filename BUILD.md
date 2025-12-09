## Dependencies

- [stduuid](https://github.com/mariusbancila/stduuid) by @mariusbancila (v1.2.3)
- [Qt](https://qt.io) (v6.9.x) by Qt Group (The Qt Company)
- [bde](https://github.com/bloomberg/bde) by Bloomberg
- [tomlplusplus](https://github.com/marzer/tomlplusplus) by @marzer (v3.4.0)

### Installing `bde`

Bloomberg provides [build instructions for
BDE](https://bloomberg.github.io/bde/library_information/build.html). When actually
building the library, configure with the following commands:

```
$ eval `bbs_build_env --profile <the compiler profile you are using> --build-dir <desired
build directory> --install-dir <desired install directory> --ufid opt_dbg_64_cpp23_pic
```

One can change the `--ufid` flag according to the desired config. See [the BBS Build
Configuration
page](https://bloomberg.github.io/bde-tools/bbs/reference/bbs_build_configuration.html#ufid) for further information.

After building and installing the library to your desired location, create a symbolic link
(Linux) to the installation in $dbs/3rdparty

**NOTE**: If building using Windows, one will need to build the debug version of BDE *if* building
DBS with a Debug build configuration.

### `stduuid`

This dependency is used to generate unique account and transaction ids.

- Git clone the repository
- Run `cmake`
- Install to `$dbs/3rdparty/stduuid` 
    ```bash
    stduuid/build$ cmake install --prefix=$dbs/3rdparty/stduuid
    ```

### `tomlplusplus`

This dependency is what is used for serialization (data-persistence).

- Git clone the repository
- Run `cmake`
- Install to `$dbs/3rdparty/tomlplusplus` 
    ```bash
    stduuid/build$ cmake install --prefix=$dbs/3rdparty/tomlplusplus
    ```

### Qt

I use `aqtinstall` to get a working Qt installation. Those who don't mind making a Qt account can
download an installation. Building from source is *also* an option, but I haven't been able to get
that to work.