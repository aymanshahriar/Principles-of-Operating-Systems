// CPSC 457 Assignment 1
// Name: Ayman Shahriar
// UCID: 10180260
// Tutorial: T03

#include <iostream>
#include <vector>
#include <string>

// take an input string, return an array of words in that string that are seperated by whitespace
std::vector<std::string> split(std::string str) {
    str += " ";                               // makes sure the last word will be included in list
    std::vector<std::string> returnLst;
    std::string word = "";
    for (auto c : str) {     // the type for c should be a char
        if (isspace(c)) {
            if (word != "") {
                returnLst.push_back(word);
                word = "";
            }
        }
        else {
            word += c;
        }
    }
    
    return returnLst;
}


// returns true if a word is palindrome
// palindrome is a string that reads the same forward and backward
//    after converting all characters to lower case
///// if the program turns out to be slower, the use the is_Palindrome function that he provided
bool is_palindrome(std::string str) {
    while (str.size() >= 1) {
        if (tolower(str.front()) != tolower(str.back())) {
            return false;
        }
        str.pop_back();   // remove last character
        str.erase(0, 1);   // remove first character
    }
    return true;
}

/*
bool
is_palindrome( const std::string & s)
{ std::cout << s << "\n";
  for( size_t i = 0 ; i < s.size() / 2 ; i ++)
    if( tolower(s[i]) != tolower(s[s.size()-i-1]))
      return false;
  return true;
}
*/


int main() {
    /*
    // test for split
    std::string str = "apple\t banana\v \n pear\f monkey\r  peach raccoon";
    std::vector<std::string> lst = split(str);
    std::cout << lst.size() << std::endl;
    for (auto word : lst) {
        std::cout << word << std::endl;
    }
    */

    
    // test for is_palindrome
    std::string str;
    while (true) {
        std::cin >> str;
        if (is_palindrome(str) == true) std::cout << "is palindrome" << std::endl;
        else std::cout << "is not palindrome" << std::endl;
    }
     
    return 0;
}
