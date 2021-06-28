configure_file(input : 'tlibconfig.h.in', output : 'tlibconfig.h',
        install_dir : join_paths(get_option('libdir'), 'tlib-1.0/include'),
        configuration : tlibconfig_conf)

tlib_headers = files(
        'tlib.h',
)
install_headers(tlib_headers, subdir : 'tlib-1.0')

tlib_sub_headers = files(
        'tobject.h',
        'tobject-p.h',
        'tversion.h',
        'tstring.h',
)
install_headers(tlib_sub_headers, subdir : 'tlib-1.0/tlib')

tlib_sources = files(
        'tobject.c',
        'tstring.c'
)

tlib_c_args = ['-DTLIB_COMPILATION'] + tlib_hidden_visibility_args
libtlib = library('tlib-1.0',
        sources : [tlib_sources],
        version : tlib_version,
        soversion : soversion,
        install : true,
        link_args : [tlib_link_flags],
        include_directories : configinc,
        c_args : tlib_c_args,
        objc_args : tlib_c_args,
)

libtlib_dep = declare_dependency(
        link_with : libtlib,
        include_directories : [configinc, tlibinc]
)

pkg.generate(libtlib,
        subdirs : ['tlib-1.0'],
        variables : ['bindir=' + join_paths('${prefix}', get_option('bindir'))],
        version : tlib_version,
        install_dir : tlib_pkgconfigreldir,
        filebase : 'tlib-1.0',
        name : 'tlib',
        description : 'Tiny Object in C API (TOCA)',
)

if build_tests
        cmocka_dep = dependency('cmocka')
        subdir('tests')
endif
