CXX     := g++-4.7
TARGET  := janosh
SRCS    := janosh.cpp logger.cpp record.cpp path.cpp value.cpp exception.cpp cache.cpp json_spirit/json_spirit_reader.cpp  json_spirit/json_spirit_value.cpp  json_spirit/json_spirit_writer.cpp tcp_server.cpp tcp_client.cpp janosh_thread.cpp commands.cpp trigger_base.cpp settings.cpp request.cpp tracker.cpp backward.cpp component.cpp json.cpp bash.cpp raw.cpp util.cpp exithandler.cpp cache_thread.cpp database_thread.cpp flusher_thread.cpp trigger_thread.cpp tcp_worker.cpp
#precompiled headers
HEADERS := backward.h easylogging++.h json_spirit/json_spirit.h
GCH    := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 
    
CXXFLAGS += -DETLOG -std=c++0x -pedantic -Wall -I./backtrace/ -I/opt/local/include -D_ELPP_THREAD_SAFE  -D_ELPP_DISABLE_LOGGING_FLAGS_FROM_ARG -D_ELPP_DISABLE_DEFAULT_CRASH_HANDLING
LDFLAGS += -L/opt/local/lib 
LIBS    += -lboost_program_options-mt -lboost_serialization-mt -lboost_system-mt -lboost_filesystem-mt -lpthread -lboost_thread-mt -lkyotocabinet  -ldl

.PHONY: all release static clean distclean 

ifeq ($(UNAME), Darwin)
 CXXFLAGS +=  -stdlib=libc++
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -O3 -D_ELPP_DISABLE_DEBUG_LOGS
release: ${TARGET}

reduce: CXXFLAGS = -DETLOG -std=c++0x -pedantic -Wall -I./backtrace/ -g0 -Os -fvisibility=hidden -fvisibility-inlines-hidden
reduce: ${TARGET}

static: LDFLAGS += -s
static: CXXFLAGS += -g0 -O3
static: LIBS = -Wl,-Bstatic -lboost_serialization -lboost_program_options -lboost_system -lboost_filesystem -lkyotocabinet  -llzma -llzo2 -Wl,-Bdynamic -lz -lpthread -lrt -ldl
static: ${TARGET}

screeninvader: LDFLAGS += -s
screeninvader: CXXFLAGS += -D_JANOSH_DEBUG -mfloat-abi=hard -mfpu=neon -g0 -O3 
screeninvader: LIBS = -Wl,-Bstatic -lboost_serialization -lboost_program_options -lboost_system -lboost_filesystem -lkyotocabinet  -llzma -llzo2 -Wl,-Bdynamic -lz -lpthread -lrt -ldl
screeninvader: ${TARGET}

screeninvader_debug: LDFLAGS += -Wl,--export-dynamic
screeninvader_debug: CXXFLAGS += -D_JANOSH_DEBUG -mfloat-abi=hard -mfpu=neon -g3 -O0 -rdynamic -D_JANOSH_DEBUG
screeninvader_debug: LIBS = -Wl,-Bstatic -lboost_serialization -lboost_program_options -lboost_system -lboost_filesystem -lkyotocabinet  -llzma -llzo2 -Wl,-Bdynamic -lz -lpthread -lrt -ldl -lbfd
screeninvader_debug: ${TARGET}

debug: CXXFLAGS += -g3 -O0 -rdynamic -D_JANOSH_DEBUG
debug: LDFLAGS += -Wl,--export-dynamic
debug: LIBS+= -lbfd
debug: ${TARGET}

${TARGET}: ${OBJS} 
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

${GCH}: %.gch: ${HEADERS} 
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.h}

install:
	mkdir -p ${DESTDIR}/${PREFIX}
	cp ${TARGET} ${DESTDIR}/${PREFIX}

uninstall:
	rm ${DESTDIR}/${PREFIX}/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${GCH} ${TARGET} 

distclean: clean



