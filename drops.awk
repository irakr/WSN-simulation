#!/usr/bin/awk -f

# Packet drop percentage

BEGIN{
	pktdrop = 0.0
	dropPercent = 0.0
	packetId = 0
	prev_packetId = 0
	totalPackets = 1
	nodes = 0
}
{
	event = $1
	packetId = $9
	
	if(event == "d")
		pktdrop++
	
	if(packetId > prev_PacketId)
		totalPackets++
	
	prev_packetId = packetId
}
END{
	#printf("Number of packets dropped = %d\n",pktdrop)
	dropPercent = (pktdrop/totalPackets) * 100.0
	printf("%d %lf\n", nodes, dropPercent);
}
