#include "SIM800.h"

bool SIM800::init(String server)
{	SERVER =server;
	echo(false);
	return sim_status() && net_registration(); //&& gprs_status();
}

bool SIM800::restart()
{
	return exec("AT+CFUN=1,1") && find_result("OK");
}

bool SIM800::sim_status()
{
	return exec("AT+CPIN?") && find_result("+CPIN: READY", "OK");
}

bool SIM800::net_registration()
{
	return exec("AT+CREG?") && find_result("+CREG: 0,1", "OK");
}

bool SIM800::gprs_status()
{
	return exec("AT+CGATT?") && find_result("+CGATT: 1", "OK");
}
void SIM800::enable_error_msg(){
	exec("AT+CMEE=2");
}
void SIM800::disable_error_msg(){
	exec("AT+CMEE=0");
}
bool SIM800::apn(const char * apn)
{
	char at[20];
	sprintf(at, "AT+CSTT=\"%s\"", apn);
	return exec(at) && find_result("OK") &&
		exec("AT+CIICR") && find_result("OK") &&
		exec("AT+CIFSR") && find_result(".");
}

bool SIM800::create_tcp(const char *ip, unsigned int port)
{
	char at[100];
	sprintf(at, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"", ip, port);
	return exec(at) && find_result("CONNECT OK", "OK");
}

bool SIM800::tcp_send(const char * send)
{
	int len = strlen(send);
	char at[20];
	sprintf(at, "AT+CIPSEND=%d", len);
	//Serial.println(at);
	exec(at) && find_result(">");
	
	return exec(send, 1000 * 3) && find_result("SEND OK");
}

String SIM800::send_result()
{
	return buffer;
}

bool SIM800::wake(){
	// while (!(exec("AT") && find_result("OK")));
	// return exec("AT+CSCLK=0") && sim_status();
		return exec("AT+CFUN=1") && sim_status();
}
void SIM800::sleep(){
	exec("AT+CFUN=0");
}
void SIM800::http_end(){
	exec("AT+HTTPTERM");
	exec("AT+SAPBR=0,1");
}
bool SIM800::http_init()
{	
	exec("AT+SAPBR=3,1,\"APN\",\"afrihost\"");
	exec("AT+SAPBR=1,1");
	exec("AT+SAPBR=2,1");
	return exec("AT+HTTPINIT") && find_result("OK");
}

bool SIM800::http_send(String send)
{
	 //enable_error_msg();
		exec("AT+HTTPPARA=\"CID\",1");
		String url = SERVER+send;
		char at[url.length() + 42];
		sprintf(at, "AT+HTTPPARA=\"URL\",\"%s\"", url.c_str());
		
		//Serial.println(at);
		exec(at);
		exec("AT+HTTPPARA=\"PROPORT\",\"80\"");
		exec("AT+HTTPACTION=0",10000);
		return  find_result("200") || find_result("202"); // Result expected: "OK" || "Accepted"
	
}

unsigned int SIM800::exec(const char * AT, unsigned int timeout)
{
	clean_buffer();
	buffer = "";
	sim.println(AT);
	unsigned int time_start = millis();
	while (millis() - time_start < timeout)
		if (sim.available())
			buffer += (char)sim.read();
	//Serial.println("");
	//Serial.print(AT);
	//Serial.println(" :");
	//Serial.println("-----------");
	//Serial.println(buffer);
	//Serial.println("-----------");
	return buffer.length();
}

// false -> not show AT, true -> show AT
bool SIM800::echo(bool flag)
{
	char at[10];
	sprintf(at, "ATE%d", flag ? 1 : 0);
	return exec(at) && find_result("OK");
}


bool SIM800::find_result(const char *res1, const char *res2)
{
	//Serial.print("res1:");
	//Serial.println(buffer.indexOf(res1));
	//Serial.print("res2:");
	//Serial.println(buffer.indexOf(res2));
	return buffer.indexOf(res1) != -1 && buffer.indexOf(res2) != -1;
}

void SIM800::clean_buffer()
{
	while (sim.available())
		sim.read();
}

void SIM800::debug()
{
	while (true) {
		if (sim.available()) {
			Serial.write(sim.read());
		}
		if (Serial.available()) {
			sim.write(Serial.read());
		}
	}
}
