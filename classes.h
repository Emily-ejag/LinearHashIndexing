#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <math.h>
#include <bitset>
#include <algorithm>
#include <tuple>

using namespace std;

const int PAGE_SIZE = 4096;
const float FILL_FACT = 0.70;//fill till 70%
int* p = NULL;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class LinearHashIndex {

private:

	vector<int> pageDirectory;
	int overFlow = 0;
	int array[2];
//	int Thisline = 0; // current line in the file
	string fName;
 // indexfile
    fstream files;
	int watch=0;
    //Do the hashing
    void hash(Record record, bool isRearrange){
    	bool bitflip = false;
		string line,word;
    	files.open(fName);
    	int cont;
    	int iBlock;//hash number with or without bitflip
		int posBlock, posWrite; //posotion of the block, position of the (block +size of line)
		bool isBitFlip = false;
		bool ban=true;
    	int hashNumber = record.id % 16; //hash index
    	int pos; // to get the position were the record will be storage
    	bitset<16> number(hashNumber); //transform it to bits
    	if(!isRearrange){
	    	if (!(std::find(pageDirectory.begin(), pageDirectory.end(), hashNumber) != pageDirectory.end())) {
	        	number.flip(1); //if the index is not in the hash, change the 2nd bit
	        	int mybit_int = (int)(number.to_ulong()); //transform that bit to number
	        	if (!(std::find(pageDirectory.begin(), pageDirectory.end(), mybit_int) != pageDirectory.end())) {
	        		pageDirectory.push_back(hashNumber);//check if this new number is in the intex, if it is not, create new index
	        		iBlock = hashNumber; //get the position for that new index
				}
				else{
					iBlock = mybit_int; // if when the flipping happends it found it, then the position will be hte one with the bit flip
					isBitFlip = true;
				}
		    }else{
		    	iBlock= hashNumber; // if it found it the position will be in the hash number
			}
		}else{// if isRearrange == true. (Redoing hashing, don't even consider overflip)
			iBlock= hashNumber;
			cout<<"DoingBFcase / rearrange \n";
		}
		string rec = std::to_string(record.id) + "," + record.name + "," +record.bio + "," +std::to_string(record.manager_id) + "$#";
		while(ban){
		    //files.seekg(pos*PAGE_SIZE);
		    posBlock = (iBlock+cont*16)*PAGE_SIZE;
			 files.seekg(posBlock);
		    files.tellg();
		    if(files.peek() == std::ifstream::traits_type::eof() ||files.peek() == '\0'){//if the line is empty
		    	files.seekg(posBlock);//look at the position
		    	posWrite=posBlock;
		    	//cout<<"entro aqui cuando no hay nada en la linea\n";
				ban=false;		    	
			}else{//if it's not empty
				if(getline(files, line,'#')){//get the line at that position of the block
			    	stringstream s(line);
			    	int found = line.find_first_not_of('\0');//cutting the nulls
			    	string newLine = line.substr(found,line.size());//trim the line without the nulls
			    	files.seekg(posBlock+newLine.size());//get the position were the new record would be storage
			    	files.tellg();//positioned in that position
			    	if((newLine.size()+rec.size()) <= FILL_FACT*PAGE_SIZE){//checking if the new record can fit in the block
						//files <<rec;
						posWrite=posBlock+newLine.size();
					//	cout<<"position: "<<files.tellg()<<"record: "<<rec<<endl;
						ban=false;
					}
					else{
					// may vary if there are 2 or more blocks per index
					if (line.size() > PAGE_SIZE|| line.size()==0){ //, if size from beggining to end charachter is greater than PAGE_SIZE, then the block was empty  
						//if the page is still not empty
						posWrite = posBlock;
						//cout<<"page was empty: "<<cont<<endl;
						ban=false;
					} 
					else{
						// if it was a real overflow (not from bitflip)
						if (isBitFlip == false){
							// do overflow. Repeat process for iBlock+cont*16		
							cout << "overflow (no bitflip). try next page. Curr page count: "<<cont << endl;
							cont++;	
							//cout<< line<<endl;
							posWrite = -1;
						}
						// if it was an overflow due to bitflip
						else{
							bitset<16> change(iBlock);
							change.flip(1);
							int changy = (int)(change.to_ulong());
							pageDirectory.push_back(changy);//now the numbers have to be put on the index
							cout << cont << "*overflow due to bitflip*. First do new hash. Then send to rearrange in a loop. "<<endl;
							// sent it to do hash again but with out option to due bitflip.
							files.close(); 
							hash(record, true);
							posWrite = -1; // do not write anything extra for this run. It was written.
							  
							
							cout << "\n\n \t\t*****Start overflow with bitflip. -> rearrange \n";	
							// take line records from all the pages for the given hash
							for (int sheet = 0; sheet <= cont; sheet++) {
								// get line for the sheet
								files.open(fName, ios::out|ios::in);
								posBlock = (iBlock+sheet*16)*PAGE_SIZE; // may vary if there are 2 or more blocks per index. FIX LATER
								files.seekp(posBlock);
								getline(files, line,'#');
								
								//cout<< line<<endl;
								// clean the line
								files.seekp(posBlock);
								for (int xc = 0; xc <=line.size(); xc++) {files.put('\0');}
								//cout << "page cleaned" <<endl;
								
								stringstream line_str(line);

								// split line into records
								string subline;
								vector<std::string> data;
								files.close(); 
								//cout << "curr line_str"<< line << endl;
								while (getline(line_str, subline, '$')) {
									vector<std::string> data;
									//cout << "subline: "<< subline <<endl;
									// turn line into a stream
									stringstream s(subline);
									// gets everything in stream up to comma
									getline(s, word,',');
									data.push_back(word);
									getline(s, word, ',');
									data.push_back(word);
									getline(s, word, ',');
									data.push_back(word);
									getline(s, word, ',');
									data.push_back(word);
									//create new record 
									Record emp(data);
									//put record in the hash table
									cout << "reorder: "<<emp.id<<endl;
									hash(emp, true);
									//emp.print();
								}
							}		
							cout << "\n\n \t\t*****End overflow with bitflip. -> rearrange \n";			
							ban=false;
						}
					}					
					// the are things written already			
				}
			}
		}	
		if (posWrite != -1){
			files.seekg(posWrite);
			files.tellg();
			cout<< "was wrote: "<<record.id<<" "<<record.name<< "in position: "<< posWrite<<endl;
			files << rec;
			files.close();
		}
		
	}
}
    
bool getRecord(int key, int nlockForLook){
		fstream files2;//EmployeeIndex
		string nLine,li,wo,wo2,eidlook;
		bool ban=true;
		int posOfPage;
		int count2=0;
		vector<std::string> data2;
		int pos=0;
		while(ban){
			posOfPage = (nlockForLook+count2*16)*PAGE_SIZE; ;
			files2.open(fName, ios::out|ios::in);
	    	files2.seekg(posOfPage);
		    files2.tellg();
			if(getline(files2, li,'#')){//get the line at that position of the block
		    	int found2 = li.find_first_not_of('\0');//cutting the nulls
		    	string nLine = li.substr(found2,li.size());
		    	stringstream wo(nLine);
		    	while (getline(wo, wo2, '$')) {
					// turn line into a stream
					stringstream s(wo2);
					// gets everything in stream up to comma
					getline(s, eidlook,',');
					if(stoi(eidlook) == key){
						data2.push_back(eidlook);
						getline(s, eidlook,',');
						data2.push_back(eidlook);
						getline(s, eidlook,',');
						data2.push_back(eidlook);
						getline(s, eidlook,',');
						data2.push_back(eidlook);
						Record emp2(data2);
						cout << "The employee is: "<<endl;
						emp2.print();
						return true;
					}
				}
		    }else{
		    	files2.close();
		    	return false;
			}
		    count2++;
		    files2.close();
		}
		return false;
	}
 
    //searching for key
    bool doesExist(int key){
    	int hashForlook = key % 16;
    	int nlockForLook;
    	bitset<16> num(hashForlook);
		if (!(std::find(pageDirectory.begin(), pageDirectory.end(), hashForlook) != pageDirectory.end())) {
        	num.flip(1);
        	int bitflipy = (int)(num.to_ulong());
        	if (!(std::find(pageDirectory.begin(), pageDirectory.end(), bitflipy) != pageDirectory.end())) {
        		//cout <<"no hay ni id ni flip"<<endl;
        		return false;
			}else{
				nlockForLook = bitflipy;
			}
		}else{
			nlockForLook = hashForlook;
		}
		return getRecord(key,nlockForLook);
	}
    
public:
    LinearHashIndex(string indexFileName) {
    	//NEXT FREE PAGE TO GO TO THE NEXT AVAILABLE POSITION IN MEMORY IF NEEDED
    	//TWO BITS FOR THE PAGE NUMBER --- BUCKET AND UPDATE THIS AS NEEDED
        fName = indexFileName+".txt"; //indexfile
    }
    
    // Read csv file and add records to the index
    void createFromFile(string csvFName) {

    	ifstream file; 
		string line,word;
		file.open(csvFName);

		while(!file.eof()){
			//Record emp;
			vector<std::string> data;
			if (getline(file, line, '\n')) {
		        // turn line into a stream
		        stringstream s(line);
		        // gets everything in stream up to comma
		        getline(s, word,',');
		        data.push_back(word);
		        getline(s, word, ',');
		        data.push_back(word);
		        getline(s, word, ',');
		        data.push_back(word);
		        getline(s, word, ',');
		        data.push_back(word);
				//create record 
				Record emp(data);
				//put record in the hash table
				hash(emp, false);
		        //emp.print();
			}
		}
		cout<< "the directory indexes are: \n";
		for(int i=0; i < pageDirectory.size(); i++)
   			std::cout << pageDirectory.at(i) << ' ';
//   		cout<<"overflow: " << overFlow;
   		cout<<"\n\n";
		file.close();
    }
     
    // Given an ID, find the relevant record and print it
    void findRecordById(int emid) {
		bool lookup = doesExist(emid);
		if (!lookup){
			cout << "there is not employee with that id" << endl << endl;
		}
    }
};
