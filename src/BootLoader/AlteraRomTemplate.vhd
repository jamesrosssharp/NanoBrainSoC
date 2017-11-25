-- Quartus II VHDL Template
-- Single-Port ROM

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Memory is

  generic
    (
      DATA_WIDTH : natural := 16;
      ADDR_WIDTH : natural := 10
      );

  port
    (
      clk  : in  std_logic;
      addr : in  natural range 0 to 2**ADDR_WIDTH - 1;
      q    : out std_logic_vector((DATA_WIDTH -1) downto 0)
      );

end entity;

architecture rtl of Memory is

  -- Build a 2-D array type for the RoM
  subtype word_t is std_logic_vector((DATA_WIDTH-1) downto 0);
  type memory_t is array(1023 downto 0) of word_t;

  -- Declare the ROM signal and specify a default value.        Quartus II
  -- will create a memory initialization file (.mif) based on the 
  -- default value.
  signal rom : memory_t := ( %ROM_DATA% );

begin

  process(clk)
  begin
    if(rising_edge(clk)) then
      q <= rom(addr);
    end if;
  end process;

end rtl;
