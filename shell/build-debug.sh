#!/usr/bin/env bash
o=bin/debug
meson setup $o --reconfigure --buildtype=debugoptimized
meson compile -C $o
