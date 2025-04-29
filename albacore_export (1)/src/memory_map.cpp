// Step 5 - Import the memory_map.h file
#include "memory_map.h"

// Step 6-5 - Set up the process method
void memory_map::process(){

  {
    HLS_DEFINE_PROTOCOL("reset");
    reset_memory_map();
  }
  
  {       
    HLS_DEFINE_PROTOCOL("wait");
    wait();
  }

  // Step 8 - Create the Thread
  while(true){

    {
      HLS_DEFINE_PROTOCOL("read_first_half");
      in_addr = addr.read();  // Port 2
      in_din = din.read();    // port 3
      in_we = we.read();      // Port 4
    }

    // Map Memory in First Half - Written From Previous Cycle
    {
      HLS_DEFINE_PROTOCOL("map_memory");
      map_memory();
    }

    {
      // Step 8-5a - Write the protocol
      HLS_DEFINE_PROTOCOL("output");
      
      write_outputs();
     
    } // Step 8.5d - Don't forget the closing }
    
    
    {       
      HLS_DEFINE_PROTOCOL("wait");
      wait();
    }

    // Step 8-3 - Define the input protocol
    {
      // Step 8-3a - Define the input protocol
      HLS_DEFINE_PROTOCOL("input");
      
      // Read the inputs
      read_inputs();
      
    } // Step 8-3c - Don't forget the closing }
    
      // Step 8-4 - Define the "work"
      // The memory map content itself

    // Map Memory in Second Half
    {
      HLS_DEFINE_PROTOCOL("map_memory");
      map_memory();
    }
    
    // Define the final protocol block in the synthesizable blobk
    {       
      HLS_DEFINE_PROTOCOL("wait");
      wait();
    }
    
  } // End the while loop
  
}

void memory_map::reset_memory_map(){

    // Step 7-3 Reset any memory elements
    q_io_out = 0;
    q_io_in = 0;
    we_ram = 0;
    we_io_out = 0;
    dout_ram = 0;

    for(size_t i = 0;i < RAM_SIZE; i++){
      Memory[i] = 0;
    }

    // Step 7-4 - Reset all the output ports
    dout.write(0);   // Port 6
    io_out.write(0); // Port 7

}

void memory_map::read_inputs(){

  // Step 8-3b Read the inputs

  in_mem_addr = mem_addr.read();  // Port 2
  in_mem_din = mem_din.read();    // port 3
  in_mem_we = mem_we.read();      // Port 4

  in_io_in = io_in.read(); // Port 5
  
}

void memory_map::write_outputs(){

  // Step 8.5b - Write to the outputs

  cout << "out_dout " << out_dout << " " << sc_time_stamp() << endl;

  dout.write(out_dout);       // Port 6
  io_out.write(out_io_out);   // Port 7
  
}

void memory_map::map_memory(){

  // I/O Input Register
  q_io_in = in_io_in;

  // Write Enable Decoder
  we_ram = 0;
  out_io_out = 0;
  if(in_addr[MSB_MEM] == 0){
    we_ram = in_we;
  }
  else{
    if(in_addr == sc_bv<ADDR_W>(0x8000)){
      out_io_out = in_we;
    }
  }

  // Write to the memory address
  if(in_mem_we){
    Memory[in_mem_addr.to_uint()] = in_mem_din;
  }
  else if(in_we){
    Memory[in_addr.to_uint()] = in_din;
  }

  dout_ram = Memory[in_addr.to_uint()];

  // Output Multiplexer
  if(in_addr[MSB_MEM] == 0){
    out_dout = dout_ram;
  }  
  else if( in_addr == 0x8001 ){
    out_dout = ( sc_bv<12>(0), q_io_in );
  }
  else{
    out_dout = 0;
  }

}


bool memory_map::test_reset_memory_map(){
  
  for(size_t i = 0;i < RAM_SIZE; i++){

    if( Memory[i] != 0 ){

      std::cout << "Unexpected: Memory[" << i << "] = " << Memory[i] << std::endl;
      return false;
    }
  }

  return true;
  
}
