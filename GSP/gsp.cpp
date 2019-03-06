#include <bits/stdc++.h>
using namespace std;

#define MAX_SYMBOL 30
#define BASE 'a' 


/*
 * A c++ implementation of GSP 
 */

struct Pattern{
	string p; //pattern 
	set<int>S; //occurrence vector set 
};


vector<vector<Pattern>>generatedFrequentPatterns;
 
vector<string>transactions; //to save the information about transactions 
vector<vector<string>>modifiedTransactions;  

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

 
void processTransactions(){
	//the symbol which does not satisfy the min support count threshold will be deleted from the list 
	string s="";
	vector<string>V;
	for(int i=0;i<(int)transactions.size();i++){
		s=transactions[i];
		V = split(s);
		modifiedTransactions.push_back(V);
	}
	return;
}

void transactionRead(){
	//function to read input from file 
	freopen("../Dataset/in2.txt","r",stdin);
	int n;
	scanf("%d",&n);
	string s; 
	for(int i=0;i<n;i++){
		cin>>s; 
		transactions.push_back(s);
	}
	return; 
}

void oneLengthFrequentItemsetGen(int minSupCount){
	//function to generate one length frequent patterns 
	map<int,set<int>>M;
	vector<Pattern>temp; 
	
	for(int i=0;i<(int)modifiedTransactions.size();i++){
		for(int j=0;j<(int)modifiedTransactions[i].size();j++){
			for(int k=0;k<(int)modifiedTransactions[i][j].size();k++){
				M[modifiedTransactions[i][j][k]-BASE].insert(i);
			}
		}
	}
	string s;
	for(auto it=M.begin();it!=M.end();it++){
			if((int)it->second.size() >= minSupCount){
				s="{";
				s=s+(char)(it->first+BASE);
				s=s+"}";
				temp.push_back({s,it->second});
			}
	}
	generatedFrequentPatterns.push_back(temp);
	return;
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

bool patternEmbeddedFunctionChecking(string a,string b){
	// b must contain a 
	int countArr[MAX_SYMBOL+1];
	memset(countArr,0,sizeof(countArr));
	
	for(int i=0;i<(int)a.size();i++){
		countArr[a[i]-BASE]++;
	}
	for(int i=0;i<(int)b.size();i++){
		if(countArr[b[i]-BASE] != 0) {
			countArr[b[i]-BASE]--;
		}
	}
	
	for(int i=0;i<(int)a.size();i++){
		if(countArr[a[i]-BASE] != 0) {
			return false;
		}
	}
	return true;
}


set<int> patternFreqCount(vector<string>V){
	//function to calculate the frequency of a pattern in a database 
	int st=0; //starting pointer of V
	bool patternEmbedded;
	set<int>S;
	for(int i=0;i<(int)modifiedTransactions.size();i++){
		st=0;
		for(int j=0;j<(int)modifiedTransactions[i].size();j++){
			patternEmbedded = patternEmbeddedFunctionChecking(V[st],modifiedTransactions[i][j]);
			if(patternEmbedded) {
				st++;
			}
			if(st>=(int)V.size()) {
				S.insert(i);
				break;
			} 
		}
	}
	return S;
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

vector<Pattern> sStep(int minSupCount){
	//function to generate and test the s-step products for second level
	vector<string>first,second,merge;
	set<int>S;
	vector<Pattern>gen; 
	for(int i=0;i<(int)generatedFrequentPatterns[0].size();i++){
		first=split(generatedFrequentPatterns[0][i].p);
		for(int j=0;j<(int)generatedFrequentPatterns[0].size();j++){
			merge.clear();
			second=split(generatedFrequentPatterns[0][j].p);
			merge.insert(merge.end(),first.begin(),first.end());
			merge.insert(merge.end(),second.begin(),second.end());
			S=patternFreqCount(merge);
			if((int)S.size()>=minSupCount){
				gen.push_back({stringVectorMerger(merge),S});
			}
		}
	}
	return gen; 
}


vector<Pattern> iStep(int minSupCount){
	vector<string>first,second,merge;
	set<int>S;
	vector<Pattern>gen; 
	for(int i=0;i<(int)generatedFrequentPatterns[0].size();i++){
		first=split(generatedFrequentPatterns[0][i].p);
		for(int j=i+1;j<(int)generatedFrequentPatterns[0].size();j++){
			second=split(generatedFrequentPatterns[0][j].p);
			merge.clear();
			merge.push_back(first[0]+second[0]);
			S=patternFreqCount(merge);
			if((int)S.size()>=minSupCount){
				gen.push_back({stringVectorMerger(merge),S});
			}
		}
	}
	return gen;
}


vector<string> join(int level){
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
	vector<string>joinResults;
	bool sameItemset=false;
	string temp;
	for(int i=0;i<(int)generatedFrequentPatterns[level].size();i++){
		left=generatedFrequentPatterns[level][i].p.substr(2,generatedFrequentPatterns[level][i].p.size()-2);
		if(left[0] == '}') {
			left=left.substr(1,left.size()-1);
		}
		else{
			left="{"+left;
		}
		//Debug: cout<<"main = "<<generatedFrequentPatterns[level][i].p<<" left = "<<left<<endl;
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
			//Debug: cout<<"right = "<<right<<" "<<left<<endl;
			if(right==left){
				//will merge 
				if(sameItemset){
					temp=generatedFrequentPatterns[level][i].p;
					temp[temp.size()-1]=generatedFrequentPatterns[level][j].p[generatedFrequentPatterns[level][j].p.size()-2];
					temp=temp+"}";
					joinResults.push_back(temp);
				}
				else{
					temp=generatedFrequentPatterns[level][i].p;
					temp=temp+"{"+generatedFrequentPatterns[level][j].p[generatedFrequentPatterns[level][j].p.size()-2]+"}";
					joinResults.push_back(temp);
				}
			}	
			else if(right<left){
				continue;
			} 
		}
	}
	return joinResults;
}

vector<string> prune(int level,vector<string>joinResults){
	map<string,int>checker;
	for(int i=0;i<(int)generatedFrequentPatterns[level].size();i++){
		checker[generatedFrequentPatterns[level][i].p]=1;
	}
	string left="",right="",mix="";
	bool allSubsetExist=false;
	vector<string>pruneResults;
	for(int i=0;i<(int)joinResults.size();i++){
		allSubsetExist=true;
		for(int j=0;j<(int)joinResults[i].size();j++){
			if(joinResults[i][j] >='a' && joinResults[i][j]<='z'){
				left=joinResults[i].substr(0,j);
				right=joinResults[i].substr(j+1,joinResults[i].size()-1-j);
				if(left[left.size()-1]=='{' && right[0]=='}'){
					//single symbol getting lost 
					left=left.substr(0,left.size()-1);
					right=right.substr(1,right.size()-1);
				}
				mix=left+right;
				if(checker[mix]==0) {
					allSubsetExist=false;
						break;
				}
			}
		}
		if(allSubsetExist){
			pruneResults.push_back(joinResults[i]);
		}
	}
	return pruneResults;
}

void patternMining(int minSupCount){
	//function to mine the patterns 
	int prev;
	vector<string>joinResults,pruneResults;
	vector<string>splitResult;
	set<int>occCnt;
	vector<Pattern>patternSave;
	
	for(int length=1; ;length++){
		if(length==1){
			prev=generatedFrequentPatterns.size();
			oneLengthFrequentItemsetGen(minSupCount);
			if(prev == (int)generatedFrequentPatterns.size()) {
				//no new item set generated 
				break;
			}
			if((int)generatedFrequentPatterns[0].size()==1) { //no more candidate generation is possible 
				break;
			}
		}
		else if(length == 2) {
			// s step working 
			vector<Pattern> sStepGen = sStep(minSupCount);
			vector<Pattern> iStepGen = iStep(minSupCount);
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
		}
		else{
			//join
			joinResults = join(generatedFrequentPatterns.size()-1);
			//Debug: cout<<"DONE HERE"<<endl;
			//prune
			pruneResults = prune(generatedFrequentPatterns.size()-1,joinResults);
			patternSave.clear();
			for(int i=0;i<(int)pruneResults.size();i++){
				splitResult=split(pruneResults[i]);
				occCnt=patternFreqCount(splitResult);
				if((int)occCnt.size() >= minSupCount){
					patternSave.push_back({pruneResults[i],occCnt});
				}
			}
			if((int)patternSave.size()>0){
				generatedFrequentPatterns.push_back(patternSave);
			}
			else {
				break;
			}
			if((int)patternSave.size() == 1){
				//no more possible 
				break; 
			}
		}
	}
	print();
}


int main(void){
	//transaction reading 
	transactionRead();
	//processing transactions 
	processTransactions(); 
	//mining patterns 
	patternMining(2);
	return 0;
}
