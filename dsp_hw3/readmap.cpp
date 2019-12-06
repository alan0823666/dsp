#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>  
#include <string.h>
using namespace std;

void read_map(FILE *fpmap, map<string, set<string> > &mapping) {
	char *line_buffer = (char*)malloc(sizeof(char)*4096);
	char *trash = (char*)malloc(sizeof(char)*4096);
	//cout << "fuck\n";
	while(fgets(line_buffer, 4096, fpmap) != NULL) {
		//cout << "fuck\n";
		string tmp_string = string(line_buffer);
		//cout << tmp_string << "\n";
		int pos = 0, num = 0;
		string zuyin = tmp_string.substr(0, 2);
		tmp_string = tmp_string.substr(3);
		//cout << zuyin << " ";
		while(1) {
			pos = tmp_string.find(" ");
			if(pos == -1) {
				//mapping[zuyin].insert(tmp_string);
				//num++;
				break;
			}
			string word = tmp_string.substr(0, pos);
			mapping[zuyin].insert(word);
			num++;
			tmp_string = tmp_string.substr(pos + 1);
			//cout << word << " ";
		}
		//cout << num <<"\n";
		for(int i = 0; i < num; i++)
			fgets(trash, 4096, fpmap);

	}
}

void Print(vector<string> line){
    cout << "<s>";
    for(int i = 0 ; i < line.size() ; i++){
       cout << " " << line.at(i);
    }
    cout << " </s>" << endl;
}


vector<string> HandleInputLine(char *Line){

    vector<string> splitLine;
    string buf;
    string tmpLine = string(Line);
    int pos;
    while((pos = tmpLine.find(" ")) != -1){
       tmpLine.erase(pos, 1);              //remove space
    }
   // assert(tmpLine.size() % 2 == 0);
    for(int i = 0 ; i < tmpLine.size() ; i += 2){
       buf = tmpLine.substr(i, 2);        //put_all_word_into_return
       splitLine.push_back(buf);
    }
    return splitLine;
}

vector< set<string> > FindAllPossible(vector<string> InputLine, map<string, set<string> > mapping){
    vector< set<string> > AllNodes;  //return
													//vector   0    1    2    3  
    map<string, set<string> >::iterator map_iter;           //		O             *
    for(int i = 0 ; i < InputLine.size() ; i++){            // O -> O -> O -> O   s
        set<string> TmpSet;                                 //		O	 O		  e
        map_iter = mapping.find(InputLine.at(i));			//		O			  t
        if(map_iter != mapping.end()){						//	                  *
            TmpSet = (map_iter->second);
        }
	    else{
            TmpSet.insert(InputLine.at(i));
        }
        AllNodes.push_back(TmpSet);
    }
    return AllNodes;
}
pair<vector<string>, double> FindBestBigram(vector< pair<vector<string>, double> > Pathes, string CurrentWord, Ngram &lm, Vocab &vocab){

    vector<string>::iterator iter;
    int MaxIndex = 0;
    double Prob, MaxProb = -1000.0;
    for(int i = 0 ; i < Pathes.size() ; i++){
        iter = Pathes.at(i).first.end();
        iter -= 1;
        string PreWord = *(iter);
        double oldProb = Pathes.at(i).second;
        double newProb = getBigramProb(vocab, lm, PreWord.c_str(), CurrentWord.c_str());
        if((Prob = oldProb * newProb) > MaxProb){
            MaxProb = Prob;
            MaxIndex = i;
        }
    }
   // assert(MaxIndex<Pathes.size());
    pair<vector<string>, double> BestPathes = Pathes.at(MaxIndex);
    BestPathes.first.push_back(CurrentWord);
    BestPathes.second = MaxProb;
    return BestPathes;
}

int main(int argc, char *argv[]) {
	
	
	
	//  argv[1] = input_file,,  argv[2] = mapping,,  argv[3] = LM,, argv[4] = out_file
	
	FILE *fp_map = fopen(argv[2], "r");
	map<string, set<string>> mapping;
	read_map(fp_map, mapping);
	/*for(map<string, set<string>>::iterator i = mapping.begin(); i != mapping.end(); i++) {
		cout << i->first;
		for(set<string>::iterator it = i->second.begin(); it != i->second.end(); it++)
			cout << *it << "";
		cout << "\n";
	}*/
	Vocab vocab;
    Ngram lm(vocab, 2);	
	
	File lmFile( argv[3], "r" );
    lm.read(lmFile);
    lmFile.close();
	
	FILE *fp_input = fopen(argv[1], "r");
	char* tmpstr = (char*)malloc(sizeof(char)*4096);
    while((fgets(tmpstr, 4096, fp_input)) != NULL) {
		tmpstr[strlen(tmpstr)-1] = '\0';
		//cout << tmpstr << "\n" ;
		vector<string> input_stings = HandleInputLine(tmpstr);
		/*
		for(vector<string>::iterator it = input_stings.begin(); it != input_stings.end(); it++)
			cout << *it << " ";
		cout << '\n';
		*/
		vector<set<string>> all_node = FindAllPossible(input_stings, mapping);
		/*
		for(vector<set<string>>::iterator it_1 = all_node.begin(); it_1 != all_node.end(); it_1++) {
			for(set<string>::iterator it_2 = it_1->begin(); it_2 != it_1->end(); it_2++)
				cout << *it_2 << " ";
			cout << "end \n";
		}
		cout << "\n";
		*/
		vector <pair <vector <string>, double>> dp;
		for(set<string>::iterator it = all_node.begin()->begin(); it != all_node.begin()->end(); it++) {
			string first_word = *it
			pair <vector <string>, double> dp_0 = make_pair(first_word, 0.0);
			dp.push_back(dp_0);
		}
		for(vector<set<string>>::iterator node_it = all_node.begin(); node_it != all_node.end(); node_it++) {
			if(node_it == all_node.begin())
				continue;
			vector <pair <vector <string>, double>> dp_next;
			for(set<string>::iterator it = node_it->begin(); it != node_it->end(); it++) {
				string current_node = *it;
				pair <vector <string>, double> best_path;
				best_path = FindBestBigram(dp, current_node, lm, vocab);
				dp_next.push_back(best_path);
			}
			dp.clear();
			dp = dp_next;
		}
       vector<string> BestLine;
	   double max = -1000;
	   vector <pair <vector <string>, double>>::iterator best_one;
	   for(vector <pair <vector <string>, double>>::iterator it = dp.begin(); it != dp.end(); it++) {
		   if(it->second > max) {
			   max = it->second;
			   best_one = it;
		   }   
	   }
        Print(best_one -> first);
	}
	
}