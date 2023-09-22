#ifndef ALPHA_CONFIG_H
#define ALPHA_CONFIG_H

#define DEFAULT_RECV_HOST "127.0.0.1"
#define DEFAULT_RECV_PORT 8010
#define DEFAULT_POST_HOST "127.0.0.1"
#define DEFAULT_POST_PORT 5700

typedef struct alphaConfig {
	std::string owner_id;
	std::string recv_host;
	std::string post_host;
	std::string sauce_apikey;
	std::string gocq_folder;
	int recv_port;
	int post_port;
};

#endif