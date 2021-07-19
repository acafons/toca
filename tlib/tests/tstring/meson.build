tlib_tests = {
        'tstring_new' : {},
        'tstring_new_v2' : {},
        'tstring_new_v3' : {},
        'tstring_new_v4' : {},
        'tstring_free' : {},
        'tstring_clone' : {},
        'tstring_length' : {},
        'tstring_empty' : {},
        'tstring_at' : {},
        'tstring_getchars' : {},
        'tstring_istypeof_string' : {},
        'tstring_equals' : {},
        'tstring_equals_ignorecase' : {},
        'tstring_compare' : {},
        'tstring_compare_v2' : {},
        'tstring_compare_ignorecase' : {},
        'tstring_compare_ignorecase_v2' : {},
}

foreach test_name, extra_args : tlib_tests
        source = extra_args.get('source', test_name + '.c')
        install = install_tests_enabled and extra_args.get('install', true)

        if install
                test_conf = configuration_data()
                test_conf.set('install_tests_dir', install_tests_execdir)
                test_conf.set('program', test_name)
                test_conf.set('env', '')
                configure_file(
                        input: installed_tests_template_tap,
                        output: test_name + '.test',
                        install_dir: installed_tests_metadir,
                        configuration: test_conf
                )
        endif

        exe = executable(test_name, source,
                c_args : extra_args.get('c_args', []),
                link_args : extra_args.get('link_args', []),
                dependencies : test_deps + extra_args.get('dependencies', []),
                install_dir: installed_tests_execdir,
                install: install,
        )

        suite = ['tlib'] + extra_args.get('suite', [])
        timeout = suite.contains('slow') ? test_timeout_slow : test_timeout
        test(test_name, exe, env : test_env, timeout : timeout, suite : suite)
endforeach
