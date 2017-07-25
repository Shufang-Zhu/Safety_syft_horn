#include "Horn.h"

using namespace std;
using namespace boost;



Horn::Horn(string filename, string partfile)
{
    //ctor
    read_from_file(filename, partfile);
    // print_trans();
    print_formula();


}

Horn::~Horn()
{
    //dtor
}

void Horn::print_trans(){
    for(int i = 0; i < nstates; i++){
        cout<<i<<endl;
        for (auto& x: *(trans[i]))
            std::cout << x.first << ": " << x.second << std::endl;
    }
}

void Horn::read_from_file(string filename, string partfile){
    ifstream f(filename);
	if(f.is_open()){
        bool flag = 0;
        bool flag_state = 0;
        string line;
        int curstate;
        vector <string> fields;
		while(getline(f, line)){
            if(flag == 0){
                if(strfind(line, "States")){
                    split(fields, line, is_any_of(" "));
                    nstates = stoi(fields[1]);
                }
                else if(strfind(line, "Start")){
                    split(fields, line, is_any_of(" "));
                    init = stoi(fields[1]);
                    //cout<<init<<endl;
                }
                else if(strfind(line, "AP")){
                    split(fields, line, is_any_of(" "));
                    nvars = stoi(fields[1]);
                    for(int i = 2; i < fields.size(); i++){
                        string tmp = fields[i];
                        tmp.pop_back();
                        tmp = tmp.erase(0,1);
                        variables.push_back(tmp);
                    }
                    read_partfile(partfile);
                }
                else if(strfind(line, "BODY"))
                    flag = 1;
                else
                    continue;
            }
            else{
                if(strfind(line, "END"))
                    break;
                if(strfind(line, "State:")){
                    split(fields, line, is_any_of(" "));
                    curstate = stoi(fields[1]);
                    unordered_map<string, int>* sio = new unordered_map<string, int>;
                    trans.push_back(sio);

                    continue;
                }
                else{
                    add_edge(line, curstate);
                }

            }
		}

	}
	f.close();
}



vector<int> Horn::edge2bin(vector<string> edge){
    bool value = 0;
    vector<int> assignment;
    int c;
    for(int i = 0; i < nvars; i++)
        assignment.push_back(2);
    for(int i = 0; i < edge.size(); i++){
        string var = edge[i];
        if(var[0] == '!'){
            var = var.erase(0,1);
            value = 0;
        }
        else
            value = 1;
        c = stoi(var);

        if(value == 0)
            assignment[c] = 0;
        else
            assignment[c] = 1;;
    }
    vector<int> res;
    for(int i = 0; i < input.size(); i++)
        res.push_back(assignment[input[i]]);
    for(int i = 0; i < output.size(); i++)
        res.push_back(assignment[output[i]]);

    return res;

}

string Horn::int2bin(int n, int num){
   
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
    
    for(int i = res.size(); i < num; i++)
        res = "0"+res;
    return res;
}


void Horn::print_formula(){
    ofstream cnf;
    cnf.open("hornf");
    cnf<<"p cnf 1000 1000"<<endl;

    cnf<<"-"<<(init+1)<<" 0"<<endl;
    int allinputs = pow(2, input.size());
    int alloutputs = pow(2, output.size());
    int offset = nstates+nstates*allinputs;
    int ps;
    int psb; //b is input, a is output
    //psb = nstates+(s-1)*allinputs+input
    //(s,b) = (psb/allinputs+1,psb%allinputs)
    int psba;
    //psba = offset+(s-1)*nb*na + (b-1)*na + output
    //s = psba/(nb*na)+1
    //b = (psba-(s-1)*nb*na)/na+1
    //a = (psba-(s-1)*nb*na)%na
    //cout<<allinputs<<endl;
    //cout<<alloutputs<<endl;

    for(int i = 1; i <= nstates; i++){
        ps = i;
        for(int j = 1; j <= allinputs; j++){
            psb = nstates+(ps-1)*allinputs+j;
            cnf<<ps<<" -"<<psb<<" 0"<<endl;
            cnf<<psb<<" ";
            for(int m = 1; m <= alloutputs; m++){
                psba = offset+(ps-1)*allinputs*alloutputs + (j-1)*alloutputs + m;
                cnf<<"-"<<psba<<" ";
            }
            cnf<<"0"<<endl;

            unordered_map<string,int>::const_iterator got;
            for(int m = 1; m <= alloutputs; m++){
                psba = offset+(ps-1)*allinputs*alloutputs + (j-1)*alloutputs + m;
                string sinput = int2bin(j-1,input.size());
                string soutput = int2bin(m-1,output.size());
                string label = sinput+soutput;
                // cout<<i<<" "<<label<<endl;
                got = (*(trans[i-1])).find(label);
                if(got == (*(trans[i-1])).end()){
                    cnf<<psba<<" 0"<<endl;
                }
                else{
                    // cnf<<"-"<<psba<<" "<<got->second<<" 0"<<endl;
                    cnf<<psba<<" -"<<got->second<<" 0"<<endl;
                }
            }
        }
    }
    cnf.close();
}

void Horn::get_strategy(string line){
    vector<string> fields;
    split(fields, line, is_any_of(" "));
    //print_vec_string(fields);
    fields.pop_back();
    set<int> winningstates;
    for(int i = 0; i < nstates; i++){
        unordered_map<string, string>* sio = new unordered_map<string, string>;
        strategy.push_back(sio);  
        if(fields[i][0] == '-'){
            fields[i].erase(0,1);
            winningstates.insert(stoi(fields[i]));
            // cout<<stoi(fields[i])-1<<" ";
        }
        else {
            // cout<<"-"<<stoi(fields[i])-1<<" ";
        }
        
    }
    // cout<<endl;
    int allinputs = pow(2, input.size());
    int alloutputs = pow(2, output.size());
    int offset = nstates+nstates*allinputs;
    //psba = offset+(s-1)*nb*na + (b-1)*na + output
    //s = psba/(nb*na)+1
    //b = (psba-(s-1)*nb*na)/na+1
    //a = (psba-(s-1)*nb*na)%na

    for(int j = offset; j < fields.size(); j++){
        //cout<<fields[j]<<" ";
        if(fields[j][0] == '-'){
            fields[j].erase(0,1);
            int psba = stoi(fields[j]) - offset;
            int s = ceil((float)psba/(allinputs*alloutputs));
            int in = ceil((float)(psba-(s-1)*allinputs*alloutputs)/alloutputs);

            int out = psba-(s-1)*allinputs*alloutputs-(in-1)*alloutputs;
            
            string sinput = int2bin(in-1, input.size());
            string soutput = int2bin(out-1, output.size());

            unordered_map<string,string>::const_iterator got;
            unordered_map<string,int>::const_iterator winstate;
            winstate = (*(trans[s-1])).find(sinput+soutput);
            if(winstate != (*(trans[s-1])).end()){
                int win = winstate->second;
                // cout<<win<<endl;
                if(winningstates.find(win) == winningstates.end()){
                    // cout<<"not "<<win<<endl;
                    continue;
                }
            }

            got = (*(strategy[s-1])).find(sinput);
            if(got == (*(strategy[s-1])).end()){
                strategy_insert(s-1,sinput,soutput);
                // cout<<s-1<<" "<<sinput<<" "<<soutput<<endl;
            }
            else
                continue;    
        }
        


    }
    

}

bool Horn::realizability(){
    ifstream f("res");
    if(f.is_open()){
        string line;
        while(getline(f, line)){
            if(line == "UNSAT")
                return false;
            else{
                getline(f,line);
                get_strategy(line);
                return true;
            }
        }
    }
    f.close();
}


void Horn::add_edge(string line, int curstate){


    if(strfind(line, "t")){
        line = "[0 | !0] "+ to_string(curstate);   
    }

    //get the successor
    vector<string> field;
    split(field, line, is_any_of(" "));

    if(strfind(field.back(),"}")){
        while(true){
            field.pop_back();
            if(strfind(field.back(),"{"))
                break;
        }
        field.pop_back();
    }
    int succesor = stoi(field.back());
    field.pop_back();


    //get all transitons
    for(int i = 0; i < field.size(); i++){
        if(field[i] == "|")
            field.erase (field.begin()+i);

        int len = field[i].length();
        if((field[i][-1] == ' ') | (field[i][len-1] == ']'))
            field[i].pop_back();
        if((field[i][0] == ' ') | (field[i][0] == '['))
            field[i].erase(0,1);

    }

    vector<string> condition; //all transitions
    for(int i = 0; i < field.size(); i++){

        split(condition, field[i], is_any_of("&"));
        //print_vec_string(field);
        vector<int> assignment = edge2bin(condition);
        //print_vec_int(assignment);
        map_insert(curstate,assignment,succesor);
    }


}

void Horn::map_insert(int curstate, vector<int> assignment, int succ){
    vector<string> allassigns = item2strings(assignment);
    unordered_map<string, int>* s;
    s = trans[curstate];
    for(int i = 0; i < allassigns.size(); i++)
        (*s).insert({allassigns[i],succ+1});
}

void Horn::strategy_insert(int curstate, string sinput, string soutput){
    unordered_map<string, string>* s;
    s = strategy[curstate];
    (*s).insert({sinput,soutput});
}

vector<string> Horn::item2strings(vector<int> line){
    vector<string> lines;
    stringstream ss;
    copy(line.begin(), line.end(), ostream_iterator<int>(ss, ""));
    string s = ss.str();
    lines.push_back(s);
    vector<string> res;
    while(!lines.empty()){
        string top = lines.back();
        size_t found = top.find("2");
        if(found != string::npos){
            lines.pop_back();
            string top1 = top;
            top.replace(found, 1, "0");
            top1.replace(found, 1, "1");
            lines.push_back(top);
            lines.push_back(top1);
        }
        else{
            lines.pop_back();
            res.push_back(top);
        }
    }
    //for(int i = 0; i < res.size(); i++)
        //cout<<res[i]<<endl;
    return res;

}

void Horn::read_partfile(string partfile){
    ifstream f(partfile);
    vector<string> inputs;
    vector<string> outputs;
    string line;
    while(getline(f, line)){
        if(f.is_open()){
            if(strfind(line, "inputs")){
                split(inputs, line, is_any_of(" "));
                //print_vec_string(inputs);
            }
            else if(strfind(line, "outputs")){
                split(outputs, line, is_any_of(" "));
                //print_vec_string(outputs);
            }
            else
                cout<<"read partfile error!"<<endl;
		}
    }
    f.close();
    set<string> input_set;
    set<string> output_set;
    for(int i = 1; i < inputs.size(); i++)
        input_set.insert(inputs[i]);
    for(int i = 1; i < outputs.size(); i++)
        output_set.insert(outputs[i]);


    for(int i = 0; i < variables.size(); i++){
        if(input_set.find(variables[i]) != input_set.end())
            input.push_back(i);
        else if(output_set.find(variables[i]) != output_set.end())
            output.push_back(i);
        else
            cout<<"error"<<endl;
    }
    //print_vec_int(input);
    //print_vec_int(output);
}

