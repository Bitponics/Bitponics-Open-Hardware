//********************************************************************************
//********************************************************************************
/** Send initial JSON to /setup
    Requires Networks = wifi.getScanNew(data, sizeof(data), true); 
    where true returns Data formated as json array */
void sendInitialJSON(){
  wifi.println(F("HTTP/1.1 200 OK"));
  wifi.println(F("Content-Type: application/json"));
  wifi.println(F("Transfer-Encoding: chunked"));
  wifi.println(F("Access-Control-Allow-Origin: *"));
  wifi.println(F("Cache-Control: no-cache"));
  wifi.println(F("Connection: close"));
  wifi.println();
  
  wifi.sendChunk(F("{ \"mac\": \""));
  wifi.sendChunk(MAC);
  wifi.sendChunkln(F("\", \"networks\": "));
  wifi.sendChunkln(networks.c_str());
  wifi.sendChunkln(F("}"));
  wifi.sendChunkln();
  
}


////********************************************************************************
///** Send a confirmation of data recieved */
//void sendConfirm(char *ssid, char *pass, char *mode, char *skey, char *pkey){
//    
//    wifi.println(F("HTTP/1.1 200 OK"));
//    wifi.println(F("Content-Type: application/json"));
//    wifi.println(F("Transfer-Encoding: chunked"));
//    wifi.println(F("Access-Control-Allow-Origin: *"));
//    wifi.println(F("Cache-Control: no-cache"));
//    wifi.println(F("Connection: close"));
//    wifi.println();
//    //formatted as JSON
//    wifi.sendChunk(F("{ \"ssid\": \""));
//    wifi.sendChunk(ssid);
//    wifi.sendChunk(F("\", \"pass\": \""));
//    wifi.sendChunk(pass);
//    wifi.sendChunk(F("\", \"mode\": \""));
//    wifi.sendChunk(mode);
//    wifi.sendChunk(F("\", \"skey\": \""));
//    wifi.sendChunk(skey);
//    wifi.sendChunk(F("\", \"pkey\": \""));
//    wifi.sendChunk(pkey);
//    wifi.sendChunkln(F("\" }"));
//    wifi.sendChunkln();
//    
//}
