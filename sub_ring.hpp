/*
 * sub_ring.hpp
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

#ifndef SUB_RING
#define SUB_RING

#include <cstdint>
//#include "bwt.hpp"
//#include "bwt_interval.hpp"
#include "Config.hpp"

//#include <stdio.h>
//#include <stdlib.h>
#include <fstream>

class sub_ring
{ 
    table_column_type F_s;
    uint64_t n_p;  // number of tuples

    sdsl::sd_vector<> B_s;
    sdsl::rank_support_sd<> rank_B_s;
    sdsl::select_support_sd<> select_B_s;
    /**/
    sdsl::sd_vector<> A_s;
    sdsl::rank_support_sd<> rank_A_s;
    sdsl::select_support_sd<> select_A_s;
    /**/
    sdsl::sd_vector<> B_o;
    sdsl::rank_support_sd<> rank_B_o;
    sdsl::select_support_sd<> select_B_o;
    /**/
    sdsl::sd_vector<> A_o;
    sdsl::rank_support_sd<> rank_A_o;
    sdsl::select_support_sd<> select_A_o;

/*    uint64_t max_S;
    uint64_t max_P;
    uint64_t max_O;
    uint64_t sigma_S;
    uint64_t sigma_P;
    uint64_t sigma_O;    
*/
    public:
        sub_ring() {;};

        ~sub_ring() {;};

        sub_ring(const sub_ring & sr)
	{
	    F_s = sr.F_s;
	    n_p = sr.n_p;
	    B_s = sr.B_s;
	    util::init_support(rank_B_s, &B_s);
            util::init_support(select_B_s, &B_s);
	    A_s = sr.A_s;
            util::init_support(rank_A_s, &A_s);
	    util::init_support(select_A_s, &A_s);
	    B_o = sr.B_o;
	    util::init_support(rank_B_o, &B_o);
            util::init_support(select_B_o, &B_o);
	    A_o = sr.A_o;
	    util::init_support(rank_A_o, &A_o);
	    util::init_support(select_A_o, &A_o);
	}

        // Builds a subring from a vector of pairs
        sub_ring(std::vector<std::pair<uint64_t,uint64_t>>& D, uint64_t U)
        {
            std::vector<uint64_t> C_s, C_o;
            std::vector<uint64_t> _A_s(U+2, 0), _A_o(U+2, 0);
            
            sdsl::int_vector<> _F_s(D.size()+2);

            cout <<  D.size() << "  "; fflush(stdout); 
            n_p = D.size();
            // First sort using the first component of the pairs
            sort(D.begin(), D.end(), [](const auto& a, const auto& b) {return a.first < b.first; });
            // Now build C_o.
            C_o.push_back(0); //dummy value
            for (uint64_t i = 0; i < D.size(); i++)
                C_o.push_back(D[i].second);
             
            // Then, stable sort using the second component of the pairs
            stable_sort(D.begin(), D.end(), [](const auto& a, const auto& b) {return a.second < b.second; }); 
            // Now build C_s
            C_s.push_back(0); //dummy value
            for (uint64_t i = 0; i < D.size(); i++)
                C_s.push_back(D[i].first);

            // Now, compute arrays A_s and A_o
            sdsl::bit_vector b_s(U+2, 0), b_o(U+2, 0);
            for (uint64_t i = 0; i < D.size(); ++i) {
                _A_s[D[i].first]++;
                b_s[D[i].first] = 1;
                _A_o[D[i].second]++;
                b_o[D[i].second] = 1;
            }
            b_s[U+1] = 1;
            b_o[U+1] = 1;
        
            uint64_t t = 0, t_p;
//            cout << "A_s" << endl;
            for (uint64_t i = 1; i < U+1; ++i) {
	        t_p = _A_s[i];
		_A_s[i] = t;
//                cout << _A_s[i] << endl;
		t += t_p; 
	    } 
            
            t = 0;
            for (uint64_t i = 1; i < U+1; ++i) {
                t_p = _A_o[i];
                _A_o[i] = t;
                t += t_p;
            }  

            // Luego, calcular F_s usando LF
            std::vector<uint64_t> v_aux(U+2, 0);
            _F_s[0] = 0; // dummy value
            //cout << "F_s" << endl;
            for (uint64_t i = 1; i < C_s.size(); i++) {
                v_aux[C_s[i]]++;
                _F_s[i] = _A_s[C_s[i]] + v_aux[C_s[i]];
                //cout << " > F_s[" << i << "] = " << _F_s[i] << endl;
            }
            //cout << "*******" << endl;

            // Luego construir la WM para F_s
            util::bit_compress(_F_s);
            construct_im(F_s, _F_s);  

//            cout << "U="<<U <<endl;
//            cout << "n="<<D.size() << endl;
            {
                sdsl::bit_vector a(D.size()+1, 0);

                for (uint64_t i = 0; i < _A_s.size(); i++) {
//                cout << i << " " << i+_A_s[i] << endl;
                    if (b_s[i])
	               a[_A_s[i]] = 1;
                }
	        a[D.size()] = 1;
/*
                cout << "As" << endl;
                for (uint64_t i = 0; i < a.size(); i++) {
                    cout << a[i];
                }
                cout << endl; 
                cout << "Bs" << endl;
                for (uint64_t i = 0; i < b_s.size(); i++) {
                    cout << b_s[i];
                }
                cout << endl;
*/
                A_s = sd_vector<>(a);
                B_s = sd_vector<>(b_s); 
                util::init_support(select_A_s, &A_s);
                util::init_support(rank_A_s, &A_s);
                util::init_support(select_B_s, &B_s);
                util::init_support(rank_B_s, &B_s);
            }

            {
                sdsl::bit_vector a(D.size()+1, 0);

                for (uint64_t i = 0; i < _A_o.size(); i++) {
//                cout << i << " " << i+_A_s[i] << endl;
                    if (b_o[i])
                       a[_A_o[i]] = 1;
                }
                a[D.size()] = 1;
/*
                cout << "Ao" << endl;
                for (uint64_t i = 0; i < a.size(); i++) {
                    cout << a[i];
                }
                cout << endl;
                cout << "Bo" << endl;
                for (uint64_t i = 0; i < b_o.size(); i++) {
                    cout << b_o[i];
                }
                cout << endl;
*/
                A_o = sd_vector<>(a);
                B_o = sd_vector<>(b_o);
                util::init_support(rank_A_o, &A_o);
                util::init_support(select_A_o, &A_o);
                util::init_support(rank_B_o, &B_o);
                util::init_support(select_B_o, &B_o);
            }

            cout << "-- Subring constructed successfully" << endl; fflush(stdout);
        };

        uint64_t size_in_bytes()
        {
            return sdsl::size_in_bytes(F_s) + sdsl::size_in_bytes(A_s) + sdsl::size_in_bytes(B_s)
                 + sdsl::size_in_bytes(A_o) + sdsl::size_in_bytes(B_o);
        };

        void save(std::string filename)
        {
            std::ofstream ofs(filename+".nP");
            ofs << n_p << std::endl;
            ofs.close();
            sdsl::store_to_file(F_s, filename+".Fs");
            sdsl::store_to_file(B_s, filename+".Bs"); 
            sdsl::store_to_file(A_s, filename+".As");
            sdsl::store_to_file(B_o, filename+".Bo");
            sdsl::store_to_file(A_o, filename+".Ao");
        };

        sub_ring load(std::string filename)
        {
            std::ifstream ifs(filename+".nP");
            ifs >> n_p;
            ifs.close();
            sdsl::load_from_file(F_s, filename+".Fs");
            sdsl::load_from_file(B_s, filename+".Bs");
            sdsl::load_from_file(A_s, filename+".As");
            sdsl::load_from_file(B_o, filename+".Bo");
            sdsl::load_from_file(A_o, filename+".Ao");

            util::init_support(rank_B_s, &B_s);
            util::init_support(select_B_s, &B_s);
            util::init_support(rank_A_s, &A_s);
            util::init_support(select_A_s, &A_s);
            util::init_support(rank_B_o, &B_o);
            util::init_support(select_B_o, &B_o);
            util::init_support(rank_A_o, &A_o);
            util::init_support(select_A_o, &A_o);
            return *this;
        };

        inline uint64_t get_As(uint64_t i) 
        {
            //cout << " get_As >> rank_B_s = " << i+1 << endl; fflush(stdout);
            //cout << rank_B_s(i+1) << endl;
            return select_A_s(rank_B_s(i+1) + !B_s[i]);
        }

        inline uint64_t get_Ao(uint64_t i)
        {
            return select_A_o(rank_B_o(i+1) + !B_o[i]);
        }

        inline uint64_t rank_Co(uint64_t i, uint64_t c)
        {
            return F_s.range_search_2d(1, i, get_As(c)+1, get_As(c+1),false).first;
        }

        inline uint64_t rank_Cs(uint64_t i, uint64_t c)
        {
            return F_s.range_search_2d(get_Ao(c)+1, get_Ao(c+1), 1, i, false).first;
        }

        inline uint64_t range_next_value_Co(uint64_t s, uint64_t e, uint64_t c)
	{
            //cout << " > F_s.rel_min_obj_maj(" << s << "," << e << "," << get_Ao(c)+1 << ")" << endl;   
            //char d;
            //cin >> d; 
            uint64_t t = F_s.rel_min_obj_maj(s, e, get_Ao(c)+1);
            //cout << "get_Ao(" << c << ")=" << get_Ao(c) << endl;
            //cout << " > range_next_value_Co t = " << t << " n_p=" << n_p << endl;
            //cout << " > F_s.rel_min_obj_maj(" << s << "," << e << "," << get_Ao(c)+1 << ") = " << t << endl; 

            if (t > n_p) return 0;
            //cout << "El rank final hasta " << t << " en Co da = " << rank_A_o(t) << endl;
            return select_B_o(rank_A_o(t));
	}

        inline uint64_t range_next_value_Cs(uint64_t s, uint64_t e, uint64_t c)
        {
            //uint64_t t = F_s.range_next_value(get_Ao(c)+1, s, e);
            uint64_t t = F_s.range_next_value(get_As(c)+1, s, e);
            if (!t) return 0;
            //cout << "t=" << t << endl;
            //cout << "El rank final hasta " << t << " en Cs da = " << rank_A_s(t) << endl; 
            return select_B_s(rank_A_s(t));
        }

        inline uint64_t nTriples() const
        {
            return n_p;
        }

 };
#endif
