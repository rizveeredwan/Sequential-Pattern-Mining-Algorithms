#include <bits/stdc++.h>
using namespace std;

#define MAX_SYMBOL 30
#define BASE 'a' 

//A C++ implementation of SPADE algorithm 

struct Occurrence{
	int sId,eId; 
};

struct Pattern{
	string p; //pattern 
	set<int>S; //occurrence vector set 
	vector<Occurrence>occ; //saving the occurrence database 
};

vector<vector<Pattern>>generatedFrequentPatterns;
map<string,vector<Occurrence>> initialDbScan;


vector<string> split(string s){
	//function to split s by symbol '{' and '}'
	vector<string>temp;
	string save="";
	for(int i=0;i<(int)s.size();i++){
		if(s[i]=='{') save="";
		else if(s[i]=='}') {
			temp.push_back(save);
		}
		else{
			save=save+s[i];
		}
	}
	return temp;
}


bool cmp(Pattern a, Pattern b){
	vector<string>a_v= split(a.p);
	vector<string>b_v=split(b.p);
	for(int i=0;i<(int)a_v.size();i++){
		if(((int)b_v.size()-1)>=i) {
			if(a_v[i]<b_v[i]) {
				return true;
			}
			if(a_v[i]>b_v[i]) {
				return false;
			}
		}
	}
	return false;
} 


void print(){
	int sum=0;
	for(int i=0;i<(int)generatedFrequentPatterns.size();i++){
		sum += generatedFrequentPatterns[i].size();
		for(int j=0;j<(int)generatedFrequentPatterns[i].size();j++){
			cout<<generatedFrequentPatterns[i][j].p<<" ";
			for(auto it=generatedFrequentPatterns[i][j].S.begin(); it !=generatedFrequentPatterns[i][j].S.end() ;it++){
				cout<<*it<<" ";
			}
			cout<<endl;
		}
	}
	cout<<"total patterns = "<<sum<<endl;
	return;
}

string stringVectorMerger(vector<string>V){
	//function is to merge a string vector 
	string s="";
	for(int i=0;i<(int)V.size();i++){
		s=s+"{";
		s+=V[i];
		s+="}";
	}
	return s;
}

vector<Occurrence> sequenceExpandVector(vector<Occurrence>left,vector<Occurrence>right){
	int i=0,j=0;
	vector<Occurrence>merge;
	for(i=0;i<(int)left.size();i++){
		for(;j<(int)right.size();j++){
			if(right[j].sId == left[i].sId){
				if(right[j].eId>left[i].eId){
					//good to take 
					merge.push_back({right[j].sId,right[j].eId});
					continue;
				}
				if(right[j].eId == left[i].eId){
					//good to take 
					continue;
				}
				if(right[j].eId<left[i].eId){
					continue;
				}
			}
			if(right[j].sId>left[i].sId){
				break;
			}
			if(right[j].sId<left[i].sId){
				continue; 
			}
		}
	}
	return merge;
}

vector<Occurrence> itemsetExpandVector(vector<Occurrence>left,vector<Occurrence>right){
	int i=0,j=0;
	vector<Occurrence>merge;
	for(i=0;i<(int)left.size();i++){
		for(;j<(int)right.size();j++){
			if(right[j].sId == left[i].sId){
				if(right[j].eId == left[i].eId){
					//good to take 
					merge.push_back({right[j].sId,right[j].eId});
					break;
				}
				if(right[j].eId > left[i].eId){
					break;
				}
				if(right[j].eId<left[i].eId){
					continue; //may come in future  
				}
			}
			if(right[j].sId>left[i].sId){
				break;
			}
			if(right[j].sId<left[i].sId){
				continue; 
			}
		}
	}
	return merge;
}


vector<Pattern> sStep(int minSupCount){
	//function to generate and test the s-step products for second level
	vector<string>first,second,merge;
	vector<Occurrence>occ;
	set<int>S;
	vector<Pattern>gen; 
	for(int i=0;i<(int)generatedFrequentPatterns[0].size();i++){
		first=split(generatedFrequentPatterns[0][i].p);
		for(int j=0;j<(int)generatedFrequentPatterns[0].size();j++){
			merge.clear();
			second=split(generatedFrequentPatterns[0][j].p);
			merge.insert(merge.end(),first.begin(),first.end());
			merge.insert(merge.end(),second.begin(),second.end());
			occ=sequenceExpandVector(generatedFrequentPatterns[0][i].occ,generatedFrequentPatterns[0][j].occ);
			S.clear();
			for(int k=0;k<(int)occ.size();k++){
				S.insert(occ[k].sId);
			}
			if((int)S.size()>=minSupCount){
				gen.push_back({stringVectorMerger(merge),S,occ});
			}
		}
	}
	return gen; 
}

vector<Pattern> iStep(int minSupCount){
	vector<string>first,second,merge;
	set<int>S;
	vector<Pattern>gen; 
	vector<Occurrence>occ;
	for(int i=0;i<(int)generatedFrequentPatterns[0].size();i++){
		first=split(generatedFrequentPatterns[0][i].p);
		for(int j=i+1;j<(int)generatedFrequentPatterns[0].size();j++){
			second=split(generatedFrequentPatterns[0][j].p);
			merge.clear();
			merge.push_back(first[0]+second[0]);
			occ=itemsetExpandVector(generatedFrequentPatterns[0][i].occ,generatedFrequentPatterns[0][j].occ);
			S.clear();
			for(int k=0;k<(int)occ.size();k++){
				S.insert(occ[k].sId);
			}
			if((int)S.size()>=minSupCount){
				gen.push_back({stringVectorMerger(merge),S,occ});
			}
		}
	}
	return gen;
}

vector<Pattern> spadeJoin(int level,int minSupCount){
	//function to perform join
	map<int,int>pos;
	//saving the positions to start with 
	for(int i=0;i<(int)generatedFrequentPatterns[level].size();i++){
		if(pos[generatedFrequentPatterns[level][i].p[1]-BASE]==0) {
			//because 0 index is {, so 1st character
			pos[generatedFrequentPatterns[level][i].p[1]-BASE]=i+1; //willingly increase value 1	
		}
	}
	//joining
	string left,right; 
	vector<Pattern>joinResults;
	bool sameItemset=false;
	string temp;
	set<int>S;
	for(int i=0;i<(int)generatedFrequentPatterns[level].size();i++){
		left=generatedFrequentPatterns[level][i].p.substr(2,generatedFrequentPatterns[level][i].p.size()-2);
		if(left[0] == '}') {
			left=left.substr(1,left.size()-1);
		}
		else{
			left="{"+left;
		}
		/*DEBUG 
		cout<<"main = "<<generatedFrequentPatterns[level][i].p<<" left = "<<left<<endl;
		for(int l=0;l<(int)generatedFrequentPatterns[level][i].occ.size();l++){
			cout<<generatedFrequentPatterns[level][i].occ[l].sId<<" "<<generatedFrequentPatterns[level][i].occ[l].sId<<endl;
		}*/
		for(int j=pos[left[1]-BASE]-1;j<(int)generatedFrequentPatterns[level].size() && j>=0 && generatedFrequentPatterns[level][j].p[1]==left[1];j++){
			sameItemset=false;
			right=generatedFrequentPatterns[level][j].p.substr(0,generatedFrequentPatterns[level][j].p.size()-2);
			if(right[right.size()-1]=='{'){
				right=right.substr(0,right.size()-1);
			}
			else{
				right=right+"}";
				sameItemset=true;
			}
			/*
			 * DEBUG
			 * cout<<"right main = "<<generatedFrequentPatterns[level][j].p<<" right = "<<right<<" "<<left<<endl;
			for(int l=0;l<(int)generatedFrequentPatterns[level][j].occ.size();l++){
				cout<<generatedFrequentPatterns[level][j].occ[l].sId<<" "<<generatedFrequentPatterns[level][j].occ[l].sId<<endl;
			}*/
			if(right==left){
				//will merge 
				if(sameItemset){
					if(generatedFrequentPatterns[level][i].p[generatedFrequentPatterns[level][i].p.size()-2] < generatedFrequentPatterns[level][j].p[generatedFrequentPatterns[level][j].p.size()-2]) {
						temp=generatedFrequentPatterns[level][i].p;
						temp[temp.size()-1]=generatedFrequentPatterns[level][j].p[generatedFrequentPatterns[level][j].p.size()-2];
						temp=temp+"}";
						
						vector<Occurrence>occ = itemsetExpandVector(generatedFrequentPatterns[level][i].occ,generatedFrequentPatterns[level][j].occ);
						S.clear();
						for(int k=0;k<(int)occ.size();k++){
							S.insert(occ[k].sId);
						}
						if((int)S.size() >= minSupCount){
							joinResults.push_back({temp,S,occ});
						}
					}
				}
				else{
					temp=generatedFrequentPatterns[level][i].p;
					temp=temp+"{"+generatedFrequentPatterns[level][j].p[generatedFrequentPatterns[level][j].p.size()-2]+"}";
					
				
					vector<Occurrence>occ = sequenceExpandVector(generatedFrequentPatterns[level][i].occ,generatedFrequentPatterns[level][j].occ);
					S.clear();
					for(int k=0;k<(int)occ.size();k++){
						S.insert(occ[k].sId);
					}
					if((int)S.size() >= minSupCount){
						joinResults.push_back({temp,S,occ});
					}
				}
			}	
			else if(right<left){
				continue;
			} 
		}
	}
	return joinResults;
}



void patternMining(int minSupCount){
	Pattern temp;
	vector<Pattern>V;
	vector<Pattern>joinResults;
	
	for(int length=1;;length++){
		if(length==1){
			V.clear();
			for(auto it=initialDbScan.begin();it != initialDbScan.end();it++){
				temp.p="{"+it->first+"}";
				temp.occ=it->second;
				temp.S.clear();
				for(int i=0;i<(int)temp.occ.size();i++){
					temp.S.insert(temp.occ[i].sId);
				}
				if((int)temp.S.size() >= minSupCount){
					V.push_back(temp);
				}
			}
			if(V.size()>0){
				generatedFrequentPatterns.push_back(V);
			}
			if(V.size() == 0 || V.size() == 1) {
				break;
			}
		}
		else if(length == 2){
			vector<Pattern>sStepGen=sStep(minSupCount);
			vector<Pattern>iStepGen=iStep(minSupCount);
			if(sStepGen.size() == 0 && iStepGen.size()== 0) {
				break;
			}
			vector<Pattern>merge;
			merge.insert(merge.end(),sStepGen.begin(),sStepGen.end());
			merge.insert(merge.end(),iStepGen.begin(),iStepGen.end());
			sort(merge.begin(),merge.end(),cmp);
			generatedFrequentPatterns.push_back(merge);
			if(merge.size() == 1) { //no more candidate generation is possible 
				break; 
			} 
			
			/*
			 * DEBUG 
			for(int i=0;i<(int)merge.size();i++){
				cout<<merge[i].p<<endl;
				for(int j=0;j<(int)merge[i].occ.size();j++){
					cout<<merge[i].occ[j].sId<<" "<<merge[i].occ[j].eId<<endl;
				}
				cout<<endl;
			}*/
			
		}
		else{
			//join
			joinResults = spadeJoin(generatedFrequentPatterns.size()-1,minSupCount);
			sort(joinResults.begin(),joinResults.end(),cmp);
			
			if(joinResults.size() > 0 ){
				generatedFrequentPatterns.push_back(joinResults);
			}
				
			if(joinResults.size() <= 0) {
				break;
			}
		}
	}
	print();
	return;
}




void transactionRead(){
	//function to read input from file 
	freopen("../Dataset/in2.txt","r",stdin);
	int n;
	scanf("%d",&n);
	string s,p; 
	vector<string>temp;
	int arr[MAX_SYMBOL+5];
	memset(arr,0,sizeof(arr));
	
	
	for(int i=0;i<n;i++){
		cin>>s; 
		temp=split(s);
		for(int j=0;j<(int)temp.size();j++){
			for(int k=0;k<(int)temp[j].size();k++){
				if(arr[temp[j][k]-BASE]==0) {
					//need to initialize a vector in map 
					arr[temp[j][k]-BASE]=1; 
					p=temp[j][k];
					vector<Occurrence>V;
					initialDbScan[p]=V;
				}
				p=temp[j][k];
				initialDbScan[p].push_back({i,j});
			}
		}
	}
	
	//Debug
	/*
	for(auto it=initialDbScan.begin();it != initialDbScan.end();it++){
		cout<<it->first<<" "<<it->second.size()<<endl;
		for(int i=0;i<(int)it->second.size();i++){
			cout<<it->second[i].sId<<" "<<it->second[i].eId<<endl;
		}
		cout<<endl;
	}*/
	
	return; 
}

int main(void){
	transactionRead(); //database read 
	patternMining(2);
}
