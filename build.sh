#!/bin/bash

ME="$(realpath "$0")"
LOCATION="$(dirname "$ME")"
PROJECT_ROOT="$(dirname "$LOCATION")"
CUSTOM_ROOT="$PROJECT_ROOT/custom"

BIN="custom_4coder.so"
BIN_DSYM="$BIN.dSYM"

pushd "$CUSTOM_ROOT" > /dev/null
bin/buildsuper_x64-mac.sh "$LOCATION/4coder_yuval.cpp"
mv "$BIN" "$PROJECT_ROOT/$BIN"

rm -r "$PROJECT_ROOT/$BIN_DSYM"
mv "$BIN_DSYM" "$PROJECT_ROOT/$BIN_DSYM"
popd > /dev/null
