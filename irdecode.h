/*
	irdecode.h

	A couple of helper functions to be used with the IRremote library.

*/

#include <Arduino.h>
#include <IRremote.h>

String decodeType(int value)
{
	String type = "UNKNOWN";
	switch (value){
        // --- most likely ---------------
        case NEC: 
          type = "NEC"; 
          break;
        case SONY: 
          type = "SONY"; 
          break;
        case RC5: 
          type = "RC5"; 
          break;
        case RC6: 
          type = "RC6"; 
          break;
        // --- others --------------------
        case AIWA_RC_T501: 
          type = "AIWA_RC_T501"; 
          break;
        case DENON: 
          type = "DENON"; 
          break;
        case DISH: 
          type = "DISH"; 
          break;
        case JVC: 
          type = "JVC"; 
          break;
        case LG: 
          type = "LG"; 
          break;
        case MITSUBISHI: 
          type = "MITSUBISHI"; 
          break;
        case PANASONIC: 
          type = "PANASONIC"; 
          break;
        case SAMSUNG: 
          type = "SAMSUNG"; 
          break;
        case SANYO: 
          type = "SANYO"; 
          break;
        case SHARP: 
          type = "SHARP"; 
          break;
        case WHYNTER: 
          type = "WHYNTER"; 
          break;
        case UNKNOWN:
          type = "UNKNOWN"; 
          break;
        default:
          type = "No idea"; 
          break;
      } // switch
	return type;
}

void decodeDetails(char *buf, decode_results results)
{
  if(results.value != 0xFFFFFFFF) {
  	String type = decodeType(results.decode_type);
	  sprintf(buf, "0x%10X %10X %3d %s", results.value, results.value, results.bits, type);
  }
}
