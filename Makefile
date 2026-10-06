CXX      ?= c++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
CXXFLAGS += -Iheaders
HEADERS  = headers/imagen.h headers/filtros.h headers/argumentos.h

# OpenMP: en macOS (clang de Apple) se usa libomp de Homebrew.
ifeq ($(shell uname),Darwin)
  LIBOMP  ?= $(shell brew --prefix libomp)
  OMPFLAGS = -Xpreprocessor -fopenmp -I$(LIBOMP)/include -L$(LIBOMP)/lib -lomp
else
  OMPFLAGS = -fopenmp
endif

all: processor filterer th_filterer omp_filterer

processor filterer: %: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $<

th_filterer: src/th_filterer.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<

omp_filterer: src/omp_filterer.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OMPFLAGS)

mpi_filterer: src/mpi_filterer.cpp $(HEADERS)
	mpicxx $(CXXFLAGS) -DOMPI_SKIP_MPICXX -o $@ $<

# Las versiones paralelas deben producir exactamente la misma imagen que la secuencial.
check: all
	@mkdir -p images/out/check
	@for img in feep.pgm lena.ppm fruit.pgm; do \
	  for f in blur gauss sharpen laplace sobel; do \
	    ./filterer    images/$$img images/out/check/seq_$${f}_$$img --f $$f >/dev/null && \
	    ./th_filterer  images/$$img images/out/check/th_$$img  --f $$f >/dev/null && \
	    ./omp_filterer images/$$img images/out/check/omp_$$img --f $$f >/dev/null && \
	    cmp -s images/out/check/seq_$${f}_$$img images/out/check/th_$$img && \
	    cmp -s images/out/check/seq_$${f}_$$img images/out/check/omp_$$img && \
	    echo "OK   $$img $$f" || { echo "FALLA $$img $$f"; exit 1; }; \
	  done; \
	done
	@./processor images/lena.ppm images/out/check/copy.ppm >/dev/null && \
	  ./processor - images/out/check/copy2.ppm < images/out/check/copy.ppm >/dev/null && \
	  cmp -s images/out/check/copy.ppm images/out/check/copy2.ppm && echo "OK   processor (archivo y stdin)"

clean:
	rm -f processor filterer th_filterer omp_filterer mpi_filterer
	rm -rf images/out

.PHONY: all check clean
