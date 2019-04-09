include_directories(/root/opensource/hiredis)
link_directories(/root/opensource/hiredis)
link_libraries(hiredis)

add_executable(hello
	main.c
)

target_link_libraries(hello
	${LWAN_COMMON_LIBS}
	${ADDITIONAL_LIBRARIES}
)
