{
  "targets": [
    {
      "target_name": "dwg2dxf",
      "sources": [
        "src/dwg2dxf.cc",
        "src/suffix.inc",
        "<!@(node -p \"require('fs').readdirSync('./libs/libredwg/src').map(f=>'libs/libredwg/src/'+f).join(' ')\")",
        "<!@(node -p \"require('fs').readdirSync('./libs/libredwg/programs').filter(f => f !== 'suffix.inc').map(f=>'libs/libredwg/programs/'+f).join(' ')\")",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "libs/libredwg/src",
        "libs/libredwg/programs",
        "libs/libredwg/include",
        "headers"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": [
        "NAPI_CPP_EXCEPTIONS",
        "NAPI_VERSION=3",
      ],
      "conditions": [
        [
          'OS=="mac"',
          {
            "link_settings": {
              "libraries": [
                "-Wl,-rpath, ."
              ]
            },
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "CLANG_ENABLE_OBJC_ARC": "YES",
              "OTHER_CFLAGS": [
                "-ObjC++",
                "-std=c++17",
              ]
            },
          }
        ],
        [
          'OS=="win"',
          {
            "sources": [
            ],
            "include_dirs": [
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": ["/std:c++17"]
              }
            },
          }
        ],
      ]
    }
  ]
}
