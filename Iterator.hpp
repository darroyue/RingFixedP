#ifndef LEAPFROG_ITERATOR_H
#define LEAPFROG_ITERATOR_H

#include <iostream>
#include "Triple.h"
#include "triple_bwt.hpp"

class Iterator {
public:
    Triple* triple;
    triple_bwt* graph;
    string index_name;
    int current_level;
    bwt_interval i_s;
    bwt_interval i_p;
    bwt_interval i_o;
    uint64_t cur_s;
    uint64_t cur_p;
    uint64_t cur_o;
    bool is_empty;
    
    Iterator(Triple* triple, triple_bwt* graph) {
        this->triple = triple;
        this->graph = graph;
        this->current_level = -1;
        this->is_empty = false;

        if (triple->s_score == 2 && triple->p_score == -1 && triple->o_score == 1) {
            this->index_name = "POS";
            
            // Down from before-first to first
            this->i_p = this->graph->open_POS();
            this->current_level += 1;

            this->cur_p = triple->p->constant;

        } 

        // PSO
        else if (triple->s_score == 1 && triple->p_score == -1 && triple->o_score == 2) {
            this->index_name = "PSO";
            
            // Down from before-first to first
            this->i_p = this->graph->open_PSO();
            this->current_level += 1;

            this->cur_p = triple->p->constant;

        }

    }

    void down() {
        // POS
        if (this->index_name.compare("POS") == 0) {
            if (this->current_level == 0) {
                this->current_level += 1;
                this->i_o = this->graph->down_P(this->cur_p);
                this->cur_o = this->graph->min_O_in_P(this->i_o);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_s = this->graph->down_P_O(this->i_o, this->cur_p, this->cur_o);
                this->cur_s = this->graph->min_S_in_PO(this->i_s);
            }
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == 0) {
                this->current_level += 1;
                this->i_s = this->graph->down_P(this->cur_p);
                this->cur_s = this->graph->min_S_in_P(this->i_s);
            } else if (this->current_level == 1) {
                this->current_level += 1;
                this->i_o = this->graph->down_P_S(this->i_s, this->cur_s);
                this->cur_o = this->graph->min_O_in_PS(this->i_o);
            }
        }

    }

    vector<uint64_t> down_last() {
        // POS
        if (this->index_name.compare("POS") == 0) {
            this->current_level += 1;
            this->i_s = this->graph->down_P_O(this->i_o, this->cur_p, this->cur_o);
            return this->graph->all_S_in_range(this->i_s);
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            this->current_level += 1;
            this->i_o = this->graph->down_P_S(this->i_s, this->cur_s);
            return this->graph->all_O_in_range(this->i_o);
        }
        
        vector<uint64_t> return_vector;
        return return_vector;

    }

    void up() {
        if (this->current_level >= 0) {
            this->current_level -= 1;
        }    
    }

    void seek(uint64_t next) {
        // POS
        if (this->index_name.compare("POS") == 0) {
            if (this->current_level == 1) {
                this->cur_o = this->graph->next_O_in_P(this->i_o, next);
            } else if (this->current_level == 2) {
                this->cur_s = this->graph->next_S_in_PO(this->i_s, next);
            }
        }

        // PSO
        else if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == 1) {
                this->cur_s = this->graph->next_S_in_P(this->i_s, next);
            } else if (this->current_level == 2) {
                this->cur_o = this->graph->next_O_in_PS(this->i_o, next);
            }
        }

    }

    uint64_t current_value() {
        // POS
        if (this->index_name.compare("POS") == 0) {
            if (this->current_level == 0) {
                return this->cur_p;
            } else if (this->current_level == 1) {
                return this->cur_o;
            } else if (this->current_level == 2) {
                return this->cur_s;
            }
        }

        // PSO
        if (this->index_name.compare("PSO") == 0) {
            if (this->current_level == 0) {
                return this->cur_p;
            } else if (this->current_level == 1) {
                return this->cur_s;
            } else if (this->current_level == 2) {
                return this->cur_o;
            }
        }

        return 0;
    }

};


#endif //LEAPFROG_ITERATOR_H
