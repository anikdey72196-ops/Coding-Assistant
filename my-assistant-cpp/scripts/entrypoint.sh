#!/bin/bash
# Docker entrypoint script

# Pass all arguments to the main application
exec /app/my-assistant-cpp "$@"
