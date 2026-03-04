$VENV_NAME = ".venv"

# create venv
if (-Not (Test-Path $VENV_NAME)) {
    python -m venv $VENV_NAME
}

# upgrade pip and install dependencies
& "$VENV_NAME\Scripts\pip.exe" install --upgrade pip
& "$VENV_NAME\Scripts\pip.exe" install -r requirements.txt