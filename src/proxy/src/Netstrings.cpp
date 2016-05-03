/*
 * The Pursuit of Happiness 2016-05-03
 */

#include "Netstrings.h"

namespace automotive {
    namespace miniature {

        using namespace std;

        string Netstrings::decodeNetstring(string toDecode){
          if (toDecode.length() < 3){ // A netstring can't be shorter than 3 characters
            return "Netstrings: Wrong format";
          }

          // Check that : and , exists at the proper places
          size_t semicolonIndex = toDecode.find(':');
          //cout << "semicolonIndex " << semicolonIndex << endl;
          size_t commaIndex = toDecode.find(',');
          //cout << "commaIndex " << commaIndex << endl;
          if (semicolonIndex == std::string::npos || commaIndex == std::string::npos) {
            return "Netstrings: No semicolon found, or no comma found";
          }

          // Parse control number
          // If the number is anywhere else than in the beginning we are not interested
          string number = toDecode.substr(0, semicolonIndex);
          //cout << "number: " << toDecode << endl;
          unsigned int controlNumber;
          try {
            controlNumber = stoi(number);
          }
          catch (std::invalid_argument&){
            return "STOI: Invalid Arguments.";
          }
          catch (std::out_of_range&){
            return "STOI: Out of range.";
          }

          if (controlNumber < 1){ // the netstring has to contain atleast 1 character to be parsed
            return "Netstrings: Control Number is to low";
          }

          // Check that the length of the string is correct
          toDecode = toDecode.substr(semicolonIndex + 1, controlNumber); // the data that we want to parse
          //cout << "Stripped String: " << toDecode << endl;
          //cout << "Length: " << toDecode.length() << endl;
          if (controlNumber != toDecode.length()){
            return "Netstrings: Wrong length of data";
          }
          return toDecode;
        }

        string Netstrings::encodeNetstring(string toEncode){
          if (toEncode.length() < 1){
            return "Netstrings: Nothing to encode";
          }
          return to_string(toEncode.length()) + ":" + toEncode + ",";
        }

    }
}
