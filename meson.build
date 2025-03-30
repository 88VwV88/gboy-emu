project('gboy', ['cpp'], default_options: ['cpp_std=c++23'])

instr_set = library('instr_set', 'src/core/instructions.cpp')
sdl2_dep = dependency('SDL2')
executable(
  'gboy',
  'src/main.cpp',
  link_with: instr_set,
  dependencies: sdl2_dep,
)