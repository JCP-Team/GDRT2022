// SIM800C.h


#ifndef _SIM800C_h
#define _SIM800C_h


#include "Arduino.h"
#include <SoftwareSerial.h>


#define DEFAULT_TIMEOUT 1000

class SIM800C {
	SoftwareSerial sim;
	String buffer;
	String SERVER ="http://ie-gtfs.up.ac.za/data/z-nano.php";
	

	bool find_result(const char *res_1, const char *res_2 = "");
	
	void clean_buffer();

public:

	SIM800C(const int TX_PIN = 8, const int RX_PIN = 9,
		const int baud_rate = 9600) :sim(TX_PIN, RX_PIN) {
		sim.begin(baud_rate);
	}

	bool init();
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
	
	String http_send(String send);
	void http_end();

	String send_result();

	unsigned long get_num_localip();
	
	String get_str_localip();

	bool echo(bool flag); 

	void debug();
	bool wake();
	void sleep();
	bool create_tcp_server(unsigned int port);

	bool multi_link_mode(bool flag);

	void enable_error_msg();
	void disable_error_msg();

	unsigned int exec(const char *AT, unsigned int timeout = DEFAULT_TIMEOUT);

	bool base_station_position(double &longitude, double &latitude, unsigned int &precision);
};

#endif

