
// This demo does web requests to a fixed IP address, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php



#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

char techincSite[] PROGMEM = "www.techinc.nl";

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,3,203 }; // nanode ip
// gateway ip address
static byte gwip[] = { 192,168,3,1 }; // gateway (router) ip
// remote website ip address and port
static byte hisip[] = { 192,168,3,1 }; // ip of the site you request

// ~guidocalvano/
static BufferFiller bfill;  // used as cursor while filling the buffer
byte Ethernet::buffer[500];   // a very small tcp/ip buffer is enough here

int awayLedPin = 6 ;
int homeLedPin = 3 ;
int buttonPin = 2 ;

// called when the client request is complete

static void my_callback (byte status, word off, word len) {
  
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setupEtherCard()
    {
     if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
         Serial.println( "Failed to access Ethernet controller");

     ether.staticSetup(myip, gwip);

     ether.copyIp(ether.hisip, hisip);
     ether.printIp("Server: ", ether.hisip);

     while (ether.clientWaitingGw())
        ether.packetLoop(ether.packetReceive());
    
     Serial.println("Gateway found");   

  if (!ether.dnsLookup(techincSite))
    Serial.println("DNS failed");     
    
         Serial.println("DNS lookup win");   
    }


int isDown = false ;
int someoneHome = false ;

void setHome()
  {
    
   // Serial.println( "setHome()" ) ;
   digitalWrite( homeLedPin, HIGH ) ;
   digitalWrite( awayLedPin, LOW  ) ;
             
   ether.browseUrl(PSTR("/space/index.php?state=open&key=KEYHERE"), "", techincSite, my_callback ) ;      
  } 


void setAway()
  {
  //  Serial.println( "setAway()" ) ;
   
   digitalWrite( homeLedPin, LOW  ) ;
   digitalWrite( awayLedPin, HIGH ) ;
    // /space/index.php?state=closed&key=blablabla          
   ether.browseUrl(PSTR("/space/index.php?state=closed&key=KEYHERE"), "", techincSite, my_callback ) ;          
  }


void setup () {
  Serial.begin(57600);
  Serial.println("\nSTART OF EXECUTION");

  
  pinMode( buttonPin,  INPUT  ) ;
    digitalWrite( buttonPin,  HIGH  ) ;
  pinMode( homeLedPin, OUTPUT ) ;
  pinMode( awayLedPin,    OUTPUT ) ;
  
  
  
   setupEtherCard() ;
  ether.packetLoop(ether.packetReceive()); 
  
 /* call these to set the state of the space 
  setHome() ;
  */
  Serial.println( "end setup" ) ;
}

long t = 0 ;

void loop () 
  {
  
    
   ether.packetLoop(ether.packetReceive());
   int nextIsDown = digitalRead( buttonPin ) ;

   // debuggin code that prints the button state
  // if( t + 1000 < millis() )
     {
      Serial.println( "buttonPin=" ) ; Serial.println( nextIsDown ) ;
     
      t = millis() ; 
     }
   
  
   
   if(  isDown && !nextIsDown ) 
     {
      someoneHome = !someoneHome ;
      // activate this to actually set the state of the space and inform the server
      if( someoneHome )
            setHome() ;
            
      else
            setAway() ;    
      
     } ;
     
    isDown = nextIsDown ;
   // processUpdate() ;
  
   
  }
