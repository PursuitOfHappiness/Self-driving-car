/**
     * Based off OpenDaVINCI's NetstringsProtocol
     * OpenDaVINCI/libopendavinci/include/opendavinci/odcore/io/protocol/NetstringsProtocol.h
     * https://github.com/se-research/OpenDaVINCI/blob/4644da781434482a98f58bd5f5a6dd1d052389fe/libopendavinci/include/opendavinci/odcore/io/protocol/NetstringsProtocol.h
 */
#include "CustomNetstringsProtocol.h"
#include "opendavinci/odcore/base/Lock.h"

using namespace odcore::base;

CustomNetstringsProtocol::CustomNetstringsProtocol() :
    AbstractProtocol(),
    m_stringListenerMutex(),
    m_stringListener(NULL),
    m_partialDataMutex(),
    m_partialData() {}

CustomNetstringsProtocol::~CustomNetstringsProtocol() {
    setStringListener(NULL);
}

void CustomNetstringsProtocol::setStringListener(StringListener* listener) {
    Lock l(m_stringListenerMutex);
    m_stringListener = listener;
}

void CustomNetstringsProtocol::send(const string& data) {
    if (data.length() > 0) {
        stringstream netstring;

        netstring << static_cast<uint32_t>(data.length()) << ":" << data << "," << '\n';

        sendByStringSender(netstring.str());
    }
}

void CustomNetstringsProtocol::nextString(const string &s) {
    
    int keepDecoding = 1;

    Lock l(m_partialDataMutex);
    m_partialData << s;

    while(keepDecoding){

        int result = decodeNetstring(m_partialData.str());

        if(m_partialData.str().length() < 1){
            keepDecoding = 0;
        } else if(result == 0){
            keepDecoding = 0;
        } else if (result > 0){
            string remaining = m_partialData.str();
            remaining = remaining.substr(result,string::npos);
            m_partialData.str(remaining);
            keepDecoding = 1;
        } else {
            string remaining = m_partialData.str().substr(result * -1 ,string::npos);
            m_partialData.str(remaining);
            keepDecoding = 1;
        }
    }
}

/**
@ return: 0 needs more chars, negative means error indicating how many chars to remove, positive means success indicating how many chars were parsed (also with purpose of removal)
*/
int CustomNetstringsProtocol::decodeNetstring(const string &rawData){
    // DO NOT MODIFY NEXT LINE:
    // CREDIT TO PETER SCOTT @ https://github.com/PeterScott/netstring-c/

    int NETSTRING_ERROR_TOO_LONG    = -9;      //More than 999999999 bytes in a field            // Discard 9 chars
    int NETSTRING_ERROR_LEADING_ZERO= -1;      //Leading zeros are not allowed                   // DISCARD 1 char
    int NETSTRING_ERROR_NO_LENGTH   = -1;      //Length not given at start of netstring          // DISCARD 1 char

    //if the Netstring is less than 3 characters, it's either an invalid one or contains an empty string
    if (rawData.length() < 2) return -(rawData.length() - 1);
    
    // NO LEADING 0
    if (rawData[0] == '0' && isdigit(rawData[1])) return NETSTRING_ERROR_LEADING_ZERO;

    /* The netstring must start with a number */
    if (!isdigit(rawData[0])) return NETSTRING_ERROR_NO_LENGTH;
    
    // How many chars represent digits, compute expected payload length
    unsigned int expectedLength = 0;
    unsigned int i = 0;

    for (; i < rawData.length() && isdigit(rawData[i]); i++) {
        /* Error if more than 9 digits */
        if (i >= 9) return NETSTRING_ERROR_TOO_LONG;
        /* Accumulate each digit, assuming ASCII. */
        expectedLength = expectedLength*10 + (rawData[i] - '0');
    }
    // If we only got digits and EOF char
    if(i + 1 == rawData.length()) return 0;

    /* Read the colon */
    if (rawData[i] != ':') return -(i);                 

    /* Check buffer length once and for all. To check if the buffer is longer than the digit chars and the colon and the comma. */
    if ((unsigned int)(i++ + expectedLength + 1) >= rawData.length()) return 0; // WAIT FOR MORE

    /* Test for the trailing comma, and set the return values */
    if (rawData[i + expectedLength] != ',') return -(i+expectedLength); // DISCARD ALL TILL EXPECTED COMA POS INCLUSIVE

    // TODO invoke stringlistener with payload
    invokeStringListener(rawData.substr(i,expectedLength));

    return (expectedLength + i + 1);
}

// Call nextString on the listener we've set up using setStringListener
void CustomNetstringsProtocol::invokeStringListener(const string& data) {
    Lock l(m_stringListenerMutex);
    if (m_stringListener != NULL) {
        m_stringListener->nextString(data);
    }
}
