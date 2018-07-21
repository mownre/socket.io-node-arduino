#include <ArduinoJson.h>

void setup(){
    Serial.begin(9600);

    while (!Serial){
        continue;
    }
}

void loop(){

    StaticJsonDocument<200> doc;
    JsonObject root = doc.to<JsonObject>();

    root["temperatura"] = random(20, 50);
    root["umidade"] = random(40, 80);
    root["movimentacao"] = random(0, 1);
    root["luminosidade"] = random(0, 1024);

    serializeJson(root, Serial);

    delay(1500);

}