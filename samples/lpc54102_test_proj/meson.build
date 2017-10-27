if meson.is_cross_build() and host_machine.cpu() == 'cortex-m4'
    sample_src = files(['./work_proj/src/work_proj.c'])
    elf = executable('lpc54102.elf',
        sample_src,
        include_directories : [Pico_inc],
        install : false)
endif
