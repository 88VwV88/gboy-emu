project('gboy', ['c', 'cpp'], default_options: ['c_std=c23', 'cpp_std=c++23'])
sdl2_dep = dependency('SDL2')
executable(
  'gboy',
  'src/main.cpp',
  dependencies: sdl2_dep,
)