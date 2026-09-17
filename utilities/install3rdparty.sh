# Must run from top-level project directory

# Ensure submodules are initialized
git submodule update --init

topLevelProject=$(pwd)

# stduuid, tomlplusplus
installHeaderOnlyLibraries()
{
    cd "$topLevelProject/3rdparty"
    for repo in stduuid tomlplusplus
    do 
        echo "Installing ${repo}"
        local buildDir="_lib/_tmp$repo"
        mkdir -p "$buildDir"
        cmake -S "$repo" -B "$buildDir"
        cmake --install "$buildDir" --prefix=$(realpath _lib)/"$repo"
        rm -rf "$buildDir"
    done
}

# Build and Install BDE for all three configurations using the default profile
buildAndInstallBde()
{
    cd "$topLevelProject/3rdparty"
    local oldPATH="$PATH"
    export PATH=$(realpath bde-tools/bin):"$PATH"
    
    local configurationsMap=([Debug]=dbg [Release]=opt [RelWithDebInfo]=opt_dbg)
    local buildDirPrefix="$topLevelProject/3rdparty/_lib/_tmpBde"
    local installDirPrefix="$topLevelProject/3rdparty/_lib/bde"
    cd bde
    for config in ${!configurationsMap[@]}
    do
        echo "Building and installing BDE ${config} configuration."
        local ufidString="${configurationsMap[$config]}_64_cpp23_pic"
        eval `bbs_build_env --install-dir "$installDirPrefix/$config" --build-dir "$buildDirPrefix/$config" --ufid "${ufidString}"`
        bbs_build configure --clean
        bbs_build build
        bbs_build install
    done
    rm -rf "$buildDirPrefix"
}

# ---

installHeaderOnlyLibraries
buildAndInstallBde