cc = meson.get_compiler('cpp')
libutil = cc.find_library('util', required: true)
add_global_arguments('-D_POSIX_C_SOURCE', language : 'cpp')
add_global_arguments('-D_DEFAULT_SOURCE', language : 'cpp')

src = [
  'child.cc',
  'child_manager.cc',
  'childs_tty.cc',
  'event_manager.cc',
  'io.cc',
  'locker_mgr.cc',
  'main.cc',
  'ringbuffer.cc',
  'signal_event_handler.cc',
  ]

executable('tlockd',
  src,
  dependencies: [ libutil ],
  )
