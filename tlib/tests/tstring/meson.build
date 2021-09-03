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
        'tstring_startswith' : {},
        'tstring_startswith_v2' : {},
        'tstring_startswith_v3' : {},
        'tstring_startswith_v4' : {},
        'tstring_endswith' : {},
        'tstring_endswith_v2' : {},
        'tstring_hashcode' : {},
        'tstring_indexof' : {},
        'tstring_indexof_v2' : {},
        'tstring_indexof_v3' : {},
        'tstring_indexof_v4' : {},
        'tstring_indexof_v5' : {},
        'tstring_indexof_v6' : {},
        'tstring_last_indexof' : {},
        'tstring_last_indexof_v2' : {},
        'tstring_last_indexof_v3' : {},
        'tstring_last_indexof_v4' : {},
        'tstring_last_indexof_v5' : {},
        'tstring_last_indexof_v6' : {},
        'tstring_substring' : {},
        'tstring_substring_v2' : {},
        'tstring_concat' : {},
        'tstring_concat_v2' : {},
        'tstring_replace' : {},
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
