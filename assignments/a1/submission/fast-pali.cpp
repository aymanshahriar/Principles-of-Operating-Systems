// CPSC 457 Assignment 1
// Name: Ayman Shahriar
// UCID: 10180260
// Tutorial: T03



#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>

// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces
std::vector<std::string>                         // return type
split( const std::string & p_line)               // function name and inputs. I do not think there is any need to use a reference,
{                                                // just use a string parameter.
  auto line = p_line + " ";
  std::vector<std::string> res;
  bool in_str = false;
  std::string curr_word = "";
  for( auto c : line) {
    if( isspace(c)) {
      if( in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// global variable used in stdin_readline() and read_one_character_fuper_fast()
char buffer[1024*1024];
int buffSize = 0;
int buffIndex = 0;

char read_one_character_super_fast() {
  if (buffSize == 0) {
    buffSize = read(0, buffer, (1024*1024));
    buffIndex = 0;

    if (buffSize == 0) {
      return -1;
    }
  }
  buffSize--;
  return buffer[buffIndex++];
  /**
   * Same as:
   * buffIndex++
   * return buffer[buffIndex-1] 
   */

  

}

// reads in a line from STDIN
// reads until \n or EOF and result includes \n if present
// returns empty string on EOF
std::string
stdin_readline()
{
  std::string result;
  while(1) {
    char c = read_one_character_super_fast();
    if (c == -1) break;
    result.push_back(c);
    if (c == '\n') break;
  }
  return result;
}

// returns true if a word is palindrome
// palindrome is a string that reads the same forward and backward
//    after converting all characters to lower case
bool
is_palindrome( const std::string & s)
{
  for( size_t i = 0 ; i < s.size() / 2 ; i ++)
    if( tolower(s[i]) != tolower(s[s.size()-i-1]))
      return false;
  return true;
}

// returns the longest palindrome on standard input
// in case of ties for length, returns the first palindrome
// all input is broken into words, and each word is checked
// word is a sequence of characters separated by white space
// white space is whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
std::string
get_longest_palindrome()
{
  std::string max_pali;
  while(1) {
    std::string line = stdin_readline();
    if( line.size() == 0) break;
    auto words = split( line);
    for( auto word : words) {
      if( word.size() <= max_pali.size()) continue;
      if( is_palindrome(word))
        max_pali = word;
    }
  }
  return max_pali;
}

int
main()
{
  std::string max_pali = get_longest_palindrome();
  printf("Longest palindrome: %s\n", max_pali.c_str());
  return 0;
}

