--
--      NanoBrainSoCMemory : package for memory controller for NanoBrain SoC
--      
--

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package NanoBrainSoCMemory is

  component NanoBrainSoCMemoryController is
  generic (
    ADDRESS_WIDTH    : natural := 23;
    BR_ADDRESS_WIDTH : natural := 2;
    WORD_WIDTH       : natural := 16;
    BURST_SIZE_BITS  : natural := 4
    );
  port (

    clk   : in std_logic;
    reset : in std_logic;

    address_1    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_1    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_1    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_1 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_1     : in  std_logic;
    rd_req_1     : in  std_logic;
    wr_grant_1   : out std_logic;
    rd_grant_1   : out std_logic;
    wr_done_1    : out std_logic;
    n_rd_1       : in  std_logic;
    n_wr_1       : in  std_logic;

    address_2    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_2    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_2    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_2 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_2     : in  std_logic;
    rd_req_2     : in  std_logic;
    wr_grant_2   : out std_logic;
    rd_grant_2   : out std_logic;
    wr_done_2    : out std_logic;
    n_rd_2       : in  std_logic;
    n_wr_2       : in  std_logic;

    address_3    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_3    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_3    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_3 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_3     : in  std_logic;
    rd_req_3     : in  std_logic;
    wr_grant_3   : out std_logic;
    rd_grant_3   : out std_logic;
    wr_done_3    : out std_logic;
    n_rd_3       : in  std_logic;
    n_wr_3       : in  std_logic;

    address_4    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_4    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_4    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_4 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_4     : in  std_logic;
    rd_req_4     : in  std_logic;
    wr_grant_4   : out std_logic;
    rd_grant_4   : out std_logic;
    wr_done_4    : out std_logic;
    n_rd_4       : in  std_logic;
    n_wr_4       : in  std_logic;

    address_5    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_5    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_5    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_5 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_5     : in  std_logic;
    rd_req_5     : in  std_logic;
    wr_grant_5   : out std_logic;
    rd_grant_5   : out std_logic;
    wr_done_5    : out std_logic;
    n_rd_5       : in  std_logic;
    n_wr_5       : in  std_logic;

    address_6    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_6    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_6    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_6 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_6     : in  std_logic;
    rd_req_6     : in  std_logic;
    wr_grant_6   : out std_logic;
    rd_grant_6   : out std_logic;
    wr_done_6    : out std_logic;
    n_rd_6       : in  std_logic;
    n_wr_6       : in  std_logic;

    address_7    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_7    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_7    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_7 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_7     : in  std_logic;
    rd_req_7     : in  std_logic;
    wr_grant_7   : out std_logic;
    rd_grant_7   : out std_logic;
    wr_done_7    : out std_logic;
    n_rd_7       : in  std_logic;
    n_wr_7       : in  std_logic;

    address_8    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_8    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_8    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_8 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_8     : in  std_logic;
    rd_req_8     : in  std_logic;
    wr_grant_8   : out std_logic;
    rd_grant_8   : out std_logic;
    wr_done_8    : out std_logic;
    n_rd_8       : in  std_logic;
    n_wr_8       : in  std_logic
    );
end component;

component NanoBrainSoCMemoryTemplate is

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

end component;

end NanoBrainSoCMemory;
