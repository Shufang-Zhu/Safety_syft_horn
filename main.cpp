#include <iostream>
#include "Horn.h"
#include "Common.h"
#include <spot/tl/parse.hh>
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/translate.hh>

using namespace std;

int main(int argc, char ** argv)
{
    string filename;
    string partfile;
    string flag;

    
    if(argc != 3){
        cout<<"Usage: ./SSyft DFAfile Partfile Starting_player(0: system, 1: environment)"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
      
    }
    //filename = "load_0.ltl";
    //partfile = "load_0.part";

    clock_t t1,t2,t3;
    t1=clock();
    ifstream f(filename);
    string formula;
    if(f.is_open())
        getline(f, formula);
    f.close();


    spot::parsed_formula pf = spot::parse_infix_psl(formula);
    if (pf.format_errors(std::cerr))
        return 1;
    spot::translator trans;
    trans.set_type(spot::postprocessor::TGBA);
    trans.set_pref(spot::postprocessor::Deterministic);
    spot::twa_graph_ptr aut = trans.run(pf.f);
    ofstream hoa("tmp.hoa");
    spot::print_hoa(hoa, aut) << '\n';
    hoa.close();
    t2=clock();

    
    Horn test("tmp.hoa", partfile);
    system("minisat -verb=0 hornf res");
    bool res = test.realizability();


    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    t3=clock();
    
    float dfa ((float)t2-(float)t1);
    float syn ((float)t3-(float)t2);
    float total ((float)t3-(float)t1);
    float dfatime = dfa / CLOCKS_PER_SEC;
    float syntime = syn / CLOCKS_PER_SEC;
    float totaltime = total / CLOCKS_PER_SEC;
    cout<<"DFA time: "<<dfatime<<endl;
    cout<<"Syn time: "<<syntime<<endl;
    cout<<"Total time: "<<totaltime<<endl;
    return 0;
}
