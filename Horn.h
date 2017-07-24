#include "Common.h"

#ifndef HORN_H
#define HORN_H

using namespace std;
typedef vector<int> item;
typedef unordered_map<string, int>* m;
class Horn
{
    public:
        Horn(string filename, string partfile);
        virtual ~Horn();
        int nstates;
        int init;
        int nvars;
        vector<int> input;
        vector<int> output;
    protected:
    private:
        vector<string> variables;
        void add_edge(string line, int curstate);
        void read_from_file(string filename, string partfile);
        void read_partfile(string partfile);
        vector<int> edge2bin(vector<string> edge);
        vector<m> trans;
        void print_trans();
        void print_formula();
        string int2bin(int n, int num);
        void map_insert(int curstate, vector<int> assignment, int succ);
        vector<string> item2strings(vector<int> line);

        void print_vec_string(vector<string> v){
            for (size_t n = 0; n < v.size(); n++)
                cout << v[n] << " ";
            cout << endl;
        };

        void print_vec_int(vector<int> v){
            for (size_t n = 0; n < v.size(); n++)
                cout<< v[n] << " ";
            cout << endl;
        };

        void print_2vec_int(vector<item> v){
            for (size_t n = 0; n < v.size(); n++)
                print_vec_int(v[n]);
            cout << endl;
        };

        bool strfind(string str, string target){
            size_t found = str.find(target);
            if(found != string::npos)
                return true;
            else
                return false;
        };
};

#endif // HORN_H
