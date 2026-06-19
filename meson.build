project('MCServer', 'cpp', 'c',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

if host_machine.system() == 'windows'
        compiler = meson.get_compiler('cpp')
        ws_lib = compiler.find_library('ws2_32', required: true)
else
endif

#threw an error
#dep_cjson = dependency('libcjson')

src = files('main.cpp')

subdir('Types')
subdir('Util')
subdir('Network')
subdir('Data')
subdir('Minecraft')
subdir('Systems')

#has some error, i don't know how to implement this
#encryption_proj = subproject('cryptolib')
#crypto_lib = encryption_proj.get_variable('cryptolib_dep')

if host_machine.system() == 'windows'
        MCServer = executable('MCServer', sources: src, install : true, dependencies: [ws_lib])
else
        MCServer = executable('MCServer', sources: src, install : true)
endif

test('test', MCServer)