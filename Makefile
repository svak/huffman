CXXFLAGS+= -std=c++14 -Wall

SOURCES=Application.cpp \
		BitsAdapter.cpp \
		Filesystem.cpp \
		HuffmanTree.cpp \
		Processor.cpp \
		SymbolsLookup.cpp

TEST_SOURCES=${SOURCES} \
			 BitsAdapterTests.cpp \
			 DecoderTests.cpp \
			 EncoderTests.cpp \
			 HuffmanTreeTests.cpp \
			 SymbolsLookupTests.cpp

all: encode decode

encode: Encode.cpp ${SOURCES}
	${CXX} ${CXXFLAGS} -O3 $^ -o $@ -lpthread

decode: Decode.cpp ${SOURCES}
	${CXX} ${CXXFLAGS} -O3 $^ -o $@ -lpthread

test: tests
	./tests

tests: ${TEST_SOURCES}
	${CXX} ${CXXFLAGS} -g $^ -o tests -lpthread -lgtest -lgmock -lgtest_main

clean:
	rm -f decode
	rm -f encode
	rm -f tests

