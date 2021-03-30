project('toca', 'c', 'cpp',
  version : '1.0.0',
  # NOTE: We keep this pinned at 0.49 because that's what Debian 10 ships
  meson_version : '>= 0.49.2',
  default_options : [
    'buildtype=debugoptimized',
    'warning_level=1',
    'c_std=gnu99'
  ]
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

cc_can_run = not meson.is_cross_build() or meson.has_exe_wrapper()

host_system = host_machine.system()

tlib_version = meson.project_version()
tlib_api_version = '1.0'
version_arr = tlib_version.split('.')
major_version = version_arr[0].to_int()
minor_version = version_arr[1].to_int()
micro_version = version_arr[2].to_int()

interface_age = minor_version.is_odd() ? 0 : micro_version
binary_age = 100 * minor_version + micro_version

soversion = 0

current = binary_age - interface_age
library_version = '@0@.@1@.@2@'.format(soversion, current, interface_age)
darwin_versions = [current + 1, '@0@.@1@'.format(current + 1, interface_age)]

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

if host_system == 'qnx'
  add_project_arguments('-D_QNX_SOURCE', language: 'c')
endif

# Disable strict aliasing;
# see https://bugzilla.gnome.org/show_bug.cgi?id=791622
if cc.has_argument('-fno-strict-aliasing')
  add_project_arguments('-fno-strict-aliasing', language: 'c')
endif

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
tlib_conf.set('TLIB_INTERFACE_AGE', interface_age)
tlib_conf.set('TLIB_BINARY_AGE', binary_age)
tlib_conf.set_quoted('GETTEXT_PACKAGE', 'tlib10')
tlib_conf.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/acafons/toca/issues')
tlib_conf.set_quoted('PACKAGE_NAME', 'tlib')
tlib_conf.set_quoted('PACKAGE_STRING', 'tlib @0@'.format(meson.project_version()))
tlib_conf.set_quoted('PACKAGE_TARNAME', 'tlib')
tlib_conf.set_quoted('PACKAGE_URL', '')
tlib_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
tlib_conf.set('ENABLE_NLS', 1)

# used by the .rc.in files
tlibconfig_conf.set('LT_CURRENT_MINUS_AGE', soversion)

tlib_conf.set('_GNU_SOURCE', 1)

if host_system == 'windows'
  # Poll doesn't work on devices on Windows
  tlib_conf.set('BROKEN_POLL', true)
endif

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

if get_option('default_library') == 'static'
    tlibconfig_conf.set('TLIB_STATIC_COMPILATION', '1')
    tlibconfig_conf.set('GOBJECT_STATIC_COMPILATION', '1')
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

if not get_option('tlib_assert')
  tlib_debug_cflags += ['-DT_DISABLE_ASSERT']
  message('Disabling GLib asserts')
endif

if not get_option('tlib_checks')
  tlib_debug_cflags += ['-DT_DISABLE_CHECKS']
  message('Disabling GLib checks')
endif

add_project_arguments(tlib_debug_cflags, language: 'c')

# check for header files

headers = [
  'alloca.h',
  'crt_externs.h',
  'dirent.h', # MSC does not come with this by default
  'float.h',
  'fstab.h',
  'grp.h',
  'inttypes.h',
  'limits.h',
  'linux/magic.h',
  'locale.h',
  'mach/mach_time.h',
  'memory.h',
  'mntent.h',
  'poll.h',
  'pwd.h',
  'sched.h',
  'spawn.h',
  'stdatomic.h',
  'stdint.h',
  'stdlib.h',
  'string.h',
  'strings.h',
  'sys/auxv.h',
  'sys/event.h',
  'sys/filio.h',
  'sys/inotify.h',
  'sys/mkdev.h',
  'sys/mntctl.h',
  'sys/mnttab.h',
  'sys/mount.h',
  'sys/param.h',
  'sys/resource.h',
  'sys/select.h',
  'sys/statfs.h',
  'sys/stat.h',
  'sys/statvfs.h',
  'sys/sysctl.h',
  'sys/time.h', # MSC does not come with this by default
  'sys/times.h',
  'sys/types.h',
  'sys/uio.h',
  'sys/vfs.h',
  'sys/vfstab.h',
  'sys/vmount.h',
  'sys/wait.h',
  'termios.h',
  'unistd.h',
  'values.h',
  'wchar.h',
  'xlocale.h',
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
    #'-Wstrict-prototypes',
    '-Wunused',
    # Due to maintained deprecated code, we do not want to see unused parameters
    '-Wno-unused-parameter',
    # Due to pervasive use of things like TPOINTER_TO_UINT(), we do not support
    # building with -Wbad-function-cast.
    '-Wbad-function-cast',
    '-Wcast-function-type',
    # Due to function casts through (void*) we cannot support -Wpedantic:
    # https://wiki.gnome.org/Projects/GLib/CompilerRequirements#Function_pointer_conversions.
    '-Wno-pedantic',
    # A zero-length format string shouldn't be considered an issue.
    '-Wformat-zero-length',
    '-Werror=declaration-after-statement',
    '-Werror=format=2',
    '-Werror=implicit-function-declaration',
    '-Werror=init-self',
    '-Werror=missing-include-dirs',
    # '-Werror=missing-prototypes',
    '-Werror=pointer-arith',
    '-Wall',
    '-Werror',
    '-Wextra',
  ]
  warning_c_link_args = [
    '-Wl,-z,nodelete',
  ]
  # if get_option('bsymbolic_functions')
  #  warning_c_link_args += ['-Wl,-Bsymbolic-functions']
  # endif
else
  warning_c_args = []
  warning_c_link_args = []
endif

add_project_arguments(cc.get_supported_arguments(warning_c_args), language: 'c')

# FIXME: We cannot build some of the GResource tests with -z nodelete, which
# means we cannot use that flag in add_project_link_arguments(), and must add
# it to the relevant targets manually. We do the same with -Bsymbolic-functions
# because that is what the autotools build did.
# See https://github.com/mesonbuild/meson/pull/3520 for a way to eventually
# improve this.
tlib_link_flags = cc.get_supported_link_arguments(warning_c_link_args)

# Windows SDK requirements and checks
if host_system == 'windows'
  # Check whether we're building for UWP apps
  code = '''
  #include <windows.h>
  #if !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
  #error "Not building for UWP"
  #endif'''
  if cc.compiles(code, name : 'building for UWP')
    tlib_conf.set('T_WINAPI_ONLY_APP', true)
    # We require Windows 10+ on WinRT
    tlib_conf.set('_WIN32_WINNT', '0x0A00')
    uwp_gio_deps = [cc.find_library('shcore'),
                    cc.find_library('runtimeobject')]
  else
    # We require Windows 7+ on Win32
    tlib_conf.set('_WIN32_WINNT', '0x0601')
    uwp_gio_deps = []
  endif
endif

functions = [
  'close_range',
  'endmntent',
  'endservent',
  'epoll_create',
  'fallocate',
  'fchmod',
  'fchown',
  'fdwalk',
  'fsync',
  'getauxval',
  'getc_unlocked',
  'getfsstat',
  'getgrgid_r',
  'getmntent_r',
  'getpwuid_r',
  'getresuid',
  'getvfsstat',
  'gmtime_r',
  'hasmntopt',
  'inotify_init1',
  'issetugid',
  'kevent',
  'kqueue',
  'lchmod',
  'lchown',
  'link',
  'localtime_r',
  'lstat',
  'mbrtowc',
  'memalign',
  'mmap',
  'newlocale',
  'pipe2',
  'poll',
  'prlimit',
  'readlink',
  'recvmmsg',
  'sendmmsg',
  'setenv',
  'setmntent',
  'strerror_r',
  'strnlen',
  'strsignal',
  'strtod_l',
  'strtoll_l',
  'strtoull_l',
  'symlink',
  'timegm',
  'unsetenv',
  'uselocale',
  'utimes',
  'valloc',
  'vasprintf',
  'vsnprintf',
  'wcrtomb',
  'wcslen',
  'wcsnlen',
  'sysctlbyname',
]

if tlib_conf.has('HAVE_SYS_STATVFS_H')
  functions += ['statvfs']
else
  have_func_statvfs = false
endif
if tlib_conf.has('HAVE_SYS_STATFS_H') or tlib_conf.has('HAVE_SYS_MOUNT_H')
  functions += ['statfs']
else
  have_func_statfs = false
endif

if host_system == 'windows'
  iphlpapi_dep = cc.find_library('iphlpapi')
  iphlpapi_funcs = ['if_nametoindex', 'if_indextoname']
  foreach ifunc : iphlpapi_funcs
    iphl_prefix =  '''#define _WIN32_WINNT @0@
      #include <winsock2.h>
      #include <iphlpapi.h>'''.format(tlib_conf.get('_WIN32_WINNT'))
    if cc.has_function(ifunc,
                       prefix : iphl_prefix,
                       dependencies : iphlpapi_dep)
      idefine = 'HAVE_' + ifunc.underscorify().to_upper()
      tlib_conf.set(idefine, 1)
      tlib_conf_prefix = tlib_conf_prefix + '#define @0@ 1\n'.format(idefine)
      set_variable('have_func_' + ifunc, true)
    else
      set_variable('have_func_' + ifunc, false)
    endif
  endforeach
else
  functions += ['if_indextoname', 'if_nametoindex']
endif

# AIX splice is something else
if host_system != 'aix'
  functions += ['splice']
endif

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

# Check that stpcpy() is usable; must use header.
# cc.has_function() in some cases (clang, gcc 10+) assumes that if the
# compiler provides a builtin of the same name that the function exists, while
# it's in fact not provided by any header or library. This is true for
# stpcpy() on Windows using clang and gcc as well as posix_memalign() using
# gcc on Windows. Skip these checks on Windows for now to avoid false
# positives. See https://github.com/mesonbuild/meson/pull/7116,
# https://github.com/mesonbuild/meson/issues/3672 and
# https://github.com/mesonbuild/meson/issues/5628.
# FIXME: Once meson no longer returns success for stpcpy() and
# posix_memalign() on Windows using GCC and clang we can remove this.
if host_system != 'windows' and cc.has_function('stpcpy', prefix : '#include <string.h>')
  tlib_conf.set('HAVE_STPCPY', 1)
endif

# When building for Android-20 and earlier, require Meson 0.54.2 or newer.
# This is needed, because Meson build versions prior to 0.54.2 return false
# positive for stpcpy has_function check when building for legacy Android.
if host_system == 'android'
    android_is_older = cc.compiles('''#if __ANDROID_API__ >= 21
                                        #error Android is 21 or newer
                                    #endif''')
    if android_is_older and meson.version().version_compare('< 0.54.2')
      error('Compiling for <Android-21 requires Meson 0.54.2 or newer')
    endif
endif

# Check that posix_memalign() is usable; must use header
if host_system != 'windows' and cc.has_function('posix_memalign', prefix : '#include <stdlib.h>')
  tlib_conf.set('HAVE_POSIX_MEMALIGN', 1)
endif

# Check that posix_spawn() is usable; must use header
if cc.has_function('posix_spawn', prefix : '#include <spawn.h>')
  tlib_conf.set('HAVE_POSIX_SPAWN', 1)
endif

# Check whether strerror_r returns char *
if have_func_strerror_r
  if cc.compiles('''#define _GNU_SOURCE
                    #include <string.h>
                    int func (void) {
                      char error_string[256];
                      char *ptr = strerror_r (-2, error_string, 256);
                      char c = *strerror_r (-2, error_string, 256);
                      return c != 0 && ptr != (void*) 0L;
                    }
                 ''',
                 name : 'strerror_r() returns char *')
    tlib_conf.set('STRERROR_R_CHAR_P', 1,
                  description: 'Defined if strerror_r returns char *')
  endif
endif

# Special-case these functions that have alternative names on Windows/MSVC
if cc.has_function('snprintf') or cc.has_header_symbol('stdio.h', 'snprintf')
  tlib_conf.set('HAVE_SNPRINTF', 1)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_SNPRINTF 1\n'
elif cc.has_function('_snprintf') or cc.has_header_symbol('stdio.h', '_snprintf')
  hack_define = '1\n#define snprintf _snprintf'
  tlib_conf.set('HAVE_SNPRINTF', hack_define)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_SNPRINTF ' + hack_define
endif

if cc.has_function('strcasecmp')
  tlib_conf.set('HAVE_STRCASECMP', 1)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_STRCASECMP 1\n'
elif cc.has_function('_stricmp')
  hack_define = '1\n#define strcasecmp _stricmp'
  tlib_conf.set('HAVE_STRCASECMP', hack_define)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_STRCASECMP ' + hack_define
endif

if cc.has_function('strncasecmp')
  tlib_conf.set('HAVE_STRNCASECMP', 1)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_STRNCASECMP 1\n'
elif cc.has_function('_strnicmp')
  hack_define = '1\n#define strncasecmp _strnicmp'
  tlib_conf.set('HAVE_STRNCASECMP', hack_define)
  tlib_conf_prefix = tlib_conf_prefix + '#define HAVE_STRNCASECMP ' + hack_define
endif

if cc.has_header_symbol('sys/sysmacros.h', 'major')
  tlib_conf.set('MAJOR_IN_SYSMACROS', 1)
elif cc.has_header_symbol('sys/mkdev.h', 'major')
  tlib_conf.set('MAJOR_IN_MKDEV', 1)
elif cc.has_header_symbol('sys/types.h', 'major')
  tlib_conf.set('MAJOR_IN_TYPES', 1)
endif

if cc.has_header_symbol('dlfcn.h', 'RTLD_LAZY')
  tlib_conf.set('HAVE_RTLD_LAZY', 1)
endif

if cc.has_header_symbol('dlfcn.h', 'RTLD_NOW')
  tlib_conf.set('HAVE_RTLD_NOW', 1)
endif

if cc.has_header_symbol('dlfcn.h', 'RTLD_GLOBAL')
  tlib_conf.set('HAVE_RTLD_GLOBAL', 1)
endif

have_rtld_next = false
if cc.has_header_symbol('dlfcn.h', 'RTLD_NEXT', args: '-D_GNU_SOURCE')
  have_rtld_next = true
  tlib_conf.set('HAVE_RTLD_NEXT', 1)
endif

# Check whether to use statfs or statvfs
# Some systems have both statfs and statvfs, pick the most "native" for these
if have_func_statfs and have_func_statvfs
  # on solaris and irix, statfs doesn't even have the f_bavail field
  if not tlib_conf.has('HAVE_STRUCT_STATFS_F_BAVAIL')
    have_func_statfs = false
  else
    # at least on linux, statfs is the actual syscall
    have_func_statvfs = false
  endif
endif
if have_func_statfs
  tlib_conf.set('USE_STATFS', 1)
  stat_func_to_use = 'statfs'
elif have_func_statvfs
  tlib_conf.set('USE_STATVFS', 1)
  stat_func_to_use = 'statvfs'
else
  stat_func_to_use = 'neither'
endif
message('Checking whether to use statfs or statvfs .. ' + stat_func_to_use)

if host_system == 'linux'
  if cc.has_function('mkostemp',
                     prefix: '''#define _GNU_SOURCE
                                #include <stdlib.h>''')
    tlib_conf.set('HAVE_MKOSTEMP', 1)
  endif
endif

osx_ldflags = []
tlib_have_os_x_9_or_later = false
tlib_have_carbon = false
tlib_have_cocoa = false
if host_system == 'darwin'
  add_languages('objc')
  objcc = meson.get_compiler('objc')

  osx_ldflags += ['-Wl,-framework,CoreFoundation']

  # Mac OS X Carbon support
  tlib_have_carbon = objcc.compiles('''#include <Carbon/Carbon.h>
                                       #include <CoreServices/CoreServices.h>''',
                                    name : 'Mac OS X Carbon support')

  if tlib_have_carbon
    tlib_conf.set('HAVE_CARBON', true)
    osx_ldflags += '-Wl,-framework,Carbon'
    tlib_have_os_x_9_or_later = objcc.compiles('''#include <AvailabilityMacros.h>
                                                  #if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
                                                  #error Compiling for minimum OS X version before 10.9
                                                  #endif''',
                                               name : 'OS X 9 or later')
  endif

  # Mac OS X Cocoa support
  tlib_have_cocoa = objcc.compiles('''#include <Cocoa/Cocoa.h>
                                      #ifdef GNUSTEP_BASE_VERSION
                                      #error "Detected GNUstep, not Cocoa"
                                      #endif''',
                                   name : 'Mac OS X Cocoa support')

  if tlib_have_cocoa
    tlib_conf.set('HAVE_COCOA', true)
    osx_ldflags += ['-Wl,-framework,Foundation', '-Wl,-framework,AppKit']
  endif

  # FIXME: libgio mix C and objC source files and there is no way to reliably
  # know which language flags it's going to use to link. Add to both languages
  # for now. See https://github.com/mesonbuild/meson/issues/3585.
  add_project_link_arguments(osx_ldflags, language : ['objc', 'c'])
endif

if host_system == 'qnx'
  tlib_conf.set('HAVE_QNX', 1)
endif

# Check for futex(2)
if cc.links('''#include <linux/futex.h>
               #include <sys/syscall.h>
               #include <unistd.h>
               int main (int argc, char ** argv) {
                 syscall (__NR_futex, NULL, FUTEX_WAKE, FUTEX_WAIT);
                 return 0;
               }''', name : 'futex(2) system call')
  tlib_conf.set('HAVE_FUTEX', 1)
endif

# Check for eventfd(2)
if cc.links('''#include <sys/eventfd.h>
               #include <unistd.h>
               int main (int argc, char ** argv) {
                 eventfd (0, EFD_CLOEXEC);
                 return 0;
               }''', name : 'eventfd(2) system call')
  tlib_conf.set('HAVE_EVENTFD', 1)
endif

# Check for __uint128_t (gcc) by checking for 128-bit division
uint128_t_src = '''int main() {
static __uint128_t v1 = 100;
static __uint128_t v2 = 10;
static __uint128_t u;
u = v1 / v2;
}'''
if cc.compiles(uint128_t_src, name : '__uint128_t available')
  tlib_conf.set('HAVE_UINT128_T', 1)
endif

clock_gettime_test_code = '''
  #include <time.h>
  struct timespec t;
  int main (int argc, char ** argv) {
    return clock_gettime(CLOCK_REALTIME, &t);
  }'''
librt = []
if cc.links(clock_gettime_test_code, name : 'clock_gettime')
  tlib_conf.set('HAVE_CLOCK_GETTIME', 1)
elif cc.links(clock_gettime_test_code, args : '-lrt', name : 'clock_gettime in librt')
  tlib_conf.set('HAVE_CLOCK_GETTIME', 1)
  librt = cc.find_library('rt')
endif

dlopen_dlsym_test_code = '''
#include <dlfcn.h>
int tlib_underscore_test (void) { return 42; }
int main (int argc, char ** argv) {
  void *f1 = (void*)0, *f2 = (void*)0, *handle;
  handle = dlopen ((void*)0, 0);
  if (handle) {
    f1 = dlsym (handle, "tlib_underscore_test");
    f2 = dlsym (handle, "_tlib_underscore_test");
  }
  return (!f2 || f1);
}'''
libdl_dep = []
if cc.links(dlopen_dlsym_test_code, name : 'dlopen() and dlsym() in system libraries')
  have_dlopen_dlsym = true
elif cc.links(dlopen_dlsym_test_code, args : '-ldl', name : 'dlopen() and dlsym() in libdl')
  have_dlopen_dlsym = true
  libdl_dep = cc.find_library('dl')
else
  have_dlopen_dlsym = false
endif

# if statfs() takes 2 arguments (Posix) or 4 (Solaris)
if have_func_statfs
  if cc.compiles(tlib_conf_prefix + '''
                 #include <unistd.h>
                        #ifdef HAVE_SYS_PARAM_H
                        #include <sys/param.h>
                        #endif
                        #ifdef HAVE_SYS_VFS_H
                        #include <sys/vfs.h>
                        #endif
                        #ifdef HAVE_SYS_MOUNT_H
                        #include <sys/mount.h>
                        #endif
                        #ifdef HAVE_SYS_STATFS_H
                        #include <sys/statfs.h>
                        #endif
                        void some_func (void) {
                          struct statfs st;
                          statfs("/", &st);
                        }''', name : 'number of arguments to statfs() (n=2)')
    tlib_conf.set('STATFS_ARGS', 2)
  elif cc.compiles(tlib_conf_prefix + '''
                   #include <unistd.h>
                          #ifdef HAVE_SYS_PARAM_H
                          #include <sys/param.h>
                          #endif
                          #ifdef HAVE_SYS_VFS_H
                          #include <sys/vfs.h>
                          #endif
                          #ifdef HAVE_SYS_MOUNT_H
                          #include <sys/mount.h>
                          #endif
                          #ifdef HAVE_SYS_STATFS_H
                          #include <sys/statfs.h>
                          #endif
                          void some_func (void) {
                            struct statfs st;
                            statfs("/", &st, sizeof (st), 0);
                          }''', name : 'number of arguments to statfs() (n=4)')
    tlib_conf.set('STATFS_ARGS', 4)
  else
    error('Unable to determine number of arguments to statfs()')
  endif
endif

# open takes O_DIRECTORY as an option
#AC_MSG_CHECKING([])
if cc.compiles('''#include <fcntl.h>
                  #include <sys/types.h>
                  #include <sys/stat.h>
                  void some_func (void) {
                    open(0, O_DIRECTORY, 0);
                  }''', name : 'open() option O_DIRECTORY')
  tlib_conf.set('HAVE_OPEN_O_DIRECTORY', 1)
endif

# fcntl takes F_FULLFSYNC as an option
# See https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fsync.2.html
if cc.compiles('''#include <fcntl.h>
                  #include <sys/types.h>
                  #include <sys/stat.h>
                  void some_func (void) {
                    fcntl(0, F_FULLFSYNC, 0);
                  }''', name : 'fcntl() option F_FULLFSYNC')
  tlib_conf.set('HAVE_FCNTL_F_FULLFSYNC', 1)
endif

# Check whether there is a vsnprintf() function with C99 semantics installed.
# (similar tests to AC_FUNC_VSNPRINTF_C99)
# Check whether there is a snprintf() function with C99 semantics installed.
# (similar tests to AC_FUNC_SNPRINTF_C99)
# Check whether there is a printf() function with Unix98 semantics installed.
# (similar tests to AC_FUNC_PRINTF_UNIX98)
have_good_vsnprintf = false
have_good_snprintf = false
have_good_printf = false

if host_system == 'windows' and (cc.get_id() == 'msvc' or cc.get_id() == 'clang-cl')
  # Unfortunately the Visual Studio 2015+ implementations of C99-style
  # snprintf and vsnprintf don't seem to be quite good enough.
  # (Sorry, I don't know exactly what is the problem,
  # but it is related to floating point formatting and decimal point vs. comma.)
  # The simple tests in AC_FUNC_VSNPRINTF_C99 and AC_FUNC_SNPRINTF_C99 aren't
  # rigorous enough to notice, though.
  tlib_conf.set('HAVE_C99_SNPRINTF', false)
  tlib_conf.set('HAVE_C99_VSNPRINTF', false)
  tlib_conf.set('HAVE_UNIX98_PRINTF', false)
elif not cc_can_run and host_system in ['ios', 'darwin']
  # All these are true when compiling natively on macOS, so we should use good
  # defaults when building for iOS and tvOS.
  tlib_conf.set('HAVE_C99_SNPRINTF', true)
  tlib_conf.set('HAVE_C99_VSNPRINTF', true)
  tlib_conf.set('HAVE_UNIX98_PRINTF', true)
  have_good_vsnprintf = true
  have_good_snprintf = true
  have_good_printf = true
else
  vsnprintf_c99_test_code = '''
#include <stdio.h>
#include <stdarg.h>

int
doit(char * s, ...)
{
  char buffer[32];
  va_list args;
  int r;

  va_start(args, s);
  r = vsnprintf(buffer, 5, s, args);
  va_end(args);

  if (r != 7)
    exit(1);

  /* AIX 5.1 and Solaris seems to have a half-baked vsnprintf()
     implementation. The above will return 7 but if you replace
     the size of the buffer with 0, it borks! */
  va_start(args, s);
  r = vsnprintf(buffer, 0, s, args);
  va_end(args);

  if (r != 7)
    exit(1);

  exit(0);
}

int
main(void)
{
  doit("1234567");
  exit(1);
}'''

  if cc_can_run
    rres = cc.run(vsnprintf_c99_test_code, name : 'C99 vsnprintf')
    if rres.compiled() and rres.returncode() == 0
      tlib_conf.set('HAVE_C99_VSNPRINTF', 1)
      have_good_vsnprintf = true
    endif
  else
      have_good_vsnprintf = meson.get_cross_property('have_c99_vsnprintf', false)
      tlib_conf.set('HAVE_C99_VSNPRINTF', have_good_vsnprintf)
  endif

  snprintf_c99_test_code = '''
#include <stdio.h>
#include <stdarg.h>

int
doit()
{
  char buffer[32];
  va_list args;
  int r;

  r = snprintf(buffer, 5, "1234567");

  if (r != 7)
    exit(1);

  r = snprintf(buffer, 0, "1234567");

  if (r != 7)
    exit(1);

  r = snprintf(NULL, 0, "1234567");

  if (r != 7)
    exit(1);

  exit(0);
}

int
main(void)
{
  doit();
  exit(1);
}'''

  if cc_can_run
    rres = cc.run(snprintf_c99_test_code, name : 'C99 snprintf')
    if rres.compiled() and rres.returncode() == 0
      tlib_conf.set('HAVE_C99_SNPRINTF', 1)
      have_good_snprintf = true
    endif
  else
      have_good_snprintf = meson.get_cross_property('have_c99_snprintf', false)
      tlib_conf.set('HAVE_C99_SNPRINTF', have_good_snprintf)
  endif

  printf_unix98_test_code = '''
#include <stdio.h>

int
main (void)
{
  char buffer[128];

  sprintf (buffer, "%2\$d %3\$d %1\$d", 1, 2, 3);
  if (strcmp ("2 3 1", buffer) == 0)
    exit (0);
  exit (1);
}'''

  if cc_can_run
    rres = cc.run(printf_unix98_test_code, name : 'Unix98 printf positional parameters')
    if rres.compiled() and rres.returncode() == 0
      tlib_conf.set('HAVE_UNIX98_PRINTF', 1)
      have_good_printf = true
    endif
  else
      have_good_printf = meson.get_cross_property('have_unix98_printf', false)
      tlib_conf.set('HAVE_UNIX98_PRINTF', have_good_printf)
  endif
endif

if host_system == 'windows'
  tlib_conf.set_quoted('EXEEXT', '.exe')
else
  tlib_conf.set('EXEEXT', '')
endif

# Our printf is 'good' only if vsnpintf()/snprintf()/printf() supports C99 well enough
use_system_printf = have_good_vsnprintf and have_good_snprintf and have_good_printf
tlib_conf.set('USE_SYSTEM_PRINTF', use_system_printf)
tlibconfig_conf.set('GLIB_USING_SYSTEM_PRINTF', use_system_printf)

if not use_system_printf
  # gnulib has vasprintf so override the previous check
  tlib_conf.set('HAVE_VASPRINTF', 1)
endif

# Check for nl_langinfo and CODESET
if cc.links('''#include <langinfo.h>
               int main (int argc, char ** argv) {
                 char *codeset = nl_langinfo (CODESET);
                 return 0;
               }''', name : 'nl_langinfo and CODESET')
  tlib_conf.set('HAVE_LANGINFO_CODESET', 1)
  tlib_conf.set('HAVE_CODESET', 1)
endif

# Check for nl_langinfo and LC_TIME parts that are needed in gdatetime.c
if cc.links('''#include <langinfo.h>
               int main (int argc, char ** argv) {
                 char *str;
                 str = nl_langinfo (PM_STR);
                 str = nl_langinfo (D_T_FMT);
                 str = nl_langinfo (D_FMT);
                 str = nl_langinfo (T_FMT);
                 str = nl_langinfo (T_FMT_AMPM);
                 str = nl_langinfo (MON_1);
                 str = nl_langinfo (ABMON_12);
                 str = nl_langinfo (DAY_1);
                 str = nl_langinfo (ABDAY_7);
                 return 0;
               }''', name : 'nl_langinfo (PM_STR)')
  tlib_conf.set('HAVE_LANGINFO_TIME', 1)
endif
if cc.links('''#include <langinfo.h>
               int main (int argc, char ** argv) {
                 char *str;
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT0_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT1_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT2_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT3_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT4_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT5_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT6_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT7_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT8_MB);
                 str = nl_langinfo (_NL_CTYPE_OUTDIGIT9_MB);
                 return 0;
               }''', name : 'nl_langinfo (_NL_CTYPE_OUTDIGITn_MB)')
  tlib_conf.set('HAVE_LANGINFO_OUTDIGIT', 1)
endif

# Check for nl_langinfo and alternative month names
if cc.links('''#ifndef _GNU_SOURCE
              # define _GNU_SOURCE
              #endif
              #include <langinfo.h>
               int main (int argc, char ** argv) {
                 char *str;
                 str = nl_langinfo (ALTMON_1);
                 str = nl_langinfo (ALTMON_2);
                 str = nl_langinfo (ALTMON_3);
                 str = nl_langinfo (ALTMON_4);
                 str = nl_langinfo (ALTMON_5);
                 str = nl_langinfo (ALTMON_6);
                 str = nl_langinfo (ALTMON_7);
                 str = nl_langinfo (ALTMON_8);
                 str = nl_langinfo (ALTMON_9);
                 str = nl_langinfo (ALTMON_10);
                 str = nl_langinfo (ALTMON_11);
                 str = nl_langinfo (ALTMON_12);
                 return 0;
               }''', name : 'nl_langinfo (ALTMON_n)')
  tlib_conf.set('HAVE_LANGINFO_ALTMON', 1)
endif

# Check for nl_langinfo and abbreviated alternative month names
if cc.links('''#ifndef _GNU_SOURCE
              # define _GNU_SOURCE
              #endif
              #include <langinfo.h>
               int main (int argc, char ** argv) {
                 char *str;
                 str = nl_langinfo (_NL_ABALTMON_1);
                 str = nl_langinfo (_NL_ABALTMON_2);
                 str = nl_langinfo (_NL_ABALTMON_3);
                 str = nl_langinfo (_NL_ABALTMON_4);
                 str = nl_langinfo (_NL_ABALTMON_5);
                 str = nl_langinfo (_NL_ABALTMON_6);
                 str = nl_langinfo (_NL_ABALTMON_7);
                 str = nl_langinfo (_NL_ABALTMON_8);
                 str = nl_langinfo (_NL_ABALTMON_9);
                 str = nl_langinfo (_NL_ABALTMON_10);
                 str = nl_langinfo (_NL_ABALTMON_11);
                 str = nl_langinfo (_NL_ABALTMON_12);
                 return 0;
               }''', name : 'nl_langinfo (_NL_ABALTMON_n)')
  tlib_conf.set('HAVE_LANGINFO_ABALTMON', 1)
endif

# Check if C compiler supports the 'signed' keyword
if not cc.compiles('''signed char x;''', name : 'signed')
  tlib_conf.set('signed', '/* NOOP */')
endif

# Check if the ptrdiff_t type exists
if cc.has_header_symbol('stddef.h', 'ptrdiff_t')
  tlib_conf.set('HAVE_PTRDIFF_T', 1)
endif

# Check for sig_atomic_t type
if cc.links('''#include <signal.h>
               #include <sys/types.h>
               sig_atomic_t val = 42;
               int main (int argc, char ** argv) {
                 return val == 42 ? 0 : 1;
               }''', name : 'sig_atomic_t')
  tlib_conf.set('HAVE_SIG_ATOMIC_T', 1)
endif

# Check if 'long long' works
# jm_AC_TYPE_LONG_LONG
if cc.compiles('''long long ll = 1LL;
                  int i = 63;
                  int some_func (void) {
                    long long llmax = (long long) -1;
                    return ll << i | ll >> i | llmax / ll | llmax % ll;
                  }''', name : 'long long')
  tlib_conf.set('HAVE_LONG_LONG', 1)
  have_long_long = true
else
  have_long_long = false
endif

# Test whether the compiler supports the 'long double' type.
if cc.compiles('''/* The Stardent Vistra knows sizeof(long double), but does not support it.  */
                  long double foo = 0.0;
                  /* On Ultrix 4.3 cc, long double is 4 and double is 8.  */
                  int array [2*(sizeof(long double) >= sizeof(double)) - 1];''',
               name : 'long double')
  tlib_conf.set('HAVE_LONG_DOUBLE', 1)
endif

# Test whether <stddef.h> has the 'wchar_t' type.
if cc.has_header_symbol('stddef.h', 'wchar_t')
  tlib_conf.set('HAVE_WCHAR_T', 1)
endif

# Test whether <wchar.h> has the 'wint_t' type.
if cc.has_header_symbol('wchar.h', 'wint_t')
  tlib_conf.set('HAVE_WINT_T', 1)
endif

found_uintmax_t = false

# Define HAVE_INTTYPES_H_WITH_UINTMAX if <inttypes.h> exists,
# doesn't clash with <sys/types.h>, and declares uintmax_t.
# jm_AC_HEADER_INTTYPES_H
if cc.compiles('''#include <sys/types.h>
                  #include <inttypes.h>
                  void some_func (void) {
                    uintmax_t i = (uintmax_t) -1;
                  }''', name : 'uintmax_t in inttypes.h')
  tlib_conf.set('HAVE_INTTYPES_H_WITH_UINTMAX', 1)
  found_uintmax_t = true
endif

# Define HAVE_STDINT_H_WITH_UINTMAX if <stdint.h> exists,
# doesn't clash with <sys/types.h>, and declares uintmax_t.
# jm_AC_HEADER_STDINT_H
if cc.compiles('''#include <sys/types.h>
                  #include <stdint.h>
                  void some_func (void) {
                    uintmax_t i = (uintmax_t) -1;
                  }''', name : 'uintmax_t in stdint.h')
  tlib_conf.set('HAVE_STDINT_H_WITH_UINTMAX', 1)
  found_uintmax_t = true
endif

# Define intmax_t to 'long' or 'long long'
# if it is not already defined in <stdint.h> or <inttypes.h>.
# For simplicity, we assume that a header file defines 'intmax_t' if and
# only if it defines 'uintmax_t'.
if found_uintmax_t
  tlib_conf.set('HAVE_INTMAX_T', 1)
elif have_long_long
  tlib_conf.set('intmax_t', 'long long')
else
  tlib_conf.set('intmax_t', 'long')
endif

char_size = cc.sizeof('char')
short_size = cc.sizeof('short')
int_size = cc.sizeof('int')
voidp_size = cc.sizeof('void*')
long_size = cc.sizeof('long')
if have_long_long
  long_long_size = cc.sizeof('long long')
else
  long_long_size = 0
endif
sizet_size = cc.sizeof('size_t')
if cc.get_id() == 'msvc' or cc.get_id() == 'clang-cl'
  ssizet_size = cc.sizeof('SSIZE_T', prefix : '#include <BaseTsd.h>')
else
  ssizet_size = cc.sizeof('ssize_t', prefix : '#include <unistd.h>')
endif

# Some platforms (Apple) hard-code int64_t to long long instead of
# using long on 64-bit architectures. This can cause type mismatch
# warnings when trying to interface with code using the standard
# library type. Test for the warnings and set gint64 to whichever
# works.
if long_long_size == long_size
  if cc.compiles('''#if defined(_AIX) && !defined(__GNUC__)
                    #pragma options langlvl=stdc99
                    #endif
                    #pragma GCC diagnostic error "-Wincompatible-pointer-types"
                    #include <stdint.h>
                    #include <stdio.h>
                    int main () {
                      int64_t i1 = 1;
                      long *i2 = &i1;
                      return 1;
                    }''', name : 'int64_t is long')
    int64_t_typedef = 'long'
  elif cc.compiles('''#if defined(_AIX) && !defined(__GNUC__)
                      #pragma options langlvl=stdc99
                      #endif
                      #pragma GCC diagnostic error "-Wincompatible-pointer-types"
                      #include <stdint.h>
                      #include <stdio.h>
                      int main () {
                        int64_t i1 = 1;
                        long long *i2 = &i1;
                        return 1;
                      }''', name : 'int64_t is long long')
    int64_t_typedef = 'long long'
  endif
endif

int64_m = 'll'
char_align = cc.alignment('char')
short_align = cc.alignment('short')
int_align = cc.alignment('int')
voidp_align = cc.alignment('void*')
long_align = cc.alignment('long')
long_long_align = cc.alignment('long long')
# NOTE: We don't check for size of __int64 because long long is guaranteed to
# be 64-bit in C99, and it is available on all supported compilers
sizet_align = cc.alignment('size_t')

tlib_conf.set('ALIGNOF_UNSIGNED_LONG', long_align)

tlib_conf.set('SIZEOF_CHAR', char_size)
tlib_conf.set('SIZEOF_INT', int_size)
tlib_conf.set('SIZEOF_SHORT', short_size)
tlib_conf.set('SIZEOF_LONG', long_size)
tlib_conf.set('SIZEOF_LONG_LONG', long_long_size)
tlib_conf.set('SIZEOF_SIZE_T', sizet_size)
tlib_conf.set('SIZEOF_SSIZE_T', ssizet_size)
tlib_conf.set('SIZEOF_VOID_P', voidp_size)
tlib_conf.set('SIZEOF_WCHAR_T', cc.sizeof('wchar_t', prefix: '#include <stddef.h>'))

if short_size == 2
  gint16 = 'short'
  gint16_modifier='h'
  gint16_format='hi'
  guint16_format='hu'
elif int_size == 2
  gint16 = 'int'
  gint16_modifier=''
  gint16_format='i'
  guint16_format='u'
else
  error('Compiler provides no native 16-bit integer type')
endif
tlibconfig_conf.set('gint16', gint16)
tlibconfig_conf.set_quoted('gint16_modifier', gint16_modifier)
tlibconfig_conf.set_quoted('gint16_format', gint16_format)
tlibconfig_conf.set_quoted('guint16_format', guint16_format)

if short_size == 4
  gint32 = 'short'
  gint32_modifier='h'
  gint32_format='hi'
  guint32_format='hu'
  guint32_align = short_align
elif int_size == 4
  gint32 = 'int'
  gint32_modifier=''
  gint32_format='i'
  guint32_format='u'
  guint32_align = int_align
elif long_size == 4
  gint32 = 'long'
  gint32_modifier='l'
  gint32_format='li'
  guint32_format='lu'
  guint32_align = long_align
else
  error('Compiler provides no native 32-bit integer type')
endif
tlibconfig_conf.set('gint32', gint32)
tlibconfig_conf.set_quoted('gint32_modifier', gint32_modifier)
tlibconfig_conf.set_quoted('gint32_format', gint32_format)
tlibconfig_conf.set_quoted('guint32_format', guint32_format)
tlib_conf.set('ALIGNOF_GUINT32', guint32_align)

if int_size == 8
  gint64 = 'int'
  gint64_modifier=''
  gint64_format='i'
  guint64_format='u'
  tlib_extension=''
  gint64_constant='(val)'
  guint64_constant='(val)'
  guint64_align = int_align
elif long_size == 8 and (long_long_size != long_size or int64_t_typedef == 'long')
  gint64 = 'long'
  tlib_extension=''
  gint64_modifier='l'
  gint64_format='li'
  guint64_format='lu'
  gint64_constant='(val##L)'
  guint64_constant='(val##UL)'
  guint64_align = long_align
elif long_long_size == 8 and (long_long_size != long_size or int64_t_typedef == 'long long')
  gint64 = 'long long'
  tlib_extension='G_GNUC_EXTENSION '
  gint64_modifier=int64_m
  gint64_format=int64_m + 'i'
  guint64_format=int64_m + 'u'
  gint64_constant='(G_GNUC_EXTENSION (val##LL))'
  guint64_constant='(G_GNUC_EXTENSION (val##ULL))'
  guint64_align = long_long_align
else
  error('Compiler provides no native 64-bit integer type')
endif
tlibconfig_conf.set('tlib_extension', tlib_extension)
tlibconfig_conf.set('gint64', gint64)
tlibconfig_conf.set_quoted('gint64_modifier', gint64_modifier)
tlibconfig_conf.set_quoted('gint64_format', gint64_format)
tlibconfig_conf.set_quoted('guint64_format', guint64_format)
tlibconfig_conf.set('gint64_constant', gint64_constant)
tlibconfig_conf.set('guint64_constant', guint64_constant)
tlib_conf.set('ALIGNOF_GUINT64', guint64_align)

if host_system == 'windows'
  tlibconfig_conf.set('g_pid_type', 'void*')
  tlibconfig_conf.set_quoted('g_pid_format', 'p')
  if host_machine.cpu_family() == 'x86_64'
    tlibconfig_conf.set_quoted('g_pollfd_format', '%#' + int64_m + 'x')
  else
    tlibconfig_conf.set_quoted('g_pollfd_format', '%#x')
  endif
  tlibconfig_conf.set('g_dir_separator', '\\\\')
  tlibconfig_conf.set('g_searchpath_separator', ';')
else
  tlibconfig_conf.set('g_pid_type', 'int')
  tlibconfig_conf.set_quoted('g_pid_format', 'i')
  tlibconfig_conf.set_quoted('g_pollfd_format', '%d')
  tlibconfig_conf.set('g_dir_separator', '/')
  tlibconfig_conf.set('g_searchpath_separator', ':')
endif

g_sizet_compatibility = {
  'short': sizet_size == short_size,
  'int': sizet_size == int_size,
  'long': sizet_size == long_size,
  'long long': sizet_size == long_long_size,
}

# Do separate checks for gcc/clang (and ignore other compilers for now), since
# we need to explicitly pass -Werror to the compilers.
# FIXME: https://github.com/mesonbuild/meson/issues/5399
# We can’t simplify these checks using a foreach loop because dictionary keys
# have to be string literals.
# FIXME: https://github.com/mesonbuild/meson/issues/5231
if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
  g_sizet_compatibility += {
    'short': g_sizet_compatibility['short'] and cc.compiles(
        '''#include <stddef.h>
        size_t f (size_t *i) { return *i + 1; }
        int main (void) {
          unsigned short i = 0;
          f (&i);
          return 0;
        }''',
        args: ['-Werror'],
        name : 'GCC size_t typedef is short'),
    'int': g_sizet_compatibility['int'] and cc.compiles(
        '''#include <stddef.h>
        size_t f (size_t *i) { return *i + 1; }
        int main (void) {
          unsigned int i = 0;
          f (&i);
          return 0;
        }''',
        args: ['-Werror'],
        name : 'GCC size_t typedef is int'),
    'long': g_sizet_compatibility['long'] and cc.compiles(
        '''#include <stddef.h>
        size_t f (size_t *i) { return *i + 1; }
        int main (void) {
          unsigned long i = 0;
          f (&i);
          return 0;
        }''',
        args: ['-Werror'],
        name : 'GCC size_t typedef is long'),
    'long long': g_sizet_compatibility['long long'] and cc.compiles(
        '''#include <stddef.h>
        size_t f (size_t *i) { return *i + 1; }
        int main (void) {
          unsigned long long i = 0;
          f (&i);
          return 0;
        }''',
        args: ['-Werror'],
        name : 'GCC size_t typedef is long long'),
  }
endif

if g_sizet_compatibility['short']
  tlibconfig_conf.set('tlib_size_type_define', 'short')
  tlibconfig_conf.set_quoted('tsize_modifier', 'h')
  tlibconfig_conf.set_quoted('tssize_modifier', 'h')
  tlibconfig_conf.set_quoted('tsize_format', 'hu')
  tlibconfig_conf.set_quoted('tssize_format', 'hi')
  tlibconfig_conf.set('tlib_msize_type', 'SHRT')
elif g_sizet_compatibility['int']
  tlibconfig_conf.set('tlib_size_type_define', 'int')
  tlibconfig_conf.set_quoted('tsize_modifier', '')
  tlibconfig_conf.set_quoted('tssize_modifier', '')
  tlibconfig_conf.set_quoted('tsize_format', 'u')
  tlibconfig_conf.set_quoted('tssize_format', 'i')
  tlibconfig_conf.set('tlib_msize_type', 'INT')
elif g_sizet_compatibility['long']
  tlibconfig_conf.set('tlib_size_type_define', 'long')
  tlibconfig_conf.set_quoted('tsize_modifier', 'l')
  tlibconfig_conf.set_quoted('tssize_modifier', 'l')
  tlibconfig_conf.set_quoted('tsize_format', 'lu')
  tlibconfig_conf.set_quoted('tssize_format', 'li')
  tlibconfig_conf.set('tlib_msize_type', 'LONG')
elif g_sizet_compatibility['long long']
  tlibconfig_conf.set('tlib_size_type_define', 'long long')
  tlibconfig_conf.set_quoted('tsize_modifier', int64_m)
  tlibconfig_conf.set_quoted('tssize_modifier', int64_m)
  tlibconfig_conf.set_quoted('tsize_format', int64_m + 'u')
  tlibconfig_conf.set_quoted('tssize_format', int64_m + 'i')
  tlibconfig_conf.set('tlib_msize_type', 'INT64')
else
  error('Could not determine size of size_t.')
endif

if voidp_size == int_size
  tlibconfig_conf.set('tlib_intptr_type_define', 'int')
  tlibconfig_conf.set_quoted('tintptr_modifier', '')
  tlibconfig_conf.set_quoted('tintptr_format', 'i')
  tlibconfig_conf.set_quoted('tuintptr_format', 'u')
  tlibconfig_conf.set('tlib_gpi_cast', '(gint)')
  tlibconfig_conf.set('tlib_gpui_cast', '(guint)')
elif voidp_size == long_size
  tlibconfig_conf.set('tlib_intptr_type_define', 'long')
  tlibconfig_conf.set_quoted('tintptr_modifier', 'l')
  tlibconfig_conf.set_quoted('tintptr_format', 'li')
  tlibconfig_conf.set_quoted('tuintptr_format', 'lu')
  tlibconfig_conf.set('tlib_gpi_cast', '(glong)')
  tlibconfig_conf.set('tlib_gpui_cast', '(gulong)')
elif voidp_size == long_long_size
  tlibconfig_conf.set('tlib_intptr_type_define', 'long long')
  tlibconfig_conf.set_quoted('tintptr_modifier', int64_m)
  tlibconfig_conf.set_quoted('tintptr_format', int64_m + 'i')
  tlibconfig_conf.set_quoted('tuintptr_format', int64_m + 'u')
  tlibconfig_conf.set('tlib_gpi_cast', '(gint64)')
  tlibconfig_conf.set('tlib_gpui_cast', '(guint64)')
else
  error('Could not determine size of void *')
endif

if long_size != 8 and long_long_size != 8 and int_size != 8
  error('GLib requires a 64-bit type. You might want to consider using the GNU C compiler.')
endif

tlibconfig_conf.set('tintbits', int_size * 8)
tlibconfig_conf.set('tlongbits', long_size * 8)
tlibconfig_conf.set('tsizebits', sizet_size * 8)
tlibconfig_conf.set('tssizebits', ssizet_size * 8)

# XXX: https://gitlab.gnome.org/GNOME/tlib/issues/1413
if host_system == 'windows'
  g_module_suffix = 'dll'
else
  g_module_suffix = 'so'
endif
tlibconfig_conf.set('g_module_suffix', g_module_suffix)

tlibconfig_conf.set('TLIB_MAJOR_VERSION', major_version)
tlibconfig_conf.set('TLIB_MINOR_VERSION', minor_version)
tlibconfig_conf.set('TLIB_MICRO_VERSION', micro_version)
tlibconfig_conf.set('TLIB_VERSION', tlib_version)

tlibconfig_conf.set('tlib_void_p', voidp_size)
tlibconfig_conf.set('tlib_long', long_size)
tlibconfig_conf.set('tlib_size_t', sizet_size)
tlibconfig_conf.set('tlib_ssize_t', ssizet_size)
if host_machine.endian() == 'big'
  tlibconfig_conf.set('t_byte_order', 'G_BIG_ENDIAN')
  tlibconfig_conf.set('t_bs_native', 'BE')
  tlibconfig_conf.set('t_bs_alien', 'LE')
else
  tlibconfig_conf.set('t_byte_order', 'G_LITTLE_ENDIAN')
  tlibconfig_conf.set('t_bs_native', 'LE')
  tlibconfig_conf.set('t_bs_alien', 'BE')
endif

# === va_copy checks ===
# we currently check for all three va_copy possibilities, so we get
# all results in config.log for bug reports.

va_copy_func = ''
foreach try_func : [ '__va_copy', 'va_copy' ]
  if cc.compiles('''#include <stdarg.h>
                    #include <stdlib.h>
                    #ifdef _MSC_VER
                    # include "msvc_recommended_pragmas.h"
                    #endif
                    void f (int i, ...) {
                    va_list args1, args2;
                    va_start (args1, i);
                    @0@ (args2, args1);
                    if (va_arg (args2, int) != 42 || va_arg (args1, int) != 42)
                      exit (1);
                    va_end (args1); va_end (args2);
                    }
                    int main() {
                      f (0, 42);
                      return 0;
                    }'''.format(try_func),
                    name : try_func + ' check')
    va_copy_func = try_func
  endif
endforeach
if va_copy_func != ''
  tlib_conf.set('G_VA_COPY', va_copy_func)
  tlib_vacopy = '#define G_VA_COPY ' + va_copy_func
else
  tlib_vacopy = '/* #undef G_VA_COPY */'
endif

va_list_val_copy_prog = '''
  #include <stdarg.h>
  #include <stdlib.h>
  void f (int i, ...) {
    va_list args1, args2;
    va_start (args1, i);
    args2 = args1;
    if (va_arg (args2, int) != 42 || va_arg (args1, int) != 42)
      exit (1);
    va_end (args1); va_end (args2);
  }
  int main() {
    f (0, 42);
    return 0;
  }'''

if cc_can_run
  rres = cc.run(va_list_val_copy_prog, name : 'va_lists can be copied as values')
  tlib_va_val_copy = rres.returncode() == 0
else
  tlib_va_val_copy = meson.get_cross_property('va_val_copy', true)
endif
if not tlib_va_val_copy
  tlib_vacopy = tlib_vacopy + '\n#define G_VA_COPY_AS_ARRAY 1'
  tlib_conf.set('G_VA_COPY_AS_ARRAY', 1)
endif
tlibconfig_conf.set('tlib_vacopy', tlib_vacopy)

# check for flavours of varargs macros
g_have_iso_c_varargs = cc.compiles('''
  void some_func (void) {
    int a(int p1, int p2, int p3);
    #define call_a(...) a(1,__VA_ARGS__)
    call_a(2,3);
  }''', name : 'ISO C99 varargs macros in C')

if g_have_iso_c_varargs
  tlibconfig_conf.set('g_have_iso_c_varargs', '''
#ifndef __cplusplus
# define G_HAVE_ISO_VARARGS 1
#endif''')
endif

g_have_iso_cxx_varargs = cxx.compiles('''
  void some_func (void) {
    int a(int p1, int p2, int p3);
    #define call_a(...) a(1,__VA_ARGS__)
    call_a(2,3);
  }''', name : 'ISO C99 varargs macros in C++')

if g_have_iso_cxx_varargs
  tlibconfig_conf.set('g_have_iso_cxx_varargs', '''
#ifdef __cplusplus
# define G_HAVE_ISO_VARARGS 1
#endif''')
endif

g_have_gnuc_varargs = cc.compiles('''
  void some_func (void) {
    int a(int p1, int p2, int p3);
    #define call_a(params...) a(1,params)
    call_a(2,3);
  }''', name : 'GNUC varargs macros')

if cc.has_header('alloca.h')
  tlibconfig_conf.set('GLIB_HAVE_ALLOCA_H', true)
endif
has_syspoll = cc.has_header('sys/poll.h')
has_systypes = cc.has_header('sys/types.h')
if has_syspoll
  tlibconfig_conf.set('GLIB_HAVE_SYS_POLL_H', true)
endif
has_winsock2 = cc.has_header('winsock2.h')

if has_syspoll and has_systypes
  poll_includes = '''
      #include<sys/poll.h>
      #include<sys/types.h>'''
elif has_winsock2
  poll_includes = '''
      #define _WIN32_WINNT @0@
      #include <winsock2.h>'''.format(tlib_conf.get('_WIN32_WINNT'))
else
  # FIXME?
  error('FIX POLL* defines')
endif

poll_defines = [
  [ 'POLLIN', 't_pollin', 1 ],
  [ 'POLLOUT', 't_pollout', 4 ],
  [ 'POLLPRI', 't_pollpri', 2 ],
  [ 'POLLERR', 't_pollerr', 8 ],
  [ 'POLLHUP', 't_pollhup', 16 ],
  [ 'POLLNVAL', 't_pollnval', 32 ],
]

if has_syspoll and has_systypes
  foreach d : poll_defines
    val = cc.compute_int(d[0], prefix: poll_includes)
    tlibconfig_conf.set(d[1], val)
  endforeach
elif has_winsock2
  # Due to a missed bug in configure.ac the poll test
  # never succeeded on Windows and used some pre-defined
  # values as a fallback. Keep using them to maintain
  # ABI compatibility with autotools builds of tlibs
  # and with *any* tlib-using code compiled against them,
  # since these values end up in a public header tlibconfig.h.
  foreach d : poll_defines
    tlibconfig_conf.set(d[1], d[2])
  endforeach
endif

# Internet address families
# FIXME: what about Cygwin (G_WITH_CYGWIN)
if host_system == 'windows'
  inet_includes = '''
      #include <winsock2.h>'''
else
  inet_includes = '''
      #include <sys/types.h>
      #include <sys/socket.h>'''
endif

inet_defines = [
  [ 'AF_UNIX', 't_af_unix' ],
  [ 'AF_INET', 't_af_inet' ],
  [ 'AF_INET6', 't_af_inet6' ],
  [ 'MSG_OOB', 't_msg_oob' ],
  [ 'MSG_PEEK', 't_msg_peek' ],
  [ 'MSG_DONTROUTE', 't_msg_dontroute' ],
]
foreach d : inet_defines
  val = cc.compute_int(d[0], prefix: inet_includes)
  tlibconfig_conf.set(d[1], val)
endforeach

if host_system == 'windows'
  have_ipv6 = true
else
  have_ipv6 = cc.has_type('struct in6_addr', prefix: '#include <netinet/in.h>')
endif
tlib_conf.set('HAVE_IPV6', have_ipv6)

# We need to decide at configure time if GLib will use real atomic
# operations ("lock free") or emulated ones with a mutex.  This is
# because we must put this information in tlibconfig.h so we know if
# it is safe or not to inline using compiler intrinsics directly from
# the header.
#
# We also publish the information via G_ATOMIC_LOCK_FREE in case the
# user is interested in knowing if they can use the atomic ops across
# processes.
#
# We can currently support the atomic ops natively when building GLib
# with recent versions of GCC or MSVC.
#
# Note that the atomic ops are only available with GCC on x86 when
# using -march=i486 or higher.  If we detect that the atomic ops are
# not available but would be available given the right flags, we want
# to abort and advise the user to fix their CFLAGS.  It's better to do
# that then to silently fall back on emulated atomic ops just because
# the user had the wrong build environment.
atomictest = '''int main() {
  int atomic = 2;
  __sync_bool_compare_and_swap (&atomic, 2, 3);
  return 0;
}
'''

atomicdefine = '''
#ifndef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
#error "compiler has atomic ops, but doesn't define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4"
#endif
'''

# We know that we can always use real ("lock free") atomic operations with MSVC
if cc.get_id() == 'msvc' or cc.get_id() == 'clang-cl' or cc.links(atomictest, name : 'atomic ops')
  have_atomic_lock_free = true
  if cc.get_id() == 'gcc' and not cc.compiles(atomicdefine, name : 'atomic ops define')
    # Old gcc release may provide
    # __sync_bool_compare_and_swap but doesn't define
    # __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
    tlib_conf.set('__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4', true)
  endif
else
  have_atomic_lock_free = false
  if host_machine.cpu_family() == 'x86' and cc.links(atomictest, args : '-march=i486')
    error('GLib must be built with -march=i486 or later.')
  endif
endif
tlibconfig_conf.set('G_ATOMIC_LOCK_FREE', have_atomic_lock_free)

# Tests for iconv
#
# We should never use the MinGW C library's iconv because it may not be
# available in the actual runtime environment. On Windows, we always use
# the built-in implementation
iconv_opt = get_option('iconv')
if host_system == 'windows'
  libiconv = []
  # We have a #include "win_iconv.c" in gconvert.c on Windows, so we don't need
  # any external library for it
  if iconv_opt != 'auto'
    warning('-Diconv was set to @0@, which was ignored')
  endif
else
  found_iconv = false
  if ['auto', 'libc'].contains(iconv_opt) and cc.has_function('iconv_open')
    libiconv = []
    found_iconv = true
  endif
  if not found_iconv and ['auto', 'external'].contains(iconv_opt) and cc.has_header_symbol('iconv.h', 'iconv_open')
    libiconv = [cc.find_library('iconv')]
    found_iconv = true
  endif

  if not found_iconv
    error('iconv implementation "@0@" not found'.format(iconv_opt))
  endif
endif

test_timeout = 60
test_timeout_slow = 180

pkg = import('pkgconfig')
windows = import('windows')
subdir('tlib')
#subdir('tio')
#subdir('fuzzing')
#if build_tests
#  subdir('tests')
#endif

