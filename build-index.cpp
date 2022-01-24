#include <iostream>
#include "triple_bwt.hpp"
#include <fstream>
#include <sdsl/construct.hpp>

using namespace std;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


int main(int argc, char **argv)
{

    vector<spo_triple> D, E;

    std::ifstream ifs(argv[1]);
    uint64_t s, p , o;
    //set<spo_triple> S;
    do {
        ifs >> s >> p >> o;
        //S.insert(spo_triple(s, p, o));
        D.push_back(spo_triple(s, p, o));
    } while (!ifs.eof());

    D.pop_back();

    //for (set<spo_triple>::iterator it=S.begin(); it!=S.end(); it++) 
    //    D.push_back(*it);
  
    //S.clear();
    
    D.shrink_to_fit();
    cout << "--Indexing " << D.size() << " triples" << endl;
    memory_monitor::start();
    auto start = timer::now();
    
    triple_bwt A(D);
    auto stop = timer::now();
    memory_monitor::stop();
    cout << "  Index built " << (float)A.size_in_bytes()/A.nTriples() << " bytes per triple" << endl;
     
    A.save(string(argv[1]));
    cout << "Index saved" << endl;
    cout << duration_cast<seconds>(stop-start).count() << " seconds." << endl;
    cout << "Peak of memory usage: " << memory_monitor::peak() << " bytes." << endl;
    return 0;
}

