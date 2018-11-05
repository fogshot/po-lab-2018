echo "The test runner relies on the presence of build/add-1.c.bc as a dummy input"
echo "Run ../samples/run.sh to create this file"
echo "--------------- BEGIN TESTS ------------------------"

# TODO: set these paths to your LLVM build root directory
VSA_LLVM_PATH=/Users/dominik/llvmbuild7
VSA_CLANG_PATH=/Users/dominik/llvmbuild7

$VSA_LLVM_PATH/bin/opt -load $VSA_LLVM_PATH/lib/llvm-vsa.dylib -testBS < ../samples/build/add-1.c.bc > /dev/null
