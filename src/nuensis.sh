#!/bin/bash

function nuensis() {
  INVOCATION_DIR=\"$(pwd)\"
  THIS_BASH_SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
  args="$*" 
 
  if [[ $WD == *" "* ]]; then
    echo There are spaces in your PATH! You call yourself programmer? Do some good to yourself and remove them!
  else
    python3 $THIS_BASH_SCRIPT_DIR/main_amanuensis.py --wd $INVOCATION_DIR $args
  fi
    
}
