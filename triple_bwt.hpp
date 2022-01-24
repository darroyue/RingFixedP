/*
 * triple_bwt.hpp
 * Copyright (C) 2020 Author name removed for double blind evaluation
 * 
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RING
#define RING

#include <cstdint>
//#include "bwt.hpp"
#include "bwt_interval.hpp"

#include <stdio.h>
#include <stdlib.h>

#include "sub_ring.hpp"


class triple_bwt
 {
    std::vector<sub_ring> P;

    uint64_t nT;  // number of triples

    public:
        triple_bwt() {;};
  
        ~triple_bwt() {;};

        // Assumes the triples have been stored in a vector<spo_triple>
        triple_bwt(vector<spo_triple> & D)
        {
            // Number of triples
            nT = D.size();

            uint64_t max_P = std::get<1>(D[0]), U = std::get<0>(D[0]);
            if (std::get<2>(D[0]) > U)
                U = std::get<2>(D[0]); 

            for (uint64_t i = 1; i < D.size(); i++) {
	        if (std::get<1>(D[i]) > max_P)
		    max_P = std::get<1>(D[i]);

                if (std::get<0>(D[i]) > U)
                    U = std::get<0>(D[i]);

                if (std::get<2>(D[i]) > U)
                    U = std::get<2>(D[i]);
            }

            cout << "Universe size " << U << endl;
            cout << "Predicates " << max_P << endl;
 
            std::vector<std::vector<std::pair<uint64_t, uint64_t>>> P_aux(max_P+1); // a vector of vectors of pairs
            
	    for (uint64_t i = 0; i < D.size(); i++)
	        P_aux[std::get<1>(D[i])].push_back(std::pair<uint64_t, uint64_t>(std::get<0>(D[i]), std::get<2>(D[i])));

            D.clear();

            // assumes enumeration of P starts from 1
	    P.push_back(sub_ring()); // dummy subring at position 0
            for (uint64_t i = 1; i < P_aux.size(); i++) {
                cout << i << "   ";
	        P.push_back(sub_ring(P_aux[i], U));
                //P.push_back(NULL);
                //P[P.size()-1] = new sub_ring(P_aux[i], U);
                P_aux[i].clear();
                P_aux[i].shrink_to_fit();
	    }


        };

        uint64_t size_in_bytes() 
        {
            uint64_t s = 0;
            for (uint64_t i = 0; i < P.size(); i++)
                s+= P[i].size_in_bytes();
            return s;
        };

        uint64_t nTriples() 
        {
	    return nT;
	}

        uint64_t nP() 
        {
            return P.size();
        }

        void save(string filename)
        {
            std::ofstream ofs_nP(filename+".nP");
	    std::ofstream ofs_P(filename+".P");

            ofs_nP << P.size()-1 << std::endl;
	    ofs_nP << nT << endl;
            
	    for (uint64_t i = 1; i < P.size(); i++)
	        P[i].save(filename+".P-"+std::to_string(i));

        };

        void load(string filename)
        {
            std::ifstream ifs_nP(filename+".nP");
	    std::ifstream ifs_P(filename+".P");

            uint64_t P_size;
	    ifs_nP >> P_size;
	    ifs_nP >> nT;

            P.push_back(sub_ring());                    
            for (uint64_t i = 1; i <= P_size; i++) {
                //std::cout << "Loading sub ring " << i << std::endl;
	        P.push_back(sub_ring().load(filename+".P-"+std::to_string(i)));
            }
        };

        /**********************************/
        // Functions for PSO
        //

        // OJO: esta funcion deja de tener sentido, porque se necesita p_value para saber nTriples. Ver si es necesario mantenerla.
        bwt_interval open_PSO()
        {
            return bwt_interval(1, nT);
        };

        // down_P is "shared" by orders PSO and POS
        bwt_interval down_P(uint64_t p_value)
        {
            std::pair<uint64_t, uint64_t> i = std::pair<uint64_t, uint64_t>(1, P[p_value].nTriples());
            return bwt_interval(i.first, i.second, p_value); //bwt intervals have a p_value associated
        };

        /**********************************/
        // P->S  (simulates going down in the trie)
        // Returns an interval within C_o
        bwt_interval down_P_S(bwt_interval & p_int, uint64_t s)
        {
            uint64_t p_value = p_int.get_p_value();
            return bwt_interval(P[p_value].get_As(s)+1, P[p_value].get_As(s+1), p_value);
        };

        uint64_t min_S_in_P(bwt_interval & I)
        {
            if (I.left() > I.right()) 
                return 0;
            else
                return P[I.get_p_value()].range_next_value_Cs(I.left(), I.right(), 1);
        };

        uint64_t next_S_in_P(bwt_interval & I, uint64_t S)
        {
            //if (S > max_S) return 0;
            if (I.left() > I.right())
                return 0;
            else
                return P[I.get_p_value()].range_next_value_Cs(I.left(), I.right(), S);
        };

/*        uint64_t min_O_in_S(bwt_interval & I)
        {
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), 1);
        }

        uint64_t next_O_in_S(bwt_interval & I, uint64_t O)
        {
            //if (O > max_O) return 0;
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), O);
        }
*/
        /*bool there_are_O_in_S(bwt_interval & I)
        {
            return I.get_cur_value() != I.end();    // OJO, revisar esto
        }*/

        uint64_t min_O_in_PS(bwt_interval & I)
        {
            if (I.left() > I.right())
                return 0;
            //cout << "min_O_in_PS Co[" << I.left() << "," << I.right() << "]=" << P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), 1) << endl;
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), 1);
        };

        uint64_t next_O_in_PS(bwt_interval & I, uint64_t O)
        {
            if (I.left() > I.right())
                return 0;
            //if (O > max_O) return 0;
            //cout << "next_O_in_PS: O=" << O << " Co[" << I.left() << "," << I.right() << "]=" << P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), O) << endl;
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), O);
        };

        /*bool there_are_O_in_PS(bwt_interval & I)
        {
            return I.get_cur_value() != I.end();   // OJO, revisar esta condicion
        }*/

        //std::vector<std::pair<uint64_t, uint64_t>>
        std::vector<uint64_t>
        all_O_in_range(bwt_interval & I)
        {
            std::vector<uint64_t> v;
            uint64_t p_value = I.get_p_value();
            uint64_t cur_o = P[p_value].range_next_value_Co(I.left(), I.right(), 1);
            while (cur_o != 0) {  
	        v.push_back(cur_o);
		cur_o =  P[p_value].range_next_value_Co(I.left(), I.right(), cur_o + 1);
	    }
            return v;
        };

        /**********************************/
        // Functions for POS
        //

        bwt_interval open_POS()
        {
            return bwt_interval(1, nT);
        };

        // P->O  (simulates going down in the trie, for the order POS)
        // Returns an interval within C_s containing the s values that are children of p_value->o_value in the trie
        bwt_interval down_P_O(bwt_interval & p_int, uint64_t p_value, uint64_t o_value)
        {
            return bwt_interval(P[p_value].get_Ao(o_value)+1, P[p_value].get_Ao(o_value+1), p_value); 
        };

        uint64_t min_O_in_P(bwt_interval & I)
        {
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), 1);
        };

        uint64_t next_O_in_P(bwt_interval & I, uint64_t O)
        {
            //if (O > max_O) return 0;
            return P[I.get_p_value()].range_next_value_Co(I.left(), I.right(), O);
        };

        uint64_t min_S_in_PO(bwt_interval & I)
        {
            return P[I.get_p_value()].range_next_value_Cs(I.left(), I.right(), 1);
        };

        uint64_t next_S_in_PO(bwt_interval & I, uint64_t S)
        {
            //if (S > max_S) return 0;
            return P[I.get_p_value()].range_next_value_Cs(I.left(), I.right(), S);
        };

        std::vector<uint64_t>
        all_S_in_range(bwt_interval & I)
        {
            std::vector<uint64_t> v;
            uint64_t p_value = I.get_p_value();
            uint64_t cur_s = P[p_value].range_next_value_Cs(I.left(), I.right(), 1);
            while (cur_s != 0) {  // OJO, cuidado que retorne 0 para terminar
                v.push_back(cur_s);
                cur_s =  P[p_value].range_next_value_Cs(I.left(), I.right(), cur_s + 1);
            }
            return v;
        };


 };
#endif
