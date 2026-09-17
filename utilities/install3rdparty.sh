# Must run from top-level project directory

# Ensure submodules are initialized
git submodule update --init

# stduuid, tomlplusplus
installHeaderOnlyLibraries()
{
    cd 3rdparty
    for repo in stduuid tomlplusplus
    do 
        echo "Installing ${repo}"
        local buildDir="_lib/_temp$repo"
        mkdir -p "$buildDir"
        cmake -S "$repo" -B "$buildDir"
        cmake --install "$buildDir" --prefix=$(realpath _lib)/"$repo"
        rm -rf "$buildDir"
    done
}

buildAndInstallBde()
{
    echo -e "\nThe BDE libraries must currently be built separately due to different levels of configuration. See BUILD.md"
}

# ---

installHeaderOnlyLibraries
buildAndInstallBde