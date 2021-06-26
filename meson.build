project('tlib', 'c',
        version : '1.0.0',
        meson_version : '>= 0.49.2',
        default_options : [
                'buildtype=debugoptimized',
                'warning_level=2',
                'c_std=gnu99'
        ]
)

cc = meson.get_compiler('c')

cc_can_run = not meson.is_cross_build() or meson.has_exe_wrapper()

host_system = host_machine.system()

tlib_version = meson.project_version()
version_arr = tlib_version.split('.')
major_version = version_arr[0].to_int()
minor_version = version_arr[1].to_int()
micro_version = version_arr[2].to_int()

soversion = 1
tlib_api_version = '2.0'

configinc = include_directories('.')
tlibinc = include_directories('tlib')
gioinc = include_directories('tio')

tlib_prefix = get_option('prefix')
tlib_bindir = join_paths(tlib_prefix, get_option('bindir'))
tlib_libdir = join_paths(tlib_prefix, get_option('libdir'))
tlib_libexecdir = join_paths(tlib_prefix, get_option('libexecdir'))
tlib_datadir = join_paths(tlib_prefix, get_option('datadir'))
tlib_pkgdatadir = join_paths(tlib_datadir, 'tlib-1.0')
tlib_includedir = join_paths(tlib_prefix, get_option('includedir'))

if get_option('tio_module_dir') != ''
        tlib_tiomodulesdir = join_paths(tlib_prefix, get_option('tio_module_dir'))
else
        tlib_tiomodulesdir = join_paths(tlib_libdir, 'tio', 'modules')
endif

tlib_pkgconfigreldir = join_paths(tlib_libdir, 'pkgconfig')

# installed_tests_metadir = join_paths(tlib_datadir, 'installed-tests', meson.project_name())
# installed_tests_execdir = join_paths(tlib_libexecdir, 'installed-tests', meson.project_name())
# installed_tests_enabled = get_option('installed_tests')
# installed_tests_template = files('template.test.in')
# installed_tests_template_tap = files('template-tap.test.in')

# Don’t build the tests unless we can run them (either natively, in an exe wrapper, or by installing them for later use)
# build_tests = get_option('tests') and (not meson.is_cross_build() or (meson.is_cross_build() and meson.has_exe_wrapper()) or installed_tests_enabled)

add_project_arguments('-D_GNU_SOURCE', language: 'c')

########################
# Configuration begins #
########################
tlib_conf = configuration_data()
tlibconfig_conf = configuration_data()

# accumulated list of defines as we check for them, so we can easily
# use them later in test programs (autoconf does this automatically)
tlib_conf_prefix = ''

tlib_conf.set('TLIB_MAJOR_VERSION', major_version)
tlib_conf.set('TLIB_MINOR_VERSION', minor_version)
tlib_conf.set('TLIB_MICRO_VERSION', micro_version)
tlib_conf.set_quoted('GETTEXT_PACKAGE', 'tlib10')
tlib_conf.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/acafons/toca/issues')
tlib_conf.set_quoted('PACKAGE_NAME', 'tlib')
tlib_conf.set_quoted('PACKAGE_STRING', 'tlib @0@'.format(meson.project_version()))
tlib_conf.set_quoted('PACKAGE_TARNAME', 'tlib')
tlib_conf.set_quoted('PACKAGE_URL', '')
tlib_conf.set_quoted('PACKAGE_VERSION', meson.project_version())

# Detect and set symbol visibility
tlib_hidden_visibility_args = []

if get_option('default_library') != 'static'
        if host_system == 'windows' or host_system == 'cygwin'
                tlib_conf.set('DLL_EXPORT', true)
                if cc.get_id() == 'msvc' or cc.get_id() == 'clang-cl'
                        tlib_conf.set('_TLIB_EXTERN', '__declspec(dllexport) extern')
                elif cc.has_argument('-fvisibility=hidden')
                        tlib_conf.set('_TLIB_EXTERN', '__attribute__((visibility("default"))) __declspec(dllexport) extern')
                        tlib_hidden_visibility_args = ['-fvisibility=hidden']
                endif
        elif cc.has_argument('-fvisibility=hidden')
                tlib_conf.set('_TLIB_EXTERN', '__attribute__((visibility("default"))) extern')
                tlib_hidden_visibility_args = ['-fvisibility=hidden']
        endif
endif

# We need to know the CRT being used to determine what .lib files we need on
# Visual Studio for dependencies that don't normally come with pkg-config files
vs_crt = 'release'
vs_crt_opt = get_option('b_vscrt')
if vs_crt_opt in ['mdd', 'mtd']
        vs_crt = 'debug'
elif vs_crt_opt == 'from_buildtype'
        if get_option('buildtype') == 'debug'
                vs_crt = 'debug'
        endif
endif

# Use debug/optimization flags to determine whether to enable debug or disable
# cast checks
tlib_debug_cflags = []
tlib_debug = get_option('tlib_debug')
if tlib_debug.enabled() or (tlib_debug.auto() and get_option('debug'))
        tlib_debug_cflags += ['-DT_ENABLE_DEBUG']
        message('Enabling various debug infrastructure')
elif get_option('optimization') in ['2', '3', 's']
        tlib_debug_cflags += ['-DT_DISABLE_CAST_CHECKS']
        message('Disabling cast checks')
endif

if not get_option('asserts')
        tlib_debug_cflags += ['-DNDEBUG']
        message('Disabling asserts')
endif

if not get_option('tlib_checks')
        tlib_debug_cflags += ['-DT_DISABLE_CHECKS']
        message('Disabling TLib checks')
endif

add_project_arguments(tlib_debug_cflags, language: 'c')

# check for header files

headers = [
        'stddef.h',
        'stdbool.h',
        'stdlib.h',
        'string.h',
        'strings.h',
        'stdarg.h',
        'ctype.h',
        'stdint.h',
        'stdio.h',
]

foreach h : headers
        if cc.has_header(h)
                define = 'HAVE_' + h.underscorify().to_upper()
                tlib_conf.set(define, 1)
                tlib_conf_prefix = tlib_conf_prefix + '#define @0@ 1\n'.format(define)
        endif
endforeach

# Compiler flags
if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
        warning_c_args = [
                '-Wduplicated-branches',
                '-Wimplicit-fallthrough',
                '-Wmisleading-indentation',
                '-Wunused',
                '-Wno-unused-parameter',
                '-Wbad-function-cast',
                '-Wcast-function-type',
                '-Wformat-zero-length',
                #'-Werror=declaration-after-statement',
                '-Werror=format=2',
                '-Werror=implicit-function-declaration',
                '-Werror=init-self',
                '-Werror=missing-include-dirs',
                '-Werror=pointer-arith',
                #'-Wall',
                '-Werror',
                #'-Wextra',
        ]

        warning_c_link_args = [
                '-Wl,-z,nodelete',
        ]
else
        warning_c_args = []
        warning_c_link_args = []
endif

add_project_arguments(cc.get_supported_arguments(warning_c_args), language: 'c')

tlib_link_flags = cc.get_supported_link_arguments(warning_c_link_args)

functions = [
        'malloc',
        'calloc',
        'strdup',
        'memcmp',
        'memcpy',
        'strcpy',
        'strncpy',
        'strcmp',
        'strncmp',
        'strcasecmp',
        'index',
        'strstr',
        'rindex',
        'strtok_r',
        'vsnprintf',
]

foreach f : functions
        if cc.has_function(f)
                define = 'HAVE_' + f.underscorify().to_upper()
                tlib_conf.set(define, 1)
                tlib_conf_prefix = tlib_conf_prefix + '#define @0@ 1\n'.format(define)
                set_variable('have_func_' + f, true)
        else
                set_variable('have_func_' + f, false)
        endif
endforeach

tlibconfig_conf.set('TLIB_MAJOR_VERSION', major_version)
tlibconfig_conf.set('TLIB_MINOR_VERSION', minor_version)
tlibconfig_conf.set('TLIB_MICRO_VERSION', micro_version)
tlibconfig_conf.set('TLIB_VERSION', tlib_version)

test_timeout = 60
test_timeout_slow = 180

configure_file(output : 'config.h', configuration : tlib_conf)

pkg = import('pkgconfig')
#windows = import('windows')
subdir('tlib')
#subdir('tio')
#subdir('fuzzing')
#if build_tests
#  subdir('tests')
#endif
