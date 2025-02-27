#!/usr/bin/env bash
o=bin/release
meson setup $o --reconfigure --buildtype=release
meson compile -C $o
meson compile -C $o strip
