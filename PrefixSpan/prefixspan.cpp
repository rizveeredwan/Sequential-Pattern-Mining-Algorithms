#include <bits/stdc++.h>
using namespace std;

#define MAX_SYMBOL 30
#define BASE 'a' 

struct SameSequenceOcc{
	int sId;
	vector<int>itemNo;
};


struct Pattern{
	string p; //pattern 
	set<int>S; //occurrence vector set 
};



vector<vector<string>>modifiedTransactions; 
vector<Pattern>generatedFrequentPatterns;


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

void patternMiningExpansion(string p,vector<SameSequenceOcc>V, int minSupCount){
	
	int seq;
	int itemNo;

	vector<SameSequenceOcc> VNew;
	vector<int>tempItemNo;
	set<int>seqSave;
	
	for(int i=0;i<MAX_SYMBOL;i++){
		//sequence expansion
		seq=-1;
		VNew.clear();
		seqSave.clear(); 
		for(int j=0;j<(int)V.size();j++){
			tempItemNo.clear(); 
			
			if(seq != -1) {
				if(seq == V[j].sId) {
					continue;
				}
			}
			seq=V[j].sId;
			itemNo=V[j].itemNo[0];
			for(int k=itemNo+1;k<(int)modifiedTransactions[seq].size();k++){
				for(int l=0;l<(int)modifiedTransactions[seq][k].size();l++){
					if((int)(modifiedTransactions[seq][k][l] - BASE) == i){
						tempItemNo.push_back(k);
						break;
					}
				}
			}
			if((int)tempItemNo.size()>0) {
				VNew.push_back({seq,tempItemNo});
				seqSave.insert(seq);
			}
		}
		
		if((int)seqSave.size() >= minSupCount){
			string notun=p+"{";
			notun = notun + (char)(i+BASE);
			notun = notun + "}"; 
			generatedFrequentPatterns.push_back({notun,seqSave});
			/*DEBUG
			cout<<"notun = "<<notun<<endl;
			for(int l=0;l<(int)VNew.size();l++){
				cout<<"sequence = "<<VNew[l].sId<<endl;
				for(int m=0;m<(int)VNew[l].itemNo.size();m++){
					cout<<VNew[l].itemNo[m]<<" ";
				}
				cout<<endl;
			}
			*/
			patternMiningExpansion(notun,VNew,minSupCount);
		}
		
		//itemset expansion
		if(p[p.size()-2] < (char)(i+BASE)){
			//itemset expansion possible 
			seqSave.clear();
			VNew.clear();
			for(int j=0;j<(int)V.size();j++){
				seq=V[j].sId;
				tempItemNo.clear(); 
				for(int k=0;k<(int)V[j].itemNo.size();k++){
					for(int l=0;l<(int)modifiedTransactions[seq][V[j].itemNo[k]].size();l++){
						if((modifiedTransactions[seq][V[j].itemNo[k]][l]-BASE) == i){
							//exists in same item set 
							tempItemNo.push_back(V[j].itemNo[k]);
							break;
						}
					}
				}
				if((int)tempItemNo.size() > 0) {
					seqSave.insert(seq);
					VNew.push_back({seq,tempItemNo});
				}
			}
			if((int)seqSave.size() >= minSupCount){
				string notun=p;
				notun[p.size()-1]=(char)(i+BASE);
				notun=notun+"}";
				generatedFrequentPatterns.push_back({notun,seqSave});
				/*cout<<"notun = "<<notun<<endl;
				for(int l=0;l<(int)VNew.size();l++){
					cout<<"sequence = "<<VNew[l].sId<<endl;
					for(int m=0;m<(int)VNew[l].itemNo.size();m++){
						cout<<VNew[l].itemNo[m]<<" ";
					}
					cout<<endl;
				}*/
				patternMiningExpansion(notun,VNew,minSupCount);
			}
		}
	}
}

void patternMiningInitiate(int minSupCount){
	vector<SameSequenceOcc>ssocc;
	vector<int>tempItemNo;
		
	set<int>S;
	
	for(int i=0;i<MAX_SYMBOL;i++){
		ssocc.clear();
		for(int j=0;j<(int)modifiedTransactions.size();j++){
			tempItemNo.clear(); 
			for(int k=0;k<(int)modifiedTransactions[j].size();k++){
				for(int l=0;l<(int)modifiedTransactions[j][k].size();l++){
					if((modifiedTransactions[j][k][l]-BASE) == i){
						tempItemNo.push_back(k);
					}
				}
			}
			if((int)tempItemNo.size() > 0){
				ssocc.push_back({j,tempItemNo});
			}
		}
		if((int)ssocc.size() >= minSupCount){
			S.clear();
			for(int j=0;j<(int)ssocc.size();j++){
				S.insert(ssocc[j].sId);
			}
			string temp="{";
			temp=temp+(char)(i+BASE)+"}";
			generatedFrequentPatterns.push_back({temp,S});
			
			patternMiningExpansion(temp,ssocc,minSupCount);
		}
	}
}

void print(){
	//int sum=0;
	for(int i=0;i<(int)generatedFrequentPatterns.size();i++){
		cout<<generatedFrequentPatterns[i].p<<" ";
		for(auto it=generatedFrequentPatterns[i].S.begin();it !=generatedFrequentPatterns[i].S.end();it++){
			cout<<*it<<" ";
		}
		cout<<endl;
	}
	cout<<"total patterns = "<<generatedFrequentPatterns.size()<<endl;
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
		modifiedTransactions.push_back(temp);
	}
	return; 
}


int main(void){
	transactionRead();
	patternMiningInitiate(2);
	print();
}
