fips_begin_app(kiss-ds-test cmdline)
    fips_files(
	    main.cc

        KISS_RING_Tests.c
        KISS_QUEUE_Tests.c
        KISS_BLOCKPOOL_Tests.c
        KISS_ARENA_Tests.c
        KISS_ARRAY_Tests.c

    )

    if (FIPS_LINUX)
        fips_libs(m)
    endif()
    fips_deps(kiss-ds)
fips_end_app()

