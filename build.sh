g++ -O9 -msse4.2 -std=c++14 -DNODEBUG -I ~/include -L ~/lib build-index.cpp -o build-index -lsdsl -ldivsufsort -ldivsufsort64
g++ -O3 -msse4.2 -std=c++14 -DNODEBUG -I ~/include -L ~/lib query-index.cc Triple.cpp Term.cpp -o query-index -lsdsl -ldivsufsort -ldivsufsort64
#g++ -O9 -std=c++14 -DNODEBUG -I ~/include -L ~/lib test-subring.cpp -o test-subring -lsdsl -ldivsufsort -ldivsufsort64
