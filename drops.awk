BEGIN{
	pktdrop = 0;
}
{
	event = $1;
	if(event == "d")
	{
		pktdrop++;
	}
}

END{
	printf("Number of packets dropped = %d\n",pktdrop);
}
