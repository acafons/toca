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
)
install_headers(tlib_sub_headers, subdir : 'tlib-1.0/tlib')

tlib_sources = files(
  'tobject.c',
)

tlib_c_args = ['-DTLIB_COMPILATION'] + tlib_hidden_visibility_args
libtlib = library('tlib-1.0',
  sources : [tlib_sources],
  version : library_version,
  soversion : soversion,
  darwin_versions : darwin_versions,
  install : true,
  # intl.lib is not compatible with SAFESEH
  link_args : [tlib_link_flags],
  include_directories : configinc,
  # dependencies : pcre_deps + [thread_dep, librt] + libintl_deps + libiconv + platform_deps + [gnulib_libm_dependency, libm] + [libsysprof_capture_dep],
  c_args : tlib_c_args,
  objc_args : tlib_c_args,
)

libtlib_dep = declare_dependency(
  link_with : libtlib,
  # We sadly need to export configina here because everyone includes <tlib/*.h>
  include_directories : [configinc, tlibinc])

pkg.generate(libtlib,
  subdirs : ['tlib-1.0'],
  variables : ['bindir=' + join_paths('${prefix}', get_option('bindir'))],
  version : tlib_version,
  install_dir : tlib_pkgconfigreldir,
  filebase : 'tlib-1.0',
  name : 'TLib',
  description : 'Tiny Object in C API (TOCA)',
)

