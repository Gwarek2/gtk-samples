project(
    'gtk4-graph',
    'c',
    default_options : ['c_std=c99']
)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

deps = [
    dependency('gtk4'),
    m_dep
]

sources = [
    'main.c'
]

executable('gtk4-graph', sources, dependencies: deps)
