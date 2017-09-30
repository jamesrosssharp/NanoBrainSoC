-- Quartus II VHDL Template
-- Single port RAM with single read/write address 

library ieee;
use ieee.std_logic_1164.all;

entity NanoBrainSoCMemoryTemplate is

  generic
    (
      DATA_WIDTH : natural := 16;
      ADDR_WIDTH : natural := 11
      );

  port
    (
      clk  : in  std_logic;
      addr : in  natural range 0 to 2**ADDR_WIDTH - 1;
      data : in  std_logic_vector((DATA_WIDTH - 1) downto 0);
      we   : in  std_logic := '1';
      q    : out std_logic_vector((DATA_WIDTH - 1) downto 0)
      );

end entity;

architecture rtl of NanoBrainSoCMemoryTemplate is

  -- Build a 2-D array type for the RAM
  subtype word_t is std_logic_vector((DATA_WIDTH - 1) downto 0);
  type memory_t is array(2**ADDR_WIDTH - 1 downto 0) of word_t;

  -- Declare the RAM signal.    
  signal ram : memory_t := (0      => "0000000000000100",  -- IMM 0x0004
                            1      => "0110101000001000",  -- LOAD r0, 0x8
                            2      => "0110101000010000",  -- LOAD r1, 0x0
                            3      => "0100011100000000",  -- NOP
                            4      => "0100011100000000",  -- NOP
                            5      => "0100011100000000",  -- NOP
                            6      => "1111110000000001",  -- OUT  r0, r1 (0x48 = 'H')
                            7      => "0000000000000110",  -- IMM 0x0006
                            8      => "0110101000000101",  -- LOAD r0, 0x5
                            9      => "0100011100000000",  -- NOP
                            10     => "0100011100000000",  -- NOP
                            11     => "0100011100000000",  -- NOP
                            12     => "1111110000000001",  -- OUT  r0, r1 (0x65 = 'e')
                            13     => "1000010000000000",  -- JMP 0x000000
                            others => "1111111111111111"
                            );

  -- Register to hold the address 
  signal addr_reg : natural range 0 to 2**ADDR_WIDTH - 1;

begin

  process(clk)
  begin
    if(rising_edge(clk)) then
      if(we = '1') then
        ram(addr) <= data;
      end if;

      -- Register the address for reading
      addr_reg <= addr;
    end if;
  end process;

  q <= ram(addr_reg);

end rtl;
