project('tlockd', ['c', 'cpp'],
  default_options: [
    'c_std=c99',
    'cpp_std=c++17',
    ]
  )

warn_flags = [
  '-Wall',
  '-Wextra',
  '-pedantic',
  ]

foreach lang : ['c', 'cpp']
  compiler = meson.get_compiler(lang)
  foreach flag : warn_flags
    if compiler.has_argument(flag)
      add_global_arguments(flag, language : lang)
    endif
  endforeach
endforeach

subdir('src')

install_data('tlock', install_dir : 'bin')
