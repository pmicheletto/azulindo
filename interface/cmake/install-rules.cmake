install(
    TARGETS interface_exe
    RUNTIME COMPONENT interface_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
