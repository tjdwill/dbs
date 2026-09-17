# Must run from top-level project directory

# Ensure submodules are initialized
git submodule update --init

# stduuid, tomlplusplus
cd 3rdparty
for repo in stduuid tomlplusplus
do 
    buildDir="_lib/_temp$repo"
    mkdir -p "$buildDir"
    cmake -S "$repo" -B "$buildDir"
    cmake --install "$buildDir" --prefix=$(realpath _lib)
    rm -rf "$buildDir"
done
unset buildDir