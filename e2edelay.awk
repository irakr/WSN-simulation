# simple awk script to generate end-to-end packet lifetime statistics 
BEGIN { 
   highest_packet_id = 0; 
} 
{ 
   action = $1; 
   time = $2; 
   s = $3; 
   d = $4; 
   energy_s = $5; 
   energy_d = $6;  
   pkt_type = $7;
   pkt_size = $8
   packet_id = $9; 
   if ( packet_id > highest_packet_id )
   	highest_packet_id = packet_id; 

   # getting start time is not a problem, provided you're not starting 
   # traffic at 0.0. id here. 
   if ( start_time[packet_id] == 0 )  start_time[packet_id] = time; 
   # only useful for small unicast where packet_id doesn't wrap. 
   # checking receive means avoiding recording drops 
   if ( action != "d" ) { 
      if ( action == "r" ) { 
         # could test for receiving node_2_address or flow_id here. 
         end_time[packet_id] = time; 
      } 
   } else { 
      end_time[packet_id] = -1; 
   } 
}                                                          
END { 
    for ( packet_id = 0; packet_id <= highest_packet_id; packet_id++ ) { 
       start = start_time[packet_id]; 
       end = end_time[packet_id]; 
       packet_duration = end - start; 
       if ( start < end ) printf("%f %f\n", start, packet_duration); 
   } 
} 
