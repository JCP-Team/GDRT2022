// SIM800.h


#ifndef _SIM800_h
#define _SIM800_h


#include "Arduino.h"
#include <SoftwareSerial.h>


#define DEFAULT_TIMEOUT 1000

class SIM800 {
	SoftwareSerial sim;
	String buffer;
	String SERVER; 
	

	bool find_result(const char *res_1, const char *res_2 = "");
	
	void clean_buffer();

public:

	SIM800(const int TX_PIN = 8, const int RX_PIN = 9,
		const int baud_rate = 9600) :sim(TX_PIN, RX_PIN) {
		sim.begin(baud_rate);
	}

	bool init(String server);
	void start(){
	sim.begin(9600);
	};
	
	bool restart();

	bool sim_status();

	bool net_registration();

	bool gprs_status();

	bool apn(const char *apn = "CMNET");

	bool create_tcp(const char *ip, unsigned int port);

	bool tcp_send(const char *send);
	
	bool http_init();
	
	bool http_send(String send);
	void http_end();

	String send_result();

	bool echo(bool flag); 

	void debug();
	bool wake();
	void sleep();
	bool create_tcp_server(unsigned int port);

	void enable_error_msg();
	void disable_error_msg();

	unsigned int exec(const char *AT, unsigned int timeout = DEFAULT_TIMEOUT);

};

#endif

