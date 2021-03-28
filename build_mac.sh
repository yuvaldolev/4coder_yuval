#!/bin/bash

set -e

SCRIPT_PATH="$(realpath "${BASH_SOURCE[0]}")"
Y4_DIR="$(dirname "$SCRIPT_PATH")"
CUSTOM_DIR="$(dirname "$Y4_DIR")"
FCODER_ROOT_DIR="$(dirname "$CUSTOM_DIR")"

pushd "$FCODER_ROOT_DIR" &>/dev/null

"$CUSTOM_DIR/bin/buildsuper_x64-mac.sh" "$Y4_DIR/4coder_yuval.cpp"
# "$CUSTOM_DIR/bin/buildsuper_x64-mac.sh" "$Y4_DIR/4coder_fleury/4coder_fleury.cpp"
# "$CUSTOM_DIR/bin/buildsuper_x64-mac.sh" "$Y4_DIR/4coder-vimmish/4coder_sandvich_maker.cpp"

popd &>/dev/null
