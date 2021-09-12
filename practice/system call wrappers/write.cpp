/* This program will write to a file using ofstream and will read a file using ifstream*/

#include <iostream>
#include <fstream>  // stands for file stream

using namespace std;

int main(int argc, const char* argv[]) {
	
	// obtain file name from command line argument
	string filename = argv[1];
	
	// open the file in write mode and write to it
	ofstream myfile;
	myfile.open(filename);
	myfile << "I have written to a file\nI have written to a file again";
	myfile.close();
	
	
	// open the file in read mode and read from it
	ifstream myfile1;
	myfile1.open(filename);
	
	string contents;

	while (getline(myfile1, contents)) {
		cout << contents << endl;
	}





	return 0;
}
