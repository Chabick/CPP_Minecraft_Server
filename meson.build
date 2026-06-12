project('MCServer', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

compiler = meson.get_compiler('cpp')
ws_lib = compiler.find_library('ws2_32', required: true)
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
encryption_proj = subproject('cryptolib')
crypto_lib = encryption_proj.get_variable('cryptolib_dep')

MCServer = executable('MCServer', sources: src, install : true, dependencies: [ws_lib])

test('test', MCServer)