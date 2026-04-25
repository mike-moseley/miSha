  #!/bin/bash
  cmake --build cmake --target tests 2>&1 | sed -E -e 's|\[100%\]|\x1b[32m[100%]\x1b[0m|' -e 's|(\[ *[0-9]{1,2}%\])|\x1b[33m\1\x1b[0m|' -e 's|Building C object (.*)|\x1b[34mBuilding C object \1\x1b[0m|' -e 's|Built target (.*)|Built target \x1b[34m\1\x1b[0m|'

  failed=0
  for test in bin/test/*
  do
      ./$test 2>&1 | sed -e 's/\(.*:PASS\)/\x1b[32m\1\x1b[0m/' -e 's/\(.*:FAIL\)/\x1b[31m\1\x1b[0m/' | awk '{print} /^OK$/{print ""}'
      [ ${PIPESTATUS[0]} -ne 0 ] && failed=1
  done
  exit $failed
