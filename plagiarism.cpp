#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

class subSentence{
public:
    string own;
    string backup;
    vector<string>words;
    double similarityRate;
    int wordCount;
    subSentence(){
        own="";
        similarityRate=0.0;
        wordCount=0;
    }
};

class document{
public:
    vector<subSentence>sentences;
    double wholeSimilarityRate;
    document(ifstream &file){
        wholeSimilarityRate=0.0;
        string sentence;
        subSentence temp;
        string tempSentence;
        while(getline(file,sentence)){
            if(tempSentence.size()!=0){
                tempSentence+=" ";
            }
            for(unsigned int i=0;i<sentence.size();i++){
                if(sentence[i]>64 && sentence[i]<91){//ascii value interval of capital letters.
                    sentence[i]+=32;
                }
                else if(!(sentence[i]==46 || (sentence[i]<123 && sentence[i]>96))){//ascii value interval of special character.
                    sentence[i]=32;//all special character swap with space character.
                }

                if(sentence[i]!='.'){//dividing to sentence part
                    tempSentence+=sentence[i];
                }
                else{
                    tempSentence+='.';
                    temp.own=tempSentence;
                    sentences.push_back(temp);
                    tempSentence="";
                }
            }
        }
    }
    void backup(ifstream &file){
            string tempSentence;
            string sentence;
            subSentence temp;
            int k=0;
            while(getline(file,sentence)){
            for(unsigned int i=0;i<sentence.size();i++){
            if(sentence[i]!='.'){//dividing to sentences part
                    tempSentence+=sentence[i];
                }
                else{
                    tempSentence+='.';
                    sentences[k].backup=tempSentence;
                    tempSentence="";
                    k++;
                }
            }
            }
    }
    void wordCount(){
        int number=0;
        for(unsigned int i=0;i < this->sentences.size();i++){
            number=1;
            for(unsigned int j=1;j < this->sentences[i].own.size();j++){
                if(this->sentences[i].own[j] == 32 && this->sentences[i].own[j-1] != 32){//32 is ascii value of space character
                    number++;
                }
            }
            this->sentences[i].wordCount=number;
            number=0;
        }
    }
    void findKeyWords(){
        string keyWord="";
        for(unsigned int i=0;i<this->sentences.size();i++){
            for(unsigned int j=0;j<this->sentences[i].own.size();j++){
                if(this->sentences[i].own[j]!=32 && this->sentences[i].own[j]!=46){//46 is ascii value of point
                    keyWord+=this->sentences[i].own[j];
                }
                else{
                    if(keyWord.size()>2){
                        bool found=false;
                        for(unsigned int k=0;k<this->sentences[i].words.size();k++){// checking word is distinct or not
                            if(sentences[i].words[k]==keyWord){
                                found=true;
                                break;
                            }
                        }
                        if(!found){
                            this->sentences[i].words.push_back(keyWord);
                        }
                    }
                    keyWord="";
                }
            }
        }
    }
    void quickSort(int l,int u){
        int j;
        if(l<u){
            j=partialSorting(l,u);
            quickSort(l,j-1);
            quickSort(j+1,u);

        }
    }
private:
    int partialSorting(int l,int u){
        int i,j;
        double v;
        v=sentences[l].similarityRate;
        i=l;
        j=u+1;
        do{
            do
                i++;
            while(sentences[i].similarityRate>v && i<=u);
            do
                j--;
            while(v>sentences[j].similarityRate);
            if(i<j){
                swap(i,j);
            }

        }while(i<j);
        swap(l,j);

        return j;
    }
    void swap(int first,int second){
        subSentence temp=sentences[first];
        sentences[first]=sentences[second];
        sentences[second]=temp;
    }
};
bool horspool(string &sentence,string &word,int badMatchTable[]){
    int m=sentence.size();
    int n=word.size();
    if(n>=m){
        return false;
    }
    int i=0;
    while(i<=m-n){
        for(int j=n-1;j>=0;j--){
            if(word[j]!=sentence[i+j]){
                break;

            }
            if(j==0){
                return true;
            }
        }
        i=i+n-1;
        i=i+badMatchTable[(int)sentence[i]];
        i=i-n+1;
    }
    return false;

}

bool naive(string &sentence,string &word){// this naive algorithm for compare with horspool
    if(sentence.size()<word.size()){
        return false;
    }
    for(unsigned int i=0;i<sentence.size()-word.size();i++){
        unsigned int j;
        for(j=0;j<word.size();j++){
            if(sentence[i+j]!=word[j]){
                break;
            }
            if(j==word.size()-1){
                return true;
            }
        }
    }
    return false;
}



void documentComparison(document &main,document &other){
    int counter=0;
    double similarity=0.0;
    double temp=0.0;
    string sentence,word;
    int m=other.sentences.size();
    int n=main.sentences.size();
    int p=0;
    for(int i=0;i<m;i++){//select the sentence from other document
        p=other.sentences[i].words.size();
        int badMatchTable[p][128];
        for(int h=0;h<p;h++){//finding bad match tables for every words.
            for(int a=0;a<128;a++){
                badMatchTable[h][a]=other.sentences[i].words[h].size();
            }
            for(unsigned int a=0;a<other.sentences[i].words[h].size()-1;a++){
                badMatchTable[h][(int)other.sentences[i].words[h][a]]=other.sentences[i].words[h].size()-a-1;
            }
        }


        for(int j=0;j<n;j++){//select the sentence from main document
            for(int k=0;k<p;k++){

                /*if(other.sentences[i].own.length() < 0.8*main.sentences[j].own.length()){
                    break;  //this part can be use for skip the some redundant comparison but it is out of project.
                }*/

                if(horspool(main.sentences[j].own,other.sentences[i].words[k],badMatchTable[k])){
                    counter++;//search part:horspool(sentence,word,badmatchtable)
                }
            }
            temp=(((double(counter)/double(main.sentences[j].wordCount))*100.0)+((double(counter)/double(other.sentences[i].wordCount))*100.0))/2.0;
            if(temp>similarity){//if we find bigger rate change it.
                similarity=temp;
            }
            counter=0;
        }
        other.sentences[i].similarityRate=similarity;
        similarity=0.0;//reset for new sentences
    }
}




int main()
{   clock_t start;//this is for execution time
    double duration;
    start=clock();

    //read and edit main document
    ifstream mainFile("main_doc.txt");
    document main_doc(mainFile);
    mainFile.close();
    main_doc.wordCount();


    int fileNumber=0;
    vector<document>documents;
    string documentName;
    //read and edit other document
    while(true){
        documentName="document"+to_string(fileNumber+1)+".txt";
        ifstream file(documentName);
        if(file.good()){
            document temp(file);
            file.close();
            documents.push_back(temp);
            documents[fileNumber].wordCount();
            documents[fileNumber].findKeyWords();
            fileNumber++;
        }
        else{
            break;
        }
    }

    fileNumber=0;
    while(true){//take backup of sentences for displaying properly.
        documentName="document"+to_string(fileNumber+1)+".txt";
        ifstream file(documentName);
        if(file.good()){
            documents[fileNumber].backup(file);
            file.close();
            fileNumber++;
        }
        else{
            break;
        }
    }

    for(int i=0;i<fileNumber;i++){//document comparion part
        documentComparison(main_doc,documents[i]);
        int totalSimilarSentences=0;
        for(unsigned int j=0;j<documents[i].sentences.size();j++){
            if(documents[i].sentences[j].similarityRate > 50.0){//this number can be changed.
                totalSimilarSentences++;
            }
        }
        documents[i].wholeSimilarityRate=double(totalSimilarSentences)/double(main_doc.sentences.size())*100.0;
        cout<<i+1<<".DOCUMENT"<<endl<<"{"<<documents[i].wholeSimilarityRate<<"}"<<endl<<endl;
        documents[i].quickSort(0,documents[i].sentences.size()-1);

        for(int j=0;j<5;j++){//this part display 5 most similar sentences!!!
            cout<<"{"<<documents[i].sentences[j].backup<<"}---similarityRate = "<<documents[i].sentences[j].similarityRate<<endl<<endl;
        }
    }

    duration=(clock()-start)/(double)CLOCKS_PER_SEC;
    cout<<endl<<"execution time = "<<duration<<" s"<<endl;


    return 0;
}
