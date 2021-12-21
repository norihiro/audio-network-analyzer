set(audio-network-analyzer_SOURCE
	main.c
	source.c
	capture.c
	context.c
)

set(audio-network-analyzer_HEADER
	source.h
	capture.h
	context.h
	context-internal.h
)

add_executable(audio-network-analyzer
	${audio-network-analyzer_SOURCE}
	${audio-network-analyzer_HEADER}
)

target_link_libraries(audio-network-analyzer
	m
	${PULSEAUDIO_LIBRARY}
)

install(TARGETS audio-network-analyzer)
