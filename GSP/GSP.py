import sys
from math import ceil

class GSP:
    def __init__(self,directory):
        self.database={}
        with open(directory+'/metadata.txt','r') as file:
            lines = file.readlines()
            self.iteration_count = int(lines[1].strip())
            self.percentage_threshold = float(lines[0].strip())
            for i in range(1,self.iteration_count+1):
                input_file_name = directory+'/in'+str(i)+'.txt'
                sys.stdout = open('Output/gsp-out'+str(i)+'.txt','w')
                self.ReadDB(input_file_name)
                minimum_support_threshold = int(ceil((self.percentage_threshold * len(self.database))/100.0))
                self.GeneratePatterns(minimum_support_threshold)
                #print("print patterns")
                #print(self.generated_patterns[1])
                self.PrintPatterns()


    def ProcessSequence(self,line):
        line = line.strip().split(' ')
        sid = int(line[0])
        value = -1
        event_list = []
        processed_sequence = []
        for j in range(1,len(line)):
            value = int(line[j])
            if(value == -1):
                processed_sequence.append(event_list)
                event_list=[]
            else:
                event_list.append(value)
        return sid,processed_sequence

    def ReadDB(self, file_name):
        with open(file_name, 'r') as file:
            lines = file.readlines()
            for i in range(1, len(lines)):
                sid, processed_sequence = self.ProcessSequence(lines[i])
                if(self.database.get(sid) == None):
                    self.database[sid] = processed_sequence
                else:
                    for j in range(0, len(processed_sequence)):
                        self.database[sid].append(processed_sequence[j])

    def SubPatternChecking(self, base_pattern, actual_pattern):
        prev_ending = -1
        found = False
        for i in range(0, len(actual_pattern)):
            prev_ending = prev_ending + 1
            found = False
            while(prev_ending<len(base_pattern)):
                if(len(base_pattern[prev_ending]) < len(actual_pattern[i])):
                    prev_ending = prev_ending + 1
                    continue
                ptr = 0
                for j in range(0, len(base_pattern[prev_ending])):
                    if(base_pattern[prev_ending][j] == actual_pattern[i][ptr]):
                        ptr=ptr+1
                    elif(base_pattern[prev_ending][j] > actual_pattern[i][ptr]):
                        found = False
                        break
                    if(ptr == len(actual_pattern[i])):
                        break
                if(ptr == len(actual_pattern[i])):
                    found = True
                    break
                prev_ending = prev_ending + 1
            if(found == False):
                return 0
        return 1

    def ListCopy(self, list):
        patt = []
        for i in range(0,len(list)):
            temp = []
            for j in range(0,len(list[i])):
                temp.append(list[i][j])
            patt.append(temp)
        return patt

    def TwoPatternMatching(self, pattern1, pattern2):
        sub_patt1 = []
        sub_patt2 = []
        for i in range(0,len(pattern1)):
            temp = []
            st = 0
            if(i==0):
                st = 1
            for j in range(st,len(pattern1[i])):
                temp.append(pattern1[i][j])
            if(len(temp)>0):
                sub_patt1.append(temp)

        for i in range(0,len(pattern2)):
            temp = []
            en = len(pattern2[i])
            if(i==len(pattern2)-1):
                en = en - 1
            for j in range(0,en):
                temp.append(pattern2[i][j])
            if(len(temp) > 0):
                sub_patt2.append(temp)

        if(sub_patt1 != sub_patt2):
            return False
        if(len(pattern2[len(pattern2)-1]) == 1):
            # sequence extension
            join_result = self.ListCopy(pattern1)
            join_result.append(pattern2[len(pattern2)-1].copy())
        else:
            join_result =  self.ListCopy(pattern1)
            join_result[len(join_result)-1].append(pattern2[len(pattern2)-1][len(pattern2[len(pattern2)-1])-1])
        return join_result

    def GenerateSubPatternsAndCheck(self, base_pattern, hash_table):
        for i in range(0,len(base_pattern)):
            for j in range(0,len(base_pattern[i])):
                sub_patt = []
                for k in range(0,len(base_pattern)):
                    temp = []
                    for l in range(0,len(base_pattern[k])):
                        if(k == i and j == l):
                            continue
                        else:
                            temp.append(base_pattern[k][l])
                    if(len(temp) > 0):
                        sub_patt.append(temp)
                if(hash_table.get(str(sub_patt)) == None):
                    return False
        return True

    def Join(self, list_of_patterns):
        candidate = []
        hash_table = {}
        for i in range(0,len(list_of_patterns)):
            hash_table[str(list_of_patterns[i])] =  True
            for j in range(0, len(list_of_patterns)):
                #print("previous list = ",list_of_patterns, list_of_patterns[i],list_of_patterns[j])
                join_result = self.TwoPatternMatching(list_of_patterns[i],list_of_patterns[j])
                #print("now = ",list_of_patterns)
                if(join_result == False):
                    continue
                candidate.append(join_result)
        return candidate, hash_table

    def Prune(self, candidate, hash_table):
        actual_candidates = []
        for i in range(0,len(candidate)):
            base_pattern = candidate[i]
            verdict = self.GenerateSubPatternsAndCheck(base_pattern, hash_table)
            if(verdict == True):
                actual_candidates.append(base_pattern)
        return actual_candidates

    def FindSingleLengthPatterns(self, minimum_support_threshold):
        temp_dict={}
        freq={}
        for i in self.database:
            temp_dict.clear()
            for j in range(0,len(self.database[i])):
                for k in range(0,len(self.database[i][j])):
                    temp_dict[self.database[i][j][k]]=True
            for key in temp_dict:
                if(freq.get(key) == None):
                    freq[key] = 0
                freq[key]=freq[key]+1
        return freq

    def FrequencyCalculationOfAPattern(self, pattern):
        support = 0
        for i in self.database:
            support = support + self.SubPatternChecking(self.database[i],pattern)
        return support

    def PatternOfTwoLength(self, minimum_support_threshold):
        patterns = []
        self.generated_patterns[2]=[]
        for i in range(0,len(self.generated_patterns[1])):
            for j in range(0,len(self.generated_patterns[1])):
                support = self.FrequencyCalculationOfAPattern([self.generated_patterns[1][i][0][0],self.generated_patterns[1][j][0][0]])
                if(support >= minimum_support_threshold):
                    self.generated_patterns[2].append([[self.generated_patterns[1][i][0][0],self.generated_patterns[1][j][0][0]],support])
                if(self.generated_patterns[1][i][0][0][0]<self.generated_patterns[1][j][0][0][0]):
                    support = self.FrequencyCalculationOfAPattern([[self.generated_patterns[1][i][0][0][0],self.generated_patterns[1][j][0][0][0]]])
                    if(support >= minimum_support_threshold):
                        self.generated_patterns[2].append([[[self.generated_patterns[1][i][0][0][0],self.generated_patterns[1][j][0][0][0]]],support])
        return

    def GeneratePatterns(self, minimum_support_threshold):
        # single length pattern
        freq = self.FindSingleLengthPatterns(minimum_support_threshold)
        self.generated_patterns={}
        self.generated_patterns[1]=[]
        for key in freq:
            if(freq[key] >= minimum_support_threshold):
                self.generated_patterns[1].append([[[key]],freq[key]])
        del freq
        # pattern of two length
        self.PatternOfTwoLength(minimum_support_threshold)
        i = 2
        while True:
            if(self.generated_patterns.get(i) == None or len(self.generated_patterns[i]) == 0):
                break
            i = i + 1
            list_of_patterns = []
            for j in range(0,len(self.generated_patterns[i-1])):
                list_of_patterns.append(self.generated_patterns[i-1][j][0])
            candidate, hash_table = self.Join(list_of_patterns)
            #print("join er pore: ",self.generated_patterns[1])
            candidate = self.Prune(candidate, hash_table)
            del hash_table
            for j in range(0,len(candidate)):
                support = self.FrequencyCalculationOfAPattern(candidate[j])
                if(support >= minimum_support_threshold):
                    if(self.generated_patterns.get(i) == None):
                        self.generated_patterns[i]=[]
                    self.generated_patterns[i].append([candidate[j],support])
            if(self.generated_patterns.get(i) == None):
                break
    def PrintPatterns(self):
        for key in self.generated_patterns:
            for i in range(0,len(self.generated_patterns[key])):
                print(self.generated_patterns[key][i][0])
                print(self.generated_patterns[key][i][1])
        return

directory = 'E:\Research\Incremental-Sequential-Pattern-Mining\Incremental-Sequential-Pattern-Mining-with-SP-Tree\Implementation\Dataset\Dataset17'
gsp = GSP(directory)
