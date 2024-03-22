workspace_dir=SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
premake=./vendor/premake/macos/premake5

cd $workspace_dir/..

# Generate makefile for gcc
$premake gmake2


