#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <set>
#include "Triple.h"
#include "triple_bwt.hpp"

uint64_t get_size_interval(Triple * triple_pattern, triple_bwt & graph) {
    if (triple_pattern->s->isVariable && !triple_pattern->p->isVariable && triple_pattern->o->isVariable) {
        bwt_interval open_interval = graph.down_P(triple_pattern->p->constant);
        return open_interval.size();
    } 
    return 0;
}

bool compare_by_second(pair<string, int> a, pair<string, int> b) {
    return a.second < b.second;
}

vector<string> get_gao_min_gen(vector<Triple*> query, triple_bwt & graph) {
    map<string, vector<uint64_t>> triple_values;
    map<string, vector<Triple*>> triples_var; 
    for (Triple * triple_pattern : query) {
        uint64_t triple_size = get_size_interval(triple_pattern, graph);
        if (triple_pattern->s->isVariable) {
          triple_values[triple_pattern->s->varname].push_back(triple_size);
          triples_var[triple_pattern->s->varname].push_back(triple_pattern);
        }
        if (triple_pattern->p->isVariable) {
          triple_values[triple_pattern->p->varname].push_back(triple_size);
          triples_var[triple_pattern->p->varname].push_back(triple_pattern);
        }
        if (triple_pattern->o->isVariable) {
          triple_values[triple_pattern->o->varname].push_back(triple_size);
          triples_var[triple_pattern->o->varname].push_back(triple_pattern);
        }
    }

    vector<string> gao;
    vector<pair<string, uint64_t>> varmin_pairs;
    vector<string> single_vars;
    map<string, bool> selectable_vars;
    map<string, bool> selected_vars;
    map<string, set<string>> related_vars;

    for(auto it = (triples_var).cbegin(); it != (triples_var).cend(); ++it) {
        for (Triple * triple_pattern : triples_var[it->first]) {
            if (triple_pattern->s->isVariable && it->first.compare(triple_pattern->s->varname) != 0) {
                related_vars[it->first].insert(triple_pattern->s->varname);
            }
            if (triple_pattern->p->isVariable && it->first.compare(triple_pattern->p->varname) != 0) {
                related_vars[it->first].insert(triple_pattern->p->varname);
            }
            if (triple_pattern->o->isVariable && it->first.compare(triple_pattern->o->varname) != 0) {
                related_vars[it->first].insert(triple_pattern->o->varname);
            }
        }    
    }


    for(auto it = (triple_values).cbegin(); it != (triple_values).cend(); ++it) {
        if (triple_values[it->first].size() == 1) {
            single_vars.push_back(it->first);
        } else {
            varmin_pairs.push_back(pair<string, uint64_t>(it->first, *min_element(triple_values[it->first].begin(), triple_values[it->first].end())));
            selectable_vars[it->first] = false;
        }
    }
    

    sort((varmin_pairs).begin(), (varmin_pairs).end(), compare_by_second);
    if (varmin_pairs.size() > 0) {
        selectable_vars[varmin_pairs[0].first] = true;
    }
    for (int i = 0; i < varmin_pairs.size(); i++) {
        for (pair<string, uint64_t> varmin_pair : varmin_pairs) {
            if (selectable_vars[varmin_pair.first] && !selected_vars[varmin_pair.first]) {
                gao.push_back(varmin_pair.first);
                selected_vars[varmin_pair.first] = true;
                for (set<string>::iterator it=related_vars[varmin_pair.first].begin(); it!=related_vars[varmin_pair.first].end(); ++it) {
                    selectable_vars[*it] = true;
                }
                break;
            }
        }
    }

    for (pair<string, uint64_t> varmin_pair : varmin_pairs) {
        if (!selected_vars[varmin_pair.first]) {
            gao.push_back(varmin_pair.first);
        }
    }

    for (string s : single_vars) {
        gao.push_back(s);
    }

    /*std::cout << "******" <<  gao.size() << "******" << std::endl;
    for (uint64_t i = 0; i < gao.size(); i++)
        std::cout << gao[i] << std::endl;
    */
    return gao;

}



#endif 
