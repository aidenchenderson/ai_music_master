#!/bin/bash
# run using '. venv_setup_unix.sh'

# exit on error
set -e 

VENV_NAME=.venv

# create virtual environment
if [ ! -d "$VENV_NAME" ]; then
    python3 -m venv $VENV_NAME
fi

# activate venv
source $VENV_NAME/bin/activate

# upgrade pip and install dependencies
pip install --upgrade pip
pip install -r requirements.txt