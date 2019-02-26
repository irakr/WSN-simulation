- Format
	<event> <time> <sender> <destination> <sender_energy> <dest_energy> <Packet_type> <Packet_size> <Packet_ID>

- Values
	<event> =
	e -> Sense Event
	+ -> Packet enqueue
	- -> Packet dequeue
	r -> Packet received
	n -> Broadcast relax packet
	
	<destination> =
	integer
	-2 -> Broadcast ID
	
	<dest_energy> =
	double
	-1 -> Energy of broadcast address as destination


