#include "Horn.h"

using namespace std;
using namespace boost;



Horn::Horn(string filename, string partfile)
{
    //ctor
    read_from_file(filename, partfile);
    //print_trans();


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
                else if(strfind(line, "init")){
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

void Horn::add_edge(string line, int curstate){
    vector<string> field;
    split(field, line, is_any_of(" "));
    field.pop_back();
    int succesor = stoi(field.back());
    field.pop_back();



    for(int i = 0; i < field.size(); i++){
        if(field[i] == "|")
            field.erase (field.begin()+i);

        int len = field[i].length();
        if((field[i][-1] == ' ') | (field[i][len-1] == ']'))
            field[i].pop_back();
        if((field[i][0] == ' ') | (field[i][0] == '['))
            field[i].erase(0,1);

    }

    vector<string> condition;
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
        (*s).insert({allassigns[i],succ});
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
    print_vec_int(input);
    print_vec_int(output);
}

