# Select based TCP messaging server
This repository contains code for a basic **concurrent TCP messaging server** which reads the client request using **select system call** and iterates over them rather than creating a child process for every client.

#### Compiling
* Type the following command to generate executable __server__  

	``` sh
	$ make
	```
	
	
	
#### Running
* To run the __server__ type 
	
	``` sh
	$ ./server <port>
	```  
	
	**port** is the port number on which TCP messaging server will listen requests. Default port is 6500.

* **Telnet** could be used as *client* application. To run **client** through telnet type following command 
	
	``` sh
	$ telnet localhost <port>
	```  
	
	**port** is the port number on which TCP messaging server is running.
	
* To **add** a client to a **particular group** type the following message in telnet    
	
	```sh
	GROUP$<ID>
	```  
	
	here **ID** is the group number to which client should be added.Value of ID could be from 0 to MAX_GROUPS defined in header.  
	For e.g. typing    
	
	```sh
	GROUP$2
	```  
	
	will add the client to group number 2.
	
* To **send** the message to all the **clients in a group** type following message in telnet    

	```sh
	GROUPMSG$<ID>$<MSG>
	```  
	
	here **ID** is the group number to which message should be sent and **MSG** is message to be sent.  
	For e.g. typing  
	
	```sh
	GROUPMSG$2$AASD
	```  
	
	will send AASD to all the clients present in group 2.
	
* To **send** the message to **all the clients** connected irrespective of group type the message directly in telnet.  
For e.g. typing    

	```sh
	asad
	```  
	
	will send asad to all the clients connected.    

#### Basic Explanation
* Client connects to server. Server adds client's file descriptor into an array.
* Clients send messages to server to which server responds by selecting file descriptors iteratively through select system all.
* If client requests to be added in a group add client's file descriptor into a 2D array of groupinfo having row as group number and column having client's file descriptors.
* If message is to be sent to all the clients in a group iterate through that group's row in groupinfo array and send message to all the clients.
* If message is to be sent to all the clients iterate through file descriptor array and send message to all the clients.  


#### TODOS
* Add command to remove client from group.
