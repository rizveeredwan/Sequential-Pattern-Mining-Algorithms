#include <bits/stdc++.h>
using namespace std;

#define MAX_SYMBOL 30
#define BASE 'a' 
#define MAX_SEQUENCE_POSSIBLE 500


struct Pattern{
	string p; //pattern 
	set<int>S; //occurrence vector set 
};

vector<Pattern>generatedFrequentPatterns;

vector<string>transactions; //to save the information about transactions 
int maxEvent;
int numberOfBit;


map<string,vector<bitset<MAX_SEQUENCE_POSSIBLE>>>bitMapRepresentationBitset;


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


vector<bitset<MAX_SEQUENCE_POSSIBLE>> sStep(string left, string right){
	//debug: cout<<"left = "<<left<<" "<<right<<endl;
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>vLeft=bitMapRepresentationBitset[left];
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>vRight=bitMapRepresentationBitset[right];
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>result;
	
	int savePos;
	
	for(int i=0;i<(int)vLeft.size();i++){
		if(vLeft[i].count()>0){
			for(int j=numberOfBit-1;j>=0;j--){
				if(vLeft[i][j]==1){
					savePos=j;
					break;
				}
			}
			bitset<MAX_SEQUENCE_POSSIBLE>newLeft;
			for(int j=savePos-1;j>=0;j--){
				newLeft[j]=1;
			}
			
			/*if(left=="{a}" && right == "{a}") {
				cout<<"save pos = "<<savePos<<endl;
				cout<<"new left "<<newLeft<<endl;
				cout<<"v right "<<vRight[i]<<endl; 
				cout<<"result = "<<(newLeft & vRight[i])<<endl;
			}*/
			
			result.push_back(newLeft & vRight[i]);
		}
		else{
			bitset<MAX_SEQUENCE_POSSIBLE>newLeft;
			result.push_back(newLeft);
		}
	}
	return result;
}

vector<bitset<MAX_SEQUENCE_POSSIBLE>> iStep(string left, string right){
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>vLeft=bitMapRepresentationBitset[left];
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>vRight=bitMapRepresentationBitset[right];
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>result;
	for(int i=0;i<(int)vLeft.size();i++){
		result.push_back(vLeft[i] & vRight[i]);
	}
	return result;
}

void lexicographicTree(string pattern,vector<string>validMoves,int minSupCount){
	string s="";
	vector<bitset<MAX_SEQUENCE_POSSIBLE>> sStepRes,iStepRes;
	set<int>S;
	for(int i=0;i<(int)validMoves.size();i++){
		//s-step
		S.clear();
		s=pattern+"{"+validMoves[i]+"}";
		
		sStepRes = sStep(pattern,"{"+validMoves[i]+"}");
		for(int j=0;j<(int)sStepRes.size();j++){
			if(sStepRes[j].count()>0){
				S.insert(j);
			}
		}
		
		if(s == "{a}{a}") cout<<S.size()<<endl;
		
		if((int)S.size() >= minSupCount){
			generatedFrequentPatterns.push_back({s,S});
			bitMapRepresentationBitset[s]=sStepRes;
			lexicographicTree(s,validMoves,minSupCount);
		}
		//i-step
		char a=validMoves[i][0];
		char b=pattern[pattern.size()-2];
		
		if(a>b){
			iStepRes = iStep(pattern,"{"+validMoves[i]+"}");
			S.clear();
			s=pattern.substr(0,pattern.size()-1);
			s=s+validMoves[i];
			s=s+"}";
			for(int j=0;j<(int)iStepRes.size();j++){
				if(iStepRes[j].count()>0){
					S.insert(j);
				}
			}
			if((int)S.size() >= minSupCount){
				generatedFrequentPatterns.push_back({s,S});
				bitMapRepresentationBitset[s]=iStepRes;
				lexicographicTree(s,validMoves,minSupCount);
			}
		}
	}
}

void transactionRead(int minSupCount){
	//function to read input from file 
	// ------------ ../Dataset/in2.txt
	freopen("../Dataset/spmf_dataset.txt","r",stdin);
	int n;
	scanf("%d",&n);
	string s; 
	
	vector<string>temp;
	
	maxEvent = -1;
	int arr[MAX_SYMBOL+5];
	memset(arr,0,sizeof(arr));
	int cnt=0;
	
	for(int i=0;i<n;i++){
		cin>>s; 
		transactions.push_back(s);
		temp=split(s);
		
		
		maxEvent=max(maxEvent,(int)temp.size()); //max event possible in a sequence 
		
		
		for(int j=0;j<(int)temp.size();j++){
			for(int k=0;k<(int)temp[j].size();k++){
				if(arr[temp[j][k]-BASE]==0){
					arr[temp[j][k]-BASE]=1;
					cnt++;
				}
			}
		}
			
	}
	
	numberOfBit = maxEvent;
	
	
	vector<int>power_2;
	power_2.push_back(0);
	int base=1;
	for(int i=1;;i++){
		power_2.push_back(base);
		if(numberOfBit>power_2[i-1] && numberOfBit<=power_2[i]){
			numberOfBit=power_2[i];
			break;
		}
		base *= 2;
	}
	
	
	// MAX_SEQUENCE_POSSIBLE must be greater or equal than numberOfBit 
	assert(MAX_SEQUENCE_POSSIBLE>=numberOfBit);
	
	vector<long long int>bitInit; 
	vector<bitset<MAX_SEQUENCE_POSSIBLE>>bitInitBitset;
	bitset<MAX_SEQUENCE_POSSIBLE>bitSet;
	for(int i=0;i<(int)transactions.size();i++){
		bitInit.push_back(0);
		bitInitBitset.push_back(bitSet);
	}
	
	for(int i=0;i<MAX_SYMBOL;i++){
		if(arr[i] == 1){
			s=(char)(i+BASE);
			bitMapRepresentationBitset["{"+s+"}"]=bitInitBitset;
		}
	}
	
	for(int i=0;i<(int)transactions.size();i++){
		temp=split(transactions[i]);
		for(int j=0;j<(int)temp.size();j++){
			for(int k=0;k<(int)temp[j].size();k++){
				s=temp[j][k];
				bitset<MAX_SEQUENCE_POSSIBLE>bitsetTemp;
				bitsetTemp[numberOfBit-j-1]=1;
				bitMapRepresentationBitset["{"+s+"}"][i] |= bitsetTemp;
			}
		}
	}
	
	cnt=0;
	vector<string>validMoves;
	set<int>S;
	for(int i=0;i<MAX_SYMBOL;i++){
		s=(char)(i+BASE);
		cnt=0;
		S.clear();
		for(int j=0;j<(int)bitMapRepresentationBitset["{"+s+"}"].size();j++){
			if(bitMapRepresentationBitset["{"+s+"}"][j].count()>0){
				cnt++;
				S.insert(j);
			}
			
		}
		/*if((int)S.size() > 0) {
			cout<<"{"+s+"}"<<endl;
			string v = "{"+s+"}";
			for(int k=0;k<(int)bitMapRepresentationBitset[v].size();k++){
				cout<<bitMapRepresentationBitset[v][k]<<endl;
			}
		}*/
		
		if(cnt>=minSupCount){
			validMoves.push_back(s);
			generatedFrequentPatterns.push_back({"{"+s+"}",S});
		}
	}
	
	for(int i=0;i<(int)validMoves.size();i++){
		lexicographicTree("{"+validMoves[i]+"}",validMoves,minSupCount);
	}
	
	/*
	for(auto it=bitMapRepresentation.begin(); it != bitMapRepresentation.end(); it++){
		cout<<it->first<<endl;
		for(int i=0;i<(int)it->second.size();i++){
			cout<<it->second[i]<<" ";
		}
		cout<<endl;
	}*/
	
	return; 
}

void print(){
	//int sum=0;
	for(int i=0;i<(int)generatedFrequentPatterns.size();i++){
		cout<<generatedFrequentPatterns[i].p<<" "<<endl;
		/*cout<<"occurrences : "<<endl;
		for(auto it=generatedFrequentPatterns[i].S.begin();it !=generatedFrequentPatterns[i].S.end();it++){
			cout<<*it<<" ";
		}
		cout<<endl<<endl;*/
	}
	cout<<"total patterns = "<<generatedFrequentPatterns.size()<<endl;
	return;
}


int main(void){
	transactionRead(100);
	print();
}
