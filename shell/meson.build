project('zebra-shell', 'c',
  version: '0.0.0',
  default_options: [
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
  ])

# Compiler
cc = meson.get_compiler('c')

# Dependences
glib_dep = dependency('glib-2.0')
json_glib_dep = dependency('json-glib-1.0')

# C flags
cflags = [
  '-pipe',
]

# Libraries
link_args = [
  '-lm',
]

# Flags for release
if get_option('buildtype') == 'release'
  cflags += [
    '-march=native',
    '-fvisibility=hidden',
    '-ffunction-sections',
    '-fdata-sections',
  ]
  link_args += [
    '-Wl,--gc-sections',
    '-Wl,--strip-all',
  ]
else
  cflags += '-g'
  link_args += '-g'
endif

# Executables
shell_exe = executable('shell',
  sources: 'shell.c',
  dependencies: [glib_dep, json_glib_dep],
  c_args: cflags,
  link_args: link_args,
)

process_exe = executable('process',
  sources: 'process.c',
  c_args: cflags,
  link_args: link_args,
)

process2_exe = executable('process2',
  sources: 'process2.c',
  c_args: cflags,
  link_args: link_args,
)

calibrate_exe = executable('calibrate',
  sources: 'calibrate.c',
  c_args: cflags,
  link_args: link_args,
)

# Strip
custom_target('strip',
  output: 'stripped',
  command: [
    'strip',
    shell_exe.full_path(),
    process_exe.full_path(),
    process2_exe.full_path(),
    calibrate_exe.full_path(),
  ],
  build_by_default: false,
)
