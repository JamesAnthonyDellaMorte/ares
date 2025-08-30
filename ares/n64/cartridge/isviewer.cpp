auto Cartridge::ISViewer::readHalf(u32 address) -> u16 {
  address = (address & 0xffff);
  return ram.read<Half>(address);
}

auto Cartridge::ISViewer::readWord(u32 address) -> u32 {
  address = (address & 0xffff);
  return ram.read<Word>(address);
}

auto Cartridge::ISViewer::messageChar(char c) -> void {
  if(!tracer->enabled()) return;
  tracer->notify(c);
}

auto Cartridge::ISViewer::writeHalf(u32 address, u16 data) -> void {
  address = (address & 0xffff);
  // printf("[ISViewer] writeHalf: addr=0x%04x data=0x%04x\n", address, data);

  if(address == 0x16) {
    printf("[ISViewer] Trigger! Length=%d\n", data);
    // Output the buffer contents
    for(auto i : range(data)) {
      char c = ram.read<Byte>(0x20 + i);
      printf("%c", c);
      // messageChar(c); // Don't double output
    }
    printf("\n");
    return;
  }

  // Write bytes individually to avoid endian issues
  ram.write<Byte>(address + 0, (data >> 8) & 0xFF);
  ram.write<Byte>(address + 1, (data >> 0) & 0xFF);
}

auto Cartridge::ISViewer::writeWord(u32 address, u32 data) -> void {
  address = (address & 0xffff);
  
  // Debug: Show all non-zero writes - COMMENTED OUT
  // if(data != 0) {
  //   printf("[ISViewer] writeWord: addr=0x%04x data=0x%08x", address, data);
  //   // Show as chars if in buffer range
  //   if(address >= 0x20 && address < 0x60) {
  //     printf(" chars='%c%c%c%c'", 
  //       (data >> 24) & 0xFF ? (char)((data >> 24) & 0xFF) : '.',
  //       (data >> 16) & 0xFF ? (char)((data >> 16) & 0xFF) : '.',
  //       (data >> 8) & 0xFF ? (char)((data >> 8) & 0xFF) : '.',
  //       (data >> 0) & 0xFF ? (char)((data >> 0) & 0xFF) : '.');
  //   }
  //   printf("\n");
  // }
  
  // Check if this write includes the trigger address 0x14 BEFORE writing
  if(address == 0x14) {
    // Extract the length from the lower 16 bits (at offset 0x16-0x17)
    u16 length = data & 0xFFFF;
    
    // Just output the message directly without extra formatting
    for(auto i : range(length)) {
      char c = ram.read<Byte>(0x20 + i);
      printf("%c", c);
      // messageChar(c); // Don't double output
    }
    
    // Don't write the trigger value to RAM - we've already handled it
    return;
  }
  
  // Write bytes directly to avoid duplication from writeHalf
  ram.write<Byte>(address + 0, (data >> 24) & 0xFF);
  ram.write<Byte>(address + 1, (data >> 16) & 0xFF);
  ram.write<Byte>(address + 2, (data >> 8) & 0xFF);
  ram.write<Byte>(address + 3, (data >> 0) & 0xFF);
}