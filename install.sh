#!/bin/bash

THIS_BASH_SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";

touch ~/.bash_profile
echo source $THIS_BASH_SCRIPT_DIR/src/nuensis.sh >> ~/.bash_profile
