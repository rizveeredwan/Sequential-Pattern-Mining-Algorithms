#include <bits/stdc++.h>
using namespace std;

#define MAX_SYMBOL 30
#define BASE 'a' 



struct Pattern{
	string p; //pattern 
	set<int>S; //occurrence vector set 
};

vector<Pattern>generatedFrequentPatterns;

vector<string>transactions; //to save the information about transactions 
int maxEvent;
int numberOfBit;

map<string,vector<long long int>>bitMapRepresentation; 

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


vector<long long int> sStep(string left, string right){
	//debug: cout<<"left = "<<left<<" "<<right<<endl;
	vector<long long int>vLeft=bitMapRepresentation[left];
	vector<long long int>vRight=bitMapRepresentation[right];
	vector<long long int>result;
	long long int base;
	int savePos;
	long long int newLeft;
	for(int i=0;i<(int)vLeft.size();i++){
		if(vLeft[i]>0){
			base=vLeft[i];
			savePos = -1;
			for(int j=0;j<numberOfBit;j++){
				base = base >> 1;
				if(base==0) {
					savePos=j;
					break;
				}
			}
			newLeft = (1<<(savePos))-1;
			//debug: cout<<"vLeft = "<<vLeft[i]<<" newLeft "<<newLeft<<" savePos "<<savePos<<endl;
			//debug: cout<<"vright = "<<vRight[i]<<endl;
			result.push_back(newLeft & vRight[i]);
		}
		else{
			result.push_back(0);
		}
	}
	return result;
}

vector<long long int>iStep(string left, string right){
	vector<long long int>vLeft=bitMapRepresentation[left];
	vector<long long int>vRight=bitMapRepresentation[right];
	vector<long long int>result;
	for(int i=0;i<(int)vLeft.size();i++){
		result.push_back(vLeft[i] & vRight[i]);
	}
	return result;
}

void lexicographicTree(string pattern,vector<string>validMoves,int minSupCount){
	string s="";
	vector<long long int> sStepRes,iStepRes;
	set<int>S;
	for(int i=0;i<(int)validMoves.size();i++){
		//s-step
		S.clear();
		s=pattern+"{"+validMoves[i]+"}";
		
		sStepRes = sStep(pattern,"{"+validMoves[i]+"}");
		for(int j=0;j<(int)sStepRes.size();j++){
			if(sStepRes[j]>0){
				S.insert(j);
			}
		}

		
		if((int)S.size() >= minSupCount){
			generatedFrequentPatterns.push_back({s,S});
			bitMapRepresentation[s]=sStepRes;
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
				if(iStepRes[j]>0){
					S.insert(j);
				}
			}
			if((int)S.size() >= minSupCount){
				generatedFrequentPatterns.push_back({s,S});
				bitMapRepresentation[s]=iStepRes;
				lexicographicTree(s,validMoves,minSupCount);
			}
		}
	}
}

void transactionRead(int minSupCount){
	//function to read input from file 
	freopen("../Dataset/in2.txt","r",stdin);
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
	numberOfBit = min(64,cnt); //max possible itemsets in a sequence will not cross 64 [given condition]
	int power_2[]={0,1,2,4,8,16,32,64};
	for(int i=1;i<=7;i++){
		if(numberOfBit>power_2[i-1] && numberOfBit<=power_2[i]){
			numberOfBit=power_2[i];
			break;
		}
	}
	
	vector<long long int>bitInit; 
	for(int i=0;i<(int)transactions.size();i++){
		bitInit.push_back(0);
	}
	
	for(int i=0;i<MAX_SYMBOL;i++){
		if(arr[i] == 1){
			s=(char)(i+BASE);
			bitMapRepresentation["{"+s+"}"]=bitInit;
		}
	}
	
	for(int i=0;i<(int)transactions.size();i++){
		temp=split(transactions[i]);
		for(int j=0;j<(int)temp.size();j++){
			for(int k=0;k<(int)temp[j].size();k++){
				s=temp[j][k];
				bitMapRepresentation["{"+s+"}"][i] |= (1<<(numberOfBit-j-1));
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
		for(int j=0;j<(int)bitMapRepresentation["{"+s+"}"].size();j++){
			if(bitMapRepresentation["{"+s+"}"][j]>0){
				cnt++;
				S.insert(j);
			}
			
		}
		
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
		cout<<generatedFrequentPatterns[i].p<<" ";
		for(auto it=generatedFrequentPatterns[i].S.begin();it !=generatedFrequentPatterns[i].S.end();it++){
			cout<<*it<<" ";
		}
		cout<<endl;
	}
	cout<<"total patterns = "<<generatedFrequentPatterns.size()<<endl;
	return;
}


int main(void){
	transactionRead(2);
	print();
}
